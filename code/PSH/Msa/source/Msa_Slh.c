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

/* !\file maslh.c
 * Logic for SMIL slide handling. 
 */

/* WE */
#include "We_Mem.h"
#include "We_Lib.h"
#include "We_Log.h"
#include "We_Core.h"

/* MMS */
#include "Mms_Def.h"

/* SIS */
#include "Sis_Cfg.h"
#include "Sis_If.h"

/* MSA*/
#include "Msa_Def.h"
#include "Msa_Types.h"
#include "Msa_Cfg.h"
#include "Msa_Slh.h"
#include "Msa_Mem.h"
#include "Msa_Moh.h"
#include "Msa_Intsig.h"
#include "Msa_Utils.h"
#include "Msa_Uicmn.h"
#include "Msa_Conf.h"

/******************************************************************************
 * Macros
 *****************************************************************************/

/* Default sizes used when calculating the message size*/
#define MSA_ENTRY_SIZE              (0x40)  /*!< The size of one entry excluding dynamic data*/
#define MSA_HEADER_SIZE             (100)   /*!< The header size excluding dynamic data */
#define MSA_SMIL_SIZE               (0xf0)  /*!< The SMIL size excluding dynamic data  */
#define MSA_SMIL_PAR_SIZE           (30)    /*!< The size of on "par" tag */
#define MSA_SMIL_MO_SIZE            (30)    /*!< The header size for one media object */

/* Definitions for region types when creating a SMIL presentation */
#define MSA_IMAGE_REGION            "Image" /*!< The image rgion definition */
#define MSA_TEXT_REGION             "Text"  /*!< The text rgion definition */

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static WE_BOOL createSisObject(const MsaObjInfo *objInfo, 
    SisMediaObject **currentObj, WE_UINT32 slideDuration);
static void setRegions(const MsaSmilInfo *smilInfo, SisRegion *textRegion, 
    SisRegion *imageRegion);
static void initObjInfo(MsaObjInfo *objInfo, MsaMoDataItem *item, 
    WE_UINT32 textColour);
static void freeSisMediaObject(SisMediaObject **object);
static MsaObjInfo *findObjectInSlide(const MsaSmilSlide *sSlide, 
    MsaMediaGroupType objType, unsigned int *index);
static WE_BOOL findEmptyObjectSlot(const MsaSmilSlide *slide, 
    unsigned int *index);
static WE_BOOL insertByOrder(MsaSmilInfo *sInfo, MsaSmilSlide *sSlide, 
    MsaMoDataItem *item);
static WE_UINT32 calculateAddrSize(const MsaAddrItem *addr);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Creates an instance for creation of a multimedia message
 *
 * \param smilInfo Where to put the new instance.
 * \return TRUE on successful creation, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaCreateSmilInstance(MsaSmilInfo **smilInfo)
{
    if (NULL == *smilInfo)
    {
        *smilInfo = MSA_ALLOC(sizeof(MsaSmilInfo));
        memset(*smilInfo, 0, sizeof(MsaSmilInfo));
        (*smilInfo)->bgColor = SIS_COLOR_NOT_USED;  /* Do not use any default color*/
        (*smilInfo)->fgColorDefault =MSA_CFG_DEFAULT_SMIL_TEXT_COLOR;
    }
    else
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) SLH instance is busy!\n", __FILE__, __LINE__));
        return FALSE;
    }
    return TRUE;
}

/*! 
 * \brief De-allocates a SMIL creation instance.
 *
 * \param smilInfo The current instance.
 *****************************************************************************/
void msaCleanupSmilInstance(MsaSmilInfo **smilInfo)
{
    if (NULL != *smilInfo)
    {
        msaDeleteAllSlides(*smilInfo);
        /*
         *	Optimize deallocates objects not referenced by any SMIL slides, i.e. 
         *  removes all media objects in the case of multipart/mixed.
         */
        msaOptimizeMoList(*smilInfo);
        /* Remove the SMIL presentation */
        MSA_FREE(*smilInfo);
        *smilInfo = NULL;
    }
}

/*!
 * \brief Get the total number of slides.
 * 
 * \param smilInfo The current instance.
 * \return The total number of slides.
 *****************************************************************************/
unsigned int msaGetSlideCount(const MsaSmilInfo *smilInfo)
{
    if (NULL != smilInfo)
    {
        return smilInfo->noOfSlides;
    }
    return 0;
}

/*!
 * \brief Deletes all slides
 *
 * \param smilInfo The current SMIl presentation.
 *****************************************************************************/
void msaDeleteAllSlides(MsaSmilInfo *smilInfo)
{
    /* Delete all slides */
    while (msaDeleteSlide(smilInfo))
    {
    }

    smilInfo->slides        = (struct MsaSmilSlideSt *)NULL;
    smilInfo->currentSlide  = (struct MsaSmilSlideSt *)NULL;
    smilInfo->noOfSlides    = 0;
} /* deleteAllSlides */

/*!
 * \brief Deletes a slide
 *
 * \param smilInfo All slides.
 * \return TRUE on ok or FALSE on error
 *****************************************************************************/
WE_BOOL msaDeleteSlide(MsaSmilInfo *smilInfo)
{
    MsaSmilSlide *delSlide;
    MsaSmilSlide *prev;
    MsaSmilSlide *current;
    unsigned int i;

    delSlide = smilInfo->currentSlide;
    /* Check that the "current" slide exists */
    if ((NULL != smilInfo->currentSlide) && (smilInfo->slides != NULL))
    {
        /* Free all data that is associated with the slide */
        for (i = 0; i < MSA_MAX_SMIL_OBJS; i++)
        {
            if ((NULL != delSlide->objects[i]) && 
                (NULL != delSlide->objects[i]->object))
            {
                /* Delete the object data */
                msaDeleteMo(&(smilInfo->mediaObjects), smilInfo->slides, 
                    delSlide->objects[i]->object);
                MSA_FREE(delSlide->objects[i]);
                delSlide->objects[i] = NULL;
            }
        }
        prev = NULL;
        current = smilInfo->slides;
        /* Find the current item and its predecessor */
        while (delSlide != current)
        {
            prev = current;
            current = current->next;
        }
       
        /* Set new active slide */
        if (NULL != prev)
        {
            smilInfo->currentSlide = prev;
            prev->next = delSlide->next;
        }
        else if (NULL != delSlide->next)
        {
            smilInfo->currentSlide = delSlide->next;
            if (smilInfo->slides == delSlide)
            {
                smilInfo->slides = smilInfo->currentSlide;
            }
        }
        else
        {
            smilInfo->currentSlide = NULL;
            smilInfo->slides = NULL;
        }
        smilInfo->noOfSlides--;
        /* Free the slide */
        MSA_FREE(delSlide);
        return TRUE;
    }
    return FALSE;
} /* msaDeleteSlide */

/*!
 * \brief Gets the currently active slide.
 *
 * \param smilInfo The current multimedia presentation.
 * \return The current slide
 *****************************************************************************/
MsaSmilSlide *msaGetCurrentSlide(const MsaSmilInfo *smilInfo)
{
    return smilInfo->currentSlide;
} /* msaGetCurrentSlide */

/*!
 * \brief Gets the duration for the current slide.
 *
 * \param smilInfo The current multimedia presentation.
 * \return  The duration in milliseconds.
 *****************************************************************************/
unsigned int msaGetDuration(const MsaSmilInfo *smilInfo)
{
    if (NULL != smilInfo->currentSlide)
    {
        return (unsigned int)smilInfo->currentSlide->duration;
    }
    return MSA_CFG_DEFAULT_DURATION;    
}

/*!
 * \brief Gets the pointer to the first slide
 * \param smilInfo The current multimedia presentation.
 * \return Pointer to the first slide or NULL.
 *****************************************************************************/
MsaSmilSlide *msaGetFirstSlide(const MsaSmilInfo *smilInfo)
{
    return smilInfo->slides;
} /* msaGetFirstSlide */

/*!
 * \brief Gets the next slide
 * \param smilInfo The current multimedia presentation.
 * \return The next slide
 *****************************************************************************/
MsaSmilSlide *msaGetNextSlide(MsaSmilInfo *smilInfo)
{
    if (NULL != smilInfo->currentSlide->next)
    {
        smilInfo->currentSlide = smilInfo->currentSlide->next;
        return smilInfo->currentSlide;
    }
    else
    {
        return smilInfo->currentSlide;
    } /* if */
} /* msaGetNextSlide */

/*!
 * \brief Gets the previous slide
 * \param smilInfo The current multimedia presentation.
 * \return The previous slide or NULL.
 *****************************************************************************/
MsaSmilSlide *msaGetPreviousSlide(MsaSmilInfo *smilInfo)
{
    MsaSmilSlide *current;
    current = smilInfo->slides;
    
    while((NULL != current) && (current->next != smilInfo->currentSlide))
    {
        current = current->next;
    } /* while */

    if (current == NULL)
    {
        return NULL;
    } 
    else if (current->next == smilInfo->currentSlide)
    {
        smilInfo->currentSlide = current;
        return current;
    }
    else
    {
        return current;
    } /* if */
} /* msaGetPreviousSlide */

/*!
 * \brief Retrieves the index of the current slide
 * \param smilInfo The current SMIL presentation.
 * \param slide The slide to find the index of.
 * \return The index of the slide counting from 0.
 *****************************************************************************/
unsigned int msaGetSlideIndex(const MsaSmilInfo *smilInfo, 
    const MsaSmilSlide *slide)
{
    MsaSmilSlide *current = smilInfo->slides;
    unsigned int index = 0;
    while ((NULL != current) && (current != slide))
    {
        current = current->next;
        index ++;
    } /* while */
    return index;
} /* msaGetSlideIndex */

/*!
 * \brief Adds a new slide to the list of slides after the current one,
 *          and updates the current-pointer and slide count.
 * \param smilInfo The current SMIL presentation.
 * \return The new slide, or NULL.
 *****************************************************************************/
MsaSmilSlide *msaInsertSlide(MsaSmilInfo *smilInfo)
{
    MsaSmilSlide *newSlide;
    
    newSlide = (MsaSmilSlide*)MSA_ALLOC(sizeof(MsaSmilSlide));
    memset(newSlide, 0, sizeof(MsaSmilSlide));
    
    if (NULL != smilInfo->currentSlide)
    {
        newSlide->next = smilInfo->currentSlide->next;
        smilInfo->currentSlide->next = newSlide;
    }
    else
    {
        smilInfo->slides = newSlide;
        newSlide->next = NULL;
    }
    smilInfo->noOfSlides++;
    smilInfo->currentSlide = newSlide;
    /* Set the default duration */
    msaSetDuration(smilInfo, (unsigned)msaGetConfig()->slideDuration * 1000);
    return newSlide;
} /* msaInsertSlide */

/*!
 * \brief Sets the duration for the current slide.
 *
 * \param smilInfo The current multimedia presentation.
 * \param duration The new duration in milliseconds.
 *****************************************************************************/
void msaSetDuration(const MsaSmilInfo *smilInfo, unsigned int duration)
{
    MsaObjInfo *objInfo;
    unsigned int i;

    if (NULL != smilInfo->currentSlide)
    {
        smilInfo->currentSlide->duration = duration;
        /* Adjust the item timing to be valid */
        for (i = 0; i < MSA_MAX_SMIL_OBJS; i++)
        {
            if (NULL != (objInfo = smilInfo->currentSlide->objects[i]))
            {
                if (objInfo->begin > duration ||
                    duration - objInfo->begin < MSA_OBJ_DURATION_MIN)
                {
                    /* If object begin is outside of page timing then 
                       adjust */
                    objInfo->begin = 0;
                }
                if (objInfo->duration != 0)
                {
                    /* Only need to check duration timing if not maximum */
                    if (objInfo->begin + objInfo->duration > duration)
                    {
                        /* If object timing is equal to or outside of page
                           timing then adjust to maximum duratio (=0) */
                        objInfo->duration = 0;
                    }
                }
            }
        }
    }
}

/* ! Sets the default values for a #MsaobjInfo structure.
 *
 * \param objInfo The structure to init.
 * \param item The #MsamoDataItem that is attached to the slide
 * \param slideDuration The duration of the current page.
 *****************************************************************************/
static void initObjInfo(MsaObjInfo *objInfo, MsaMoDataItem *item, 
    WE_UINT32 textColour)
{
    memset(objInfo, 0, sizeof(MsaObjInfo));
    objInfo->textColor =textColour;
    objInfo->object    = item;
    /* As default: display the object as long as the page is displayed. */
    /* duration = 0 msans max available time (slide time - start time) */
    objInfo->begin     = 0;
    objInfo->duration  = 0; 
}

/*!
 * \brief Sets a media object for the current slide.
 * 
 * \param smilInfo The current instance.
 * \param buffer The media content to set.
 * \param bufferSize The size of the content to add.
 * \param name The unique name of the content.
 *****************************************************************************/
MsaMoDataItem *msaSetSlideMo(MsaSmilInfo *smilInfo, MmsContentType *mmsType, 
    unsigned char *buffer, unsigned int bufferSize, 
    char *name)
{
    MsaMoDataItem *item;
    MsaSmilSlide *sSlide;

    /* get current slide */
    if (NULL == (sSlide = msaGetCurrentSlide(smilInfo)))
    {
        return NULL;
    }
    /* Get the object, the msaAddMo returns a new object or an existing one */
    if (NULL == (item = msaAddMo(&(smilInfo->mediaObjects), buffer, 
        bufferSize, mmsType, name)))
    {
        return NULL;
    }
    
    /* Find the slot */
    if (insertByOrder(smilInfo, sSlide, item))
    {
        return item;
    }
    return NULL;
}

/*! Gets information about a an object referenced by the current slide
 *
 * \param sInfo The current presentation.
 * \param type The type of object to find
 * \param objInfo The resulting object is placed here.
 * \return TRUE if a object of the correct type was found otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaGetObjInfo(const MsaSmilInfo *sInfo, MsaMediaGroupType objType, 
    MsaObjInfo **objInfo)
{
    unsigned int i;
    MsaSmilSlide *sSlide = msaGetCurrentSlide(sInfo);
    if (NULL == sSlide)
    {
        return FALSE;
    }
    *objInfo = findObjectInSlide(sSlide, objType, &i);
    return (NULL != *objInfo) ? TRUE: FALSE;
}

/*!
 * \brief Get the current media object.
 * 
 * \param smilInfo The current instance.
 * \param objType The type of object to return.
 * \return A reference to the item or NULL.
 *****************************************************************************/
MsaMoDataItem *msaGetMo(const MsaSmilInfo *smilInfo, MsaMediaGroupType objType)
{
    MsaSmilSlide *sSlide = msaGetCurrentSlide(smilInfo);
    MsaObjInfo *objInfo;
    unsigned int i;
    /* Check the current slide */
    if (NULL != sSlide)
    {
        /* Find the media object */
        if (NULL != (objInfo = findObjectInSlide(sSlide, objType, &i)))
        {
            return objInfo->object;
        }
    }
    return NULL;
}

/*! \brief Calculates the size of a list of addresses.
 *
 * \param addr The list of addresses.
 * \return The size in bytes.
 *****************************************************************************/
static WE_UINT32 calculateAddrSize(const MsaAddrItem *addr)
{
    WE_UINT32 size = 0;
	while (NULL != addr) 
	{
		size += (NULL != addr->address) ? (strlen(addr->address) + 1) : 0;
		size += (NULL != addr->name)    ? (strlen(addr->name)    + 1) : 0;
        addr = addr->next;
	} /* while */
    return size;
}

/*!
 * \brief Calculate the approximate size of the message.
 *
 * \param The current instance.
 * \param The current message properties.
 * \return The approximate size of the message.
 *****************************************************************************/
WE_UINT32 msaCalculateMsgSize(const MsaSmilInfo *smilInfo, 
    const MsaPropItem *propItem)
{
    unsigned int msgSize = 0;
    MsaMoDataItem *moItem;
    MsaSmilSlide *sSlide;
    unsigned int i;

    /* Message header */
    /* Static data estimation */
    msgSize += MSA_HEADER_SIZE;
    /* Subject */
    if (NULL != propItem->subject)
    {
        msgSize += strlen(propItem->subject) + 1;
    }
    /* To, Cc and Bcc sizes */
    msgSize += calculateAddrSize(propItem->to);
    msgSize += calculateAddrSize(propItem->cc);
    msgSize += calculateAddrSize(propItem->bcc);
    
    /* Body parts */
    moItem = smilInfo->mediaObjects;
    while (NULL != moItem)
    {
        /* Body part */
        msgSize += moItem->size + MSA_ENTRY_SIZE;
        moItem = moItem->next;
    }
    /* The SMIL presentation */
    msgSize += MSA_SMIL_SIZE;
    sSlide = smilInfo->slides;
    while (NULL != sSlide)
    {
        /* Add the size of each par  */
        msgSize += MSA_SMIL_PAR_SIZE;
        for (i = 0; i < MSA_MAX_SMIL_OBJS; i++)
        {
            if (NULL != sSlide->objects[i])
            {
                msgSize += MSA_SMIL_MO_SIZE;
            }
            else
            {
                break;
            }
        }
        sSlide = sSlide->next;
    }
    /* return the approximate size of the message */
    return msgSize;
}

/*!
 * \brief Deletes a media object from the current slide.
 * 
 * \param The current SMIL presentation
 * \param The object type to delete.
 * \return TRUE if a media object was deleted, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaDeleteSlideMo(MsaSmilInfo *smilInfo, MsaMediaGroupType objType)
{
    unsigned int i;
    unsigned int ii;
    MsaSmilSlide *slide;
    MsaObjInfo *objInfo = NULL;
    if (NULL == (slide = msaGetCurrentSlide(smilInfo)))
    {
        return FALSE;
    }
    /* Get object according to the object type */
    if (NULL == (objInfo = findObjectInSlide(slide, objType, &i)))
    {
        return FALSE;
    }
    /* Delete the object from the list of objects */
    msaDeleteMo(&(smilInfo->mediaObjects), smilInfo->slides, 
        objInfo->object);
    /* Reorder the list of current objects */
    for (ii = i = 0; i < MSA_MAX_SMIL_OBJS; i++)
    {
        if (objInfo != slide->objects[i])
        {
            slide->objects[ii] = slide->objects[i];
            ++ii;
        }
    }
    /* The last entry must be set to NULL */
    slide->objects[MSA_MAX_SMIL_OBJS - 1] = NULL;
    /*lint -e{774} */
    MSA_FREE(objInfo);
    objInfo = NULL;

    return TRUE;
}

/*! \brief Removes all media-objects that does not have any reference in the
 *         Slide list.
 *
 * \param smilInfo A reference to the current SMIL presentation.
 *****************************************************************************/
void msaOptimizeMoList(MsaSmilInfo *smilInfo)
{
    MsaSmilSlide  *slide;
    MsaMoDataItem *mo;
    MsaMoDataItem *tmpMo;
    unsigned int i;
    WE_BOOL refFound;
    if (NULL != smilInfo)
    {
        /* Go through all media objects */
        mo = smilInfo->mediaObjects;
        while (NULL != mo)
        {
            /* Check that the current media object has a reference */
            slide = smilInfo->slides;
            refFound = FALSE;
            while((NULL != slide) && !refFound)
            {
                for (i = 0; i < MSA_MAX_SMIL_OBJS; i++)
                {
                    if (NULL != slide->objects[i])
                    {
                        if (slide->objects[i]->object == mo)
                        {
                            refFound = TRUE;
                            break;
                        }
                    }
                    else if (NULL == slide->objects[i])
                    {
                        break;
                    }
                }
                slide = slide->next;
            }
            /* Save the pointer if the object needs to be removed */
            tmpMo = mo;
            /* Check next object */
            mo = mo->next;
            /* Check if any reference has been found */
            if (!refFound)
            {
                /* Delete the object since it has no reference */
                msaDeleteMo(&(smilInfo->mediaObjects), smilInfo->slides, 
                    tmpMo);
            }
        }
    }
}

/*!
 * \brief Create a SIS object.
 *
 * \param objInfo The current media onject.
 * \param currentObj Where to put the new object.
 *
 * \return TRUE if successful, otherwise FALSE
 *****************************************************************************/
static WE_BOOL createSisObject(const MsaObjInfo *objInfo, 
    SisMediaObject **currentObj, WE_UINT32 slideDuration)
{
    int size;
    const char *regionType;
    SisMediaObject *obj = NULL;

    /* Sanity check */
    if ((NULL == objInfo->object) || (NULL == objInfo->object->name))
    {
        *currentObj = NULL;
    }
    else
    {
        /* MO */
        MSA_CALLOC(obj, sizeof(SisMediaObject));
        /* src */
        size = (signed)strlen(objInfo->object->name) + 1;
        obj->src = MSA_ALLOC((unsigned)size);
        strcpy(obj->src, (char*)objInfo->object->name);
        
        /* region */
        regionType = NULL;
        switch(msaMimeToObjGroup((char *)objInfo->object->type->strValue))
        {
        case MSA_MEDIA_GROUP_TEXT:
            regionType = MSA_TEXT_REGION;
            obj->color = objInfo->textColor;
            break;
        case MSA_MEDIA_GROUP_IMAGE:
            regionType = MSA_IMAGE_REGION;
            break;
        case MSA_MEDIA_GROUP_AUDIO:
            break;
        case MSA_MEDIA_GROUP_NONE:
            return FALSE;
        }
        /* Begin and end time */
        obj->beginT   = objInfo->begin;
        obj->duration = objInfo->duration == 0 ?
                        slideDuration - objInfo->begin:
                        objInfo->duration;
        obj->endT     = 0;  /* Not used */
        /* Check if any region needs to be set */
        if (NULL != regionType)
        {
            size = (signed)strlen(regionType) + 1;
            obj->region = MSA_ALLOC((unsigned)size);
            strcpy(obj->region, regionType);
        }
        else
        {
            obj->region = NULL;
        }
        
        /* type */
        obj->type = msaMimeToSis(
            (char *)objInfo->object->type->strValue);
    } /* if */
    *currentObj = obj;
    return TRUE;
}

/*!
 * \brief Creates the text part of a SMIL presentation.
 *
 * \param smilInfo The current SMIL presentation
 * \param buffer The SMIL text part or NULL.
 * \param size The size of the SMIL part
 *
 * \return TRUE if the SMIL was created successfully, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaCreateSmil(MsaSmilInfo *smilInfo, char **buffer, 
    unsigned int *size)
{
    int status =    TRUE;
    MsaSmilSlide    *old;
    char            *smilBuffer = NULL;

    SisRootLayout rootLayout;
    SisRegion   textRegion;
    SisRegion   imageRegion;
    WeSize     displaySize;
    SisCreateSmilHandle handle = NULL;

    SisMediaObject  *textObject;
    SisMediaObject  *imageObject;
    SisMediaObject  *audioObject;
    
    MsaObjInfo      *objInfo;
    
    *buffer = NULL;
    *size = 0;
    /* Set the layout */
    msaGetDisplaySize(&displaySize);
    rootLayout.cx.type  = SIS_ABS;
    rootLayout.cx.value = (WE_UINT16)displaySize.width;
    rootLayout.cy.type  = SIS_ABS;
    rootLayout.cy.value = (WE_UINT16)displaySize.height;
    rootLayout.bgColor  = smilInfo->bgColor;

    /* Set regions */
    setRegions(smilInfo, &textRegion, &imageRegion);
    /* Create the SMIL presentation */
    if (SIS_RESULT_OK != SISlib_createSmil(WE_MODID_MSA, &handle, 
        &rootLayout))
    {
        return FALSE;
    }
    else if (SIS_RESULT_OK != SISlib_createRegion(WE_MODID_MSA, &handle,
        &imageRegion, MSA_IMAGE_REGION))
    {
        (void)SISlib_cancelCreateSmil(WE_MODID_MSA, &handle);
        return FALSE;
    }
    else if (SIS_RESULT_OK != SISlib_createRegion(WE_MODID_MSA, &handle,
        &textRegion, MSA_TEXT_REGION))
    {
        (void)SISlib_cancelCreateSmil(WE_MODID_MSA, &handle);
        return FALSE;
    }

    /* Get the first slide */
    old = smilInfo->currentSlide;
    smilInfo->currentSlide = msaGetFirstSlide(smilInfo);
    /* Go through all slides */
    while ((NULL != smilInfo->currentSlide) && (status))
    {
        /* Text */
        textObject = NULL;
        if (msaGetObjInfo(smilInfo, MSA_MEDIA_GROUP_TEXT, &objInfo))
        {
            if (!createSisObject(objInfo, &textObject,
                smilInfo->currentSlide->duration))
            {
                status = FALSE;
            }
        }
        /* Image */
        imageObject = NULL;
        if (msaGetObjInfo(smilInfo, MSA_MEDIA_GROUP_IMAGE, &objInfo))
        {
            if (!createSisObject(objInfo, &imageObject,
                smilInfo->currentSlide->duration))
            {
                status = FALSE;
            }
        }
        /* Audio */
        audioObject = NULL;
        if (msaGetObjInfo(smilInfo, MSA_MEDIA_GROUP_AUDIO, &objInfo))
        {
            if (!createSisObject(objInfo, &audioObject,
                smilInfo->currentSlide->duration))
            {
                status = FALSE;
            }
        }
        /* Check that the slide is valid, i.e., that objects exists in the 
           slide */
        if ((NULL != textObject) || (NULL != audioObject) || (NULL != imageObject))
        {
            /* Create the slide */
            if (status && (SIS_RESULT_OK != SISlib_createSlide(WE_MODID_MSA, 
                &handle, smilInfo->currentSlide->duration, textObject, 
                imageObject, NULL, audioObject)))
            {
                (void)SISlib_cancelCreateSmil(WE_MODID_MSA, &handle);
                status = FALSE;
            } /*  */
        }

        /* Free Text object */
        if (textObject != NULL)
        {
            freeSisMediaObject(&textObject);
        } /* if */
        /* Free Image object */
        if (imageObject != NULL)
        {
           freeSisMediaObject(&imageObject);
        }
        /* Free Audio object */
        if (audioObject != NULL)
        {
           freeSisMediaObject(&audioObject);
        }
        smilInfo->currentSlide = smilInfo->currentSlide->next;
    } /* while */
    smilInfo->currentSlide = old;
    if (status)
    {
        if (SIS_RESULT_OK != SISlib_generateSmil(WE_MODID_MSA, &handle, 
            (char **)&smilBuffer))
        {
            (void)SISlib_cancelCreateSmil(WE_MODID_MSA, &handle);
            return FALSE;
        } /* if */
        *buffer = smilBuffer;
        *size   = strlen(smilBuffer);
        return TRUE;
    } /* if */
    return FALSE;
}

/*!
 * \brief Sets text and image regions for one slide
 * \param smilInfo The current SMIL presentation.
 * \param textRegion Where to set text region
 * \param imageRegion Where to set image region
 *****************************************************************************/
static void setRegions(const MsaSmilInfo *smilInfo, SisRegion *textRegion, 
    SisRegion *imageRegion)
{
    MsaSmilSlide *sSlide;
    unsigned int tIndex;
    unsigned int iIndex;

    /* Common region settings */
    /* Text type */
    textRegion->position.cx.value   = 0;
    textRegion->size.cx.value       = 100; 
    textRegion->position.cx.type    = SIS_REL;
    textRegion->position.cy.type    = SIS_REL;
    textRegion->size.cx.type        = SIS_REL;
    textRegion->size.cy.type        = SIS_REL;
    textRegion->fit                 = SIS_ATTR_FILL;
    /* image type */
    imageRegion->position.cx.value  = 0;
    imageRegion->size.cx.value      = 100;
    imageRegion->position.cx.type   = SIS_REL;
    imageRegion->position.cy.type   = SIS_REL;
    imageRegion->size.cx.type       = SIS_REL;
    imageRegion->size.cy.type       = SIS_REL;
    imageRegion->fit                = SIS_ATTR_FILL;
  

    /* Check for multiple regions */
    for (sSlide = msaGetFirstSlide(smilInfo); sSlide != NULL; sSlide = sSlide->next)
    {
        /* Check if any slides has reference to both image and text */
        if (findObjectInSlide(sSlide, MSA_MEDIA_GROUP_TEXT, &tIndex) && 
            findObjectInSlide(sSlide, MSA_MEDIA_GROUP_IMAGE, &iIndex))
        {
            if (tIndex < iIndex)
            {
                /* Text on top, image at the bottom */
                textRegion->position.cy.value   = 0;
                textRegion->size.cy.value       = 50;
                imageRegion->position.cy.value  = 50;
                imageRegion->size.cy.value      = 50;
            }
            else
            {
                /* Text on image, text at the bottom */
                textRegion->position.cy.value   = 50;
                textRegion->size.cy.value       = 50;
                imageRegion->position.cy.value  = 0;
                imageRegion->size.cy.value      = 50;
            }
            return;
        }
    }
    /* Only one text or image region is displayed on each slide*/
    textRegion->position.cy.value   = 0;
    textRegion->size.cy.value       = 100;
    imageRegion->position.cy.value  = 0;
    imageRegion->size.cy.value      = 100;
} /* setRegions */

/*! \brief Check whether the there are enough information to create/display a SMIL
 *         presentation.
 *
 * \param sInfo The current presentation.
 * \return TRUE if this SMIL presentation can be played, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaIsSmilValid(const MsaSmilInfo *sInfo)
{
	MsaSmilSlide * currentSlide;
    if (NULL != sInfo)
    {
        /* Check that slides and objects exists */
        if ((NULL == sInfo->slides) || (NULL == sInfo->mediaObjects))
        {
            return FALSE;
        }
   		if(NULL == sInfo->currentSlide->objects[0])
        {
            return FALSE;
        }
        /* TR 18998 Check that at least one object is present */
		currentSlide = sInfo->slides;
		while (NULL != currentSlide)
		{
			if(NULL != currentSlide->objects[0])
			{
				return TRUE; /* This is a valid presentation */
			}
			currentSlide = currentSlide->next;
		}       
    }
    return FALSE;
}

/*!
 * \brief Free a SisMediaObject
 *
 * \param object The media object to free
 *****************************************************************************/
static void freeSisMediaObject(SisMediaObject **object)
{
    MSA_FREE((*object)->region);
    MSA_FREE((*object)->src);
    MSA_FREE(*object);
    *object = NULL;
} /* freeSisMediaObject */

/*! \brief Gets information about what types of objects that are present on the
 *         current slide.
 *
 * \param sInfo The current presentation.
 * \return A bit-array defining what types of objects that are present on the 
 *         slide. See #MsaMediaGroupType
 *****************************************************************************/
MsaMediaGroupType msaGetSlideObjectDef(const MsaSmilInfo *sInfo)
{
    MsaSmilSlide *sSlide = msaGetCurrentSlide(sInfo);
    MsaMediaGroupType gType = MSA_MEDIA_GROUP_NONE;
    unsigned int i;

    if (NULL != sSlide)
    {
        for (i = 0; i < MSA_MAX_SMIL_OBJS; i++)
        {
            if (NULL == sSlide->objects[i])
            {
                break;
            }
            else
            {
                gType |= msaMimeToObjGroup(
                    (char *)sSlide->objects[i]->object->type->strValue);
            }
        }
    }
    return gType;
}

/*! \brief Inserts a media object into the slide data-structure according to
 *         the order that objects have been inserted previously
 *	
 * \param sInfo The current SMIL presentation 
 * \param sSlide The current slide.
 * \param  item The new media object to insert.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL insertByOrder(MsaSmilInfo *sInfo, MsaSmilSlide *sSlide, 
    MsaMoDataItem *item)
{
    MsaMediaGroupType objType;
    MsaMediaGroupType objTypeExisting;
    unsigned int i;
    unsigned int ii;
    MsaSmilSlide *currentSlide;
    MsaObjInfo *textObj;
    MsaObjInfo *imageObj;
    unsigned int emptySlot;
    MsaObjInfo *objInfo;

    /* Check if the object type already exists in the current slide */
    objType = msaMimeToObjGroup((char *)item->type->strValue);
    if (NULL != (objInfo = findObjectInSlide(sSlide, objType, &i)))
    {
        /* Don't do anything if it is the same object, TR16917 */
        if (objInfo->object != item)
        {
            /* Delete the old object */
            msaDeleteMo(&(sInfo->mediaObjects), sInfo->slides, 
                        objInfo->object);
            /* Set the new object but keep begin/end/color and so on */
            objInfo->object = item;
        }
        return TRUE;
    }

    /* Check if the object can be inserted without checking other slides */
    textObj = findObjectInSlide(sSlide, MSA_MEDIA_GROUP_TEXT, &i);
    imageObj = findObjectInSlide(sSlide, MSA_MEDIA_GROUP_IMAGE, &ii);
    if ((objType == MSA_MEDIA_GROUP_AUDIO) || 
        ((NULL == textObj) && (MSA_MEDIA_GROUP_IMAGE == objType)) ||
        ((NULL == imageObj) && (MSA_MEDIA_GROUP_TEXT == objType)))
    {
        if (!findEmptyObjectSlot(sSlide, &emptySlot))
        {
            return FALSE;
        }
        /* Set the new object */
        MSA_CALLOC(objInfo, sizeof(MsaObjInfo));
        initObjInfo(objInfo, item, sInfo->fgColorDefault);
        sSlide->objects[emptySlot] = objInfo;
        return TRUE;
    }

    /* A new item needs to be added to the list, check other slides so 
       that text and image are inserted in the same order */
    for (currentSlide = sInfo->slides; currentSlide != NULL; 
        currentSlide = currentSlide->next)
    {
        if ((NULL != (textObj = findObjectInSlide(currentSlide, MSA_MEDIA_GROUP_TEXT, &i))) && 
            (NULL != (imageObj = findObjectInSlide(currentSlide, MSA_MEDIA_GROUP_IMAGE, &ii))))
        {
            /* This slide has one image and on text object, use this order */
            if (MSA_MEDIA_GROUP_TEXT == objType)
            {
                objTypeExisting = MSA_MEDIA_GROUP_IMAGE;
                /* Swap i and ii */
                MSA_SWAP(i, ii);
            }
            else
            {
                objTypeExisting = MSA_MEDIA_GROUP_TEXT;
            }

            if (i < ii)
            {
                /* Append new */
                if (!findEmptyObjectSlot(sSlide, &emptySlot))
                {
                    return FALSE;
                }
                MSA_CALLOC(objInfo, sizeof(MsaObjInfo));
                initObjInfo(objInfo, item, sInfo->fgColorDefault);
                sSlide->objects[emptySlot] = objInfo;
            }
            else
            {
                /* Replace with new and append old */
                if (NULL == findObjectInSlide(sSlide, objTypeExisting, &i))
                {
                    return FALSE;
                }
                if (!findEmptyObjectSlot(sSlide, &emptySlot))
                {
                    return FALSE;
                }
                /* Append old */
                sSlide->objects[emptySlot] = sSlide->objects[i];
                MSA_CALLOC(objInfo, sizeof(MsaObjInfo));
                initObjInfo(objInfo, item, sInfo->fgColorDefault);
                /* Replace */
                sSlide->objects[i] = objInfo;
            }
            return TRUE;
        }
    }
    /* It does not matter where the object is inserted, insert at the end */
    if (!findEmptyObjectSlot(sSlide, &emptySlot))
    {
        return FALSE;
    }
    /* Set the new object */
    MSA_CALLOC(objInfo, sizeof(MsaObjInfo));
    initObjInfo(objInfo, item, sInfo->fgColorDefault);
    sSlide->objects[emptySlot] = objInfo;
    return TRUE;
}

/*! \brief Finds a specific media object in a slide.
 *	
 * \param sSlide  The slide to check to find the specified object in.
 * \param objType The type of object to look for.
 * \param index   The index of the object.
 * \return A reference to the media object or NULL.
 *****************************************************************************/
static MsaObjInfo *findObjectInSlide(const MsaSmilSlide *sSlide, 
    MsaMediaGroupType objType, unsigned int *index)
{
    unsigned int i;
    for (i = 0; i < MSA_MAX_SMIL_OBJS; i++)
    {
        if ((NULL != sSlide->objects[i]) && (objType == 
            msaMimeToObjGroup(
            (char *)sSlide->objects[i]->object->type->strValue)))
        {
            *index = i;
            return sSlide->objects[i];
        }
    }
    return NULL;
}

/*! \brief Finds the first empty position to insert a media object
 *	
 * \param slide The current slide.
 * \param index The index of the empty slot is returned here.
 * \return TRUE if an empty slot was found, otherwise FASLE.
 *****************************************************************************/
static WE_BOOL findEmptyObjectSlot(const MsaSmilSlide *slide, 
    unsigned int *index)
{
    unsigned int i;
    for (i = 0; i < MSA_MAX_SMIL_OBJS; i++)
    {
        if (NULL == slide->objects[i])
        {
            *index = i;
            return TRUE;
        }
    }
    return FALSE;
}

/*! \brief Sets the start time for a specific object.
 *
 *   Note that this function may change the objects duration in order to be up to
 *   date with the slides duration.
 *
 * \param sInfo The current SMIL presentation.
 * \param gType The type of object to set.
 * \param start The new start time. Note that the start time may be changed in
 *              order be within timing bounds.
 * \return TRUE if the objects start time was set, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaSetObjStart(const MsaSmilInfo *sInfo, MsaMediaGroupType gType, 
    WE_UINT32 start)
{
    MsaObjInfo *objInfo;
    MsaSmilSlide *sSlide;

    if (sInfo != NULL)
    {
        if (!msaGetObjInfo(sInfo, gType, &objInfo))
        {
            return FALSE;
        }
        if (NULL == (sSlide = msaGetCurrentSlide(sInfo)))
        {
            return FALSE;
        }
        /* Check that all values are within bounds */
        if ((unsigned int)start > sSlide->duration ||
            sSlide->duration - (unsigned int)start < MSA_OBJ_DURATION_MIN)
        {
            /* Error, reset start value */
            start = 0;
        }
        if (objInfo->duration != 0)
        {
            if (start + objInfo->duration > sSlide->duration)
            {
                objInfo->duration = 0;
            }
        }
        objInfo->begin = start;
        return TRUE;
    }
    return FALSE;
}

/*! \brief Provides to start time for the SMIL object that is currently active.
 *
 * \param sInfo The current SMIL presentation.
 * \param gType The type of object to get the timing from.
 * \param begin The objects start time is written here.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaGetObjStart(const MsaSmilInfo *sInfo, MsaMediaGroupType gType, 
    WE_UINT32 *begin)
{
    MsaObjInfo *objInfo;
    if (NULL == sInfo)
    {
        return FALSE;
    }
    if (msaGetObjInfo(sInfo, gType, &objInfo))
    {
        *begin = objInfo->begin;
        return TRUE;
    }
    return FALSE;
}

/*! \brief Sets the duration time for a specific object.
 *
 *   Note that this function may change the objects start time in order to be 
 *   up to date with the slides duration.
 *
 * \param sInfo The current SMIL presentation.
 * \param gType The type of object to set.
 * \param duration The new object duartion.
 * \return TRUE if the objects start time was set, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaSetObjDuration(const MsaSmilInfo *sInfo, MsaMediaGroupType gType, 
    WE_UINT32 duration)
{
    MsaObjInfo *objInfo;
    MsaSmilSlide *sSlide;
    WE_UINT32 newDuration;

    if (NULL != sInfo)
    {
        if (!msaGetObjInfo(sInfo, gType, &objInfo))
        {
            return FALSE;
        }
        if (NULL == (sSlide = msaGetCurrentSlide(sInfo)))
        {
            return FALSE;
        }
        
        newDuration = duration;
        if (duration < MSA_OBJ_DURATION_MIN)
        {
            /* Set min duration */
            newDuration = MSA_OBJ_DURATION_MIN;
        }        
        if ((unsigned int)duration >= sSlide->duration - objInfo->begin)
        {
            /* Set max duration (=0) */
            newDuration = 0;
        }
        objInfo->duration = newDuration;
        return TRUE;
    }
    return FALSE;
}

/*! \brief Provides to duration for the SMIL object that is currently active.
 *
 * \param sInfo The current SMIL presentation.
 * \param gType The type of object to get the timing from.
 * \param duration The objects duration is written here.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaGetObjDuration(const MsaSmilInfo *sInfo, MsaMediaGroupType gType, 
    WE_UINT32 *duration)
{
    MsaObjInfo *objInfo;
    if (NULL == sInfo)
    {
        return FALSE;
    }
    if (msaGetObjInfo(sInfo, gType, &objInfo))
    {
        *duration = objInfo->duration == 0 ?
                    sInfo->currentSlide->duration - objInfo->begin :
                    objInfo->duration;
        return TRUE;
    }
    return FALSE;
}

/*! \brief Sets a media object for the current slide.
 *
 * \param smilInfo The current SMIL presentation
 * \param item The item that should be visible on the slide.
 * \return A reference to the inserted object or NULL.
 *****************************************************************************/
MsaObjInfo *msaSetSlideMoByRef(MsaSmilInfo *smilInfo, MsaMoDataItem *item)
{
    unsigned int i;
    MsaSmilSlide *slide;
    if ((NULL == smilInfo) || (NULL == item))
    {
        return FALSE;
    }
    slide = msaGetCurrentSlide(smilInfo);
    /* Insert the object into the current slide */
    if (!insertByOrder(smilInfo, slide, item))
    {
        return FALSE;
    }
    for (i = 0; i < MSA_MAX_SMIL_OBJS; i++)
    {
        if (slide->objects[i]->object == item)
        {
            return slide->objects[i];
        }
    }
    return NULL;
}

/*! \brief Provides to duration for the SMIL object that is currently active.
 *
 * \param sInfo The current SMIL presentation.
 * \param gType The type of object to get the timing from.
 * \param fgColor The foreground-color of the object, only for text color for 
 *                 now.
 * \param fgColor The background-color of the object, only for text bg color 
 *                for now.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaGetObjColor(const MsaSmilInfo *sInfo, MsaMediaGroupType gType, 
    WE_UINT32 *fgColor, WE_UINT32 *bgColor)
{
    MsaObjInfo *objInfo;
    if (NULL == sInfo)
    {
        return FALSE;
    }
    if (msaGetObjInfo(sInfo, gType, &objInfo))
    {
        *fgColor = objInfo->textColor;
        *bgColor = objInfo->bgColor;
        return TRUE;
    }
    return FALSE;
}

/*! \brief Sets the color for a specific object.
 *
 * \param sInfo The current SMIL presentation.
 * \param gType The type of object to set.
 * \param fgColor The fore-ground color to set.
 * \param bgColor The back-ground color to set.
 * \return TRUE if the colors was set, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaSetObjColor(const MsaSmilInfo *sInfo, MsaMediaGroupType gType, 
    WE_UINT32 fgColor, WE_UINT32 bgColor)
{
    MsaObjInfo *objInfo;

    if (NULL != sInfo)
    {
        if (!msaGetObjInfo(sInfo, gType, &objInfo))
        {
            return FALSE;
        }
        objInfo->textColor = fgColor;
        objInfo->bgColor   = bgColor;
        return TRUE;
    }
    return FALSE;
}

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

/* !\file mamoh.c
 *  \brief Media object handling.
 */

/* WE */
#include "We_Mem.h"
#include "We_Lib.h"
#include "We_Dcvt.h"

/* SIS */
#include "Sis_Cfg.h"
#include "Sis_If.h"

/* MMS */
#include "Mms_Def.h"
#include "Mms_Cfg.h"
#include "Mms_If.h"

/* MSA*/
#include "Msa_Def.h"
#include "Msa_Cfg.h"
#include "Msa_Types.h"
#include "Msa_Mem.h"
#include "Msa_Moh.h"
#include "Msa_Utils.h"

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static void msaFreeMoItem(MsaMoDataItem *mo);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Searches for a media object with a specific name.
 * 
 * \param mo The list of media objects.
 * \param name The name of the media object, usually its filename.
 * \return The media object or NULL if the object does not exist.
 *****************************************************************************/
MsaMoDataItem *msaFindMo(MsaMoDataItem *mo, const char *name)
{
    while(NULL != mo)
    {
        /* Check if the media object already exists */
        if ((NULL != mo->name) && (0 == strcmp(mo->name, name)))
        {
            return mo;
        }
        mo = mo->next;
    }
    return mo;
}

/*!
 * \brief Adds a media object to the list of media objects.
 * 
 * \param mediaObjects The list of media objects.
 * \param data The new media objects data.
 * \param dataSize The size of the data for the new media object.
 * \param mediaType The type of object to add, see #MsaMediaType.
 * \param name The name of the new object, usually its filename.
 * \return A reference to the media object or NULL.
 *****************************************************************************/
MsaMoDataItem *msaAddMo(MsaMoDataItem **mediaObjects, unsigned char *data, 
    unsigned int dataSize, MmsContentType *mmsType, char *name)
{
    MsaMoDataItem *item;
    /* Check if the item already exists */
    if (NULL != (item = msaFindMo(*mediaObjects, name)))
    {
        /* If the item already exists the data needs to be deallocated */
        MSA_FREE(data);
        MSA_FREE(name);
        if (NULL != mmsType)
        {
            MMSif_freeMmsContentType(WE_MODID_MSA, mmsType, TRUE);
        }
        return item;
    }

    /* Allocate new item */
    item = MSA_ALLOC(sizeof(MsaMoDataItem));
    /* Initiate data-structure */
    memset(item, 0, sizeof(MsaMoDataItem));
    item->data             = data;
    item->size             = dataSize;
    item->type             = mmsType;
    item->name             = name;
    /* Check if any objects are present */
    if (NULL == *mediaObjects)
    {
        /* Set the first item to the current */
        *mediaObjects = item;
    }
    else
    {
        /* Insert the new item first in the list */
        item->next = *mediaObjects;
        *mediaObjects = item;
    }
    return item;
}

/*!
 * \brief De-allocates a media object.
 *
 * \param mo The media object to de-allocate.
 *****************************************************************************/
static void msaFreeMoItem(MsaMoDataItem *mo)
{
    MSA_FREE(mo->data);
    MSA_FREE(mo->name);
    if (NULL != mo->type)
    {
        MMSif_freeMmsContentType(WE_MODID_MSA, mo->type, TRUE);
    }
    MSA_FREE(mo);
}

/*!
 * \brief Counts the number of references to a media object.
 * 
 * \param sSlide A list of SMIL slides.
 * \param mo The media object to find references for.
 * \return The numer of references to mo.
 *****************************************************************************/
int msaGetMoReferenceCount(const MsaSmilSlide *sSlide, const MsaMoDataItem *mo)
{
    int refCount = 0;
    unsigned int i;
    /* Check all slides */
    while(sSlide)
    {
        /* Are there any references */
        for (i = 0; i < MSA_MAX_SMIL_OBJS; ++i)
        {
            if ((NULL != sSlide->objects[i]) && 
                (mo == sSlide->objects[i]->object))
            {
                ++refCount;
                continue;
            }
        }
        sSlide = sSlide->next;
    }
    return refCount;
}

/*!
 * \brief Deletes a media object from the list of media objects.
 *
 * \param mediaObjects The list of media objects.
 * \param sSlide The list of slides.
 * \param mo The media object to delete.
 *****************************************************************************/
void msaDeleteMo(MsaMoDataItem **mediaObjects, const MsaSmilSlide *sSlide, 
    MsaMoDataItem *mo)
{
    MsaMoDataItem *item;
    MsaMoDataItem *prevItem;
    if ((NULL == (item = *mediaObjects)) || (NULL == mo))
    {
        return;
    }
    /* Check if more than one reference exists */
    if (msaGetMoReferenceCount(sSlide, mo) > 1)
    {
        return;
    }
    /* Delete the media object */
    if (*mediaObjects == mo)
    {
        /* Handle the first item */
        *mediaObjects = mo->next; 
        msaFreeMoItem(mo);
    }
    else
    {
        item = *mediaObjects;
        prevItem = NULL;
        /* Find the current item and the previous item */
        while( (NULL != item) && (item != mo) )
        {
            prevItem = item;
            item = item->next;
        }
        /* Remove the item from the list */
        if (NULL == item)
        {
            return;
        }
        /*lint -e{794} */
        prevItem->next = item->next;
        msaFreeMoItem(item);
    }
}

/*!
 * \brief Get the total number of media objects.
 * 
 * \param mediaObjects A list of media objects.
 *****************************************************************************/
unsigned int msaGetMoCount(const MsaMoDataItem *mediaObjects)
{
    unsigned int count = 0;
    while (NULL != mediaObjects)
    {
        count++;
        mediaObjects = mediaObjects->next;
    }
    return count;
}

/* ! \brief Calculates the index of a media object
 *
 * \param mediaObjects The list of media objects
 * \param mo The media object o find the index of
 * \return The index of the object (counting from zero) or -1 if the object 
 *         does not exist.
 *****************************************************************************/
int msaGetMoIndex(const MsaMoDataItem *mediaObjects, const MsaMoDataItem *mo)
{
    int i = 0;
    while(NULL != mediaObjects)
    {
        if (mediaObjects == mo)
        {
            return i;
        }
        ++i;
        mediaObjects = mediaObjects->next;
    }
    return -1;
}

/*! \brief Finds an object with a specific body-part reference
 *
 * \param mediaObject The list of media objects.
 * \param index The MMS body-part index to search for.
 * \return The objects or NULL if the object does not exist.
 *****************************************************************************/
MsaMoDataItem *msaFindObjectByMmsIndex(MsaMoDataItem *mediaObjects, 
    WE_UINT32 index)
{
    while(mediaObjects != NULL)
    {
        if (mediaObjects->bpIndex == index)
        {
            return mediaObjects;
        }
        mediaObjects = mediaObjects->next;
    }
    return NULL;
}

/*!
 * \brief Searches for a specific media object
 * 
 * \param mo The list of media objects.
 * \return Next matching video media object or NULL if not found
 *****************************************************************************/
MsaMoDataItem *msaFindObjByType(MsaMoDataItem *mo, MsaMediaGroupType gType)
{
    while (NULL != mo)
    {
        /* Check for match */
        if ((gType & msaMimeToObjGroup((const char*)mo->type->strValue)) > 
            MSA_MEDIA_GROUP_NONE)
        {
            return mo;
        } /* if */
        mo = mo->next;
    }
    return NULL;
}

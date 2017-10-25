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
/*! \file Sis_if.c
 * \brief  <brief description>
 */

/*--- Include files ---*/
#include "We_Core.h"
#include "We_Mem.h"
#include "We_Cfg.h"
#include "We_Log.h"    /* WE: Signal logging */ 

#include "Mms_Def.h"
#include "Mms_Cfg.h"
#include "Mms_If.h"
#include "We_Lib.h"

#include "Sis_cfg.h"
#include "Sis_def.h"
#include "Sis_if.h"
#include "Sis_cvt.h"
#include "Sis_prse.h"

#ifdef WE_LOG_FC
#include "Sis_main.h"
#include "c_Sis_script.h"
#endif


/*--- Definitions/Declarations ---*/

/*--- Types ---*/

typedef struct SlsSlideListSt
{
    int                 duration;
    SisMediaObject      text;    
    SisMediaObject      image;   
	SisMediaObject      video;   
    SisMediaObject      audio;
    struct SlsSlideListSt *next;
} SlsSlideList;

typedef struct SlsRegionListNode
{
    SisRegion           data;
    char               *name;
    struct SlsRegionListNode  *next;
}SlsRegionListNode;

typedef struct SlsCreateSmilData
{
    SisRootLayout       *rootLayout;
    SlsRegionListNode   *regionList;
    SlsSlideList        *slideList;
}SlsCreateSmilData;

/*--- Constants ---*/

#define SIS_SMIL_START      "<smil>\n"
#define SIS_SMIL_END        "</smil>\n"
#define SIS_HEAD_START      "<head>\n"
#define SIS_HEAD_END        "</head>\n"
#define SIS_BODY_START      "<body>\n"
#define SIS_BODY_END        "</body>\n"
#define SIS_PAR_START       "<par dur=\"%dms\">\n"
#define SIS_PAR_END         "</par>\n"
#define SIS_TEXT_T1         "<text src=\"%s\" region=\"%s\" "
#define SIS_TEXT_T2         "begin=\"%lums\" "
#define SIS_TEXT_T3         "end=\"%lums\" "
#define SIS_TEXT_T4         "dur=\"%lums\" "
/*TR 17970 replace "foregroundcolor" with "foreground-color"*/
#define SIS_TEXT_T5         "<param name=\"foreground-color\" value=\"%s\" /></text>\n"
#define SIS_TEXT_T6         "/>\n"
#define SIS_TEXT_T7         ">\n"
#define SIS_IMAGE_T1        "<img src=\"%s\" region=\"%s\" "
#define SIS_IMAGE_T2         "begin=\"%lums\" "
#define SIS_IMAGE_T3         "end=\"%lums\" "
#define SIS_IMAGE_T4         "dur=\"%lums\" "
#define SIS_IMAGE_T5        "/>\n"
#define SIS_VIDEO_T1        "<video src=\"%s\" region=\"%s\" "
#define SIS_VIDEO_T2        "begin=\"%lums\" "
#define SIS_VIDEO_T3        "end=\"%lums\" "
#define SIS_VIDEO_T4        "dur=\"%lums\" "
#define SIS_VIDEO_T5        "/>\n"
#define SIS_AUDIO_T1         "<audio src=\"%s\" "
#define SIS_AUDIO_T2         "begin=\"%lums\" "
#define SIS_AUDIO_T3         "end=\"%lums\" "
#define SIS_AUDIO_T4         "dur=\"%lums\" "
#define SIS_AUDIO_T5        "/>\n"
#define SIS_LAYOUT_START    "<layout>\n"
#define SIS_LAYOUT_END      "</layout>\n"
#define SIS_ROOT_LAYOUT_T1  "<root-layout width=\""
#define SIS_ROOT_LAYOUT_T2  "\" height=\""
#define SIS_ROOT_LAYOUT_T3  "\" background-color=\""
#define SIS_ROOT_LAYOUT_T4  "\"/>\n"
#define SIS_REGION_T1       "<region id=\""
#define SIS_REGION_T2       "\" width=\""
#define SIS_REGION_T3       "\" height=\""
#define SIS_REGION_T4       "\" left=\""
#define SIS_REGION_T5       "\" top=\""
#define SIS_REGION_T6       "\" />\n"

#define SIS_INTSZ           (WE_INT32)8
/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Global variables ---*/

/*--- Static variables ---*/

/*--- Prototypes ---*/
static char *Sis_strdup(WE_UINT8, const char *);
static void Sis_itoa(SisSizeValue, char *);
static void Sis_itoacol(WE_UINT32 color, char *s);
static WE_BOOL copySlsMediaObj(WE_UINT8 modId, const SisMediaObject *source, 
	SisMediaObject *dest);

/******************************************************************************/

/*!
 * \brief <Short description>
 * 
 * \param <x> <Description of x>
 * \return <whatever is returned>
 *****************************************************************************/
void SISif_startInterface(void)
{
    #ifdef WE_LOG_FC  
        WE_LOG_FC_BEGIN(SISif_startInterface)
        WE_LOG_FC_PRE_IMPL
    #endif

    WE_SIGNAL_REG_FUNCTIONS(WE_MODID_SIS, slsConvert, slsDestruct);

    #ifdef WE_LOG_FC  
        WE_LOG_FC_END
    #endif
}


/*!
 * \brief Opens an MMS message and parses the SMIL part. 
 *
 * Type: Signal Function 
 *
 * Reply Signal: 
 *      signal name: #SIS_OPEN_SMIL_RSP
 *      data type    #SisOpenSmilRsp
 *
 * \param source Module ID of the calling module
 * \param msgId Message ID of the message to be opened
 * \param userData Custom data as sent back in result signal
 *****************************************************************************/
void SISif_openSmil(WE_UINT8 source, 
                    WE_UINT32 msgId, 
                    WE_UINT32 userData)
{
    we_dcvt_t      cvt;
    void           *sig;
    void           *sigData;
    WE_UINT16      length;
    
#ifdef WE_LOG_FC  
    WE_LOG_FC_BEGIN(SISif_openSmil)
    WE_LOG_FC_UINT8(source, NULL); 
    WE_LOG_FC_UINT32(msgId, NULL); 
    WE_LOG_FC_UINT32(userData, NULL); 
    WE_LOG_FC_PRE_IMPL
#endif

    we_dcvt_init (&cvt, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);

    (void)we_dcvt_uint32(&cvt, &msgId);
    (void)we_dcvt_uint32(&cvt, &userData);

    length = (WE_UINT16)cvt.pos;

    sig = WE_SIGNAL_CREATE (SIS_OPEN_SMIL_REQ, source, 
        WE_MODID_SIS, length);

    sigData = WE_SIGNAL_GET_USER_DATA (sig, &length);
    
    we_dcvt_init (&cvt, WE_DCVT_ENCODE, sigData, length, source);

    (void)we_dcvt_uint32(&cvt, &msgId);
    (void)we_dcvt_uint32(&cvt, &userData);

    WE_SIGNAL_SEND (sig);

#ifdef WE_LOG_FC  
    WE_LOG_FC_END
#endif


} /* SISif_openSmil */

/*!
 * \brief This method deallocates the resources from a previous call 
 * to SIS_openSmil. 
 *
 * Type: Signal Function 
 *
 * Reply Signal: 
 *      signal name: #SIS_CLOSE_SMIL_RSP
 *      data type    #SisCloseSmilRsp
 * 
 * \param source Module ID of the calling module
 * \param userData Custom data as sent back in result signal
 *****************************************************************************/
void SISif_closeSmil(WE_UINT8 source, 
                     WE_UINT32 userData)
{
    we_dcvt_t      cvt;
    void           *sig;
    void           *sigData;
    WE_UINT16      length;
    
#ifdef WE_LOG_FC  
    WE_LOG_FC_BEGIN(SISif_closeSmil)
    WE_LOG_FC_UINT8(source, NULL);  
    WE_LOG_FC_UINT32(userData, NULL); 
    WE_LOG_FC_PRE_IMPL
#endif

    we_dcvt_init (&cvt, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);

    (void)we_dcvt_uint32(&cvt, &userData);

    length = (WE_UINT16)cvt.pos;

    sig = WE_SIGNAL_CREATE (SIS_CLOSE_SMIL_REQ, source, 
        WE_MODID_SIS, length);

    sigData = WE_SIGNAL_GET_USER_DATA (sig, &length);
    
    we_dcvt_init (&cvt, WE_DCVT_ENCODE, sigData, length, source);

    (void)we_dcvt_uint32(&cvt, &userData);

    WE_SIGNAL_SEND (sig);

#ifdef WE_LOG_FC  
    WE_LOG_FC_END
#endif

} /* SISif_closeSmil */


/*!
 * \brief Gets information about a given range of slides. 
 *
 *
 * Type: Signal Function 
 *
 * Reply Signal: 
 *      signal name: #SIS_GET_SLIDE_INFO_RSP
 *      data type    #SisGetSlideInfoRsp
 * 
 * \param source Module ID of the calling module
 * \param startSlide The first slide to get information about
 * \param numberOfSlides Number of slides to get information about
 * \param userData Custom data as sent back in result signal
 *****************************************************************************/
void SISif_getSlideInfo(WE_UINT8 source, 
                        WE_UINT32 startSlide, 
                        WE_UINT32 numberOfSlides,
                        WE_UINT32 userData)
{

    we_dcvt_t      cvt;
    void           *sig;
    void           *sigData;
    WE_UINT16      length;

#ifdef WE_LOG_FC  
    WE_LOG_FC_BEGIN(SISif_getSlideInfo)
    WE_LOG_FC_UINT8(source, NULL); 
    WE_LOG_FC_UINT32(startSlide, NULL);
    WE_LOG_FC_UINT32(numberOfSlides, NULL);
    WE_LOG_FC_UINT32(userData, NULL);  
    WE_LOG_FC_PRE_IMPL
#endif

    we_dcvt_init (&cvt, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
    (void)we_dcvt_uint32(&cvt, &startSlide);
    (void)we_dcvt_uint32(&cvt, &numberOfSlides);
    (void)we_dcvt_uint32(&cvt, &userData);

    length = (WE_UINT16)cvt.pos;

    sig = WE_SIGNAL_CREATE (SIS_GET_SLIDE_INFO_REQ, source, 
        WE_MODID_SIS, length);
   
    sigData = WE_SIGNAL_GET_USER_DATA (sig, &length);
   
    we_dcvt_init (&cvt, WE_DCVT_ENCODE, sigData, length, source);
    
    (void)we_dcvt_uint32(&cvt, &startSlide);
    (void)we_dcvt_uint32(&cvt, &numberOfSlides);
    (void)we_dcvt_uint32(&cvt, &userData);

    WE_SIGNAL_SEND (sig);

#ifdef WE_LOG_FC  
    WE_LOG_FC_END
#endif

}


/*!
 * \brief Creates a new smil message
 * 
 * \param modId Module id of the calling module 
 * \param handle The handle of the operation
 * \param rootLayout The root-layout definition for the smil.
 * \return Result code
 *****************************************************************************/
SisResult SISlib_createSmil(WE_UINT8 modId, SisCreateSmilHandle *handle,
    const SisRootLayout *rootlayout)
{
    SlsCreateSmilData *p;

#ifdef WE_LOG_FC  
    WE_LOG_FC_BEGIN(SISlib_createSmil)	
#endif

    /* Alloc data for SMIL creation */
    *handle = WE_MEM_ALLOC(modId, sizeof(SlsCreateSmilData));
    p = (SlsCreateSmilData*)*handle;
    /* Initiate data */
    p->regionList = NULL;
    p->rootLayout = NULL;
    p->slideList = NULL;
    
    /* Set rootlayout */
    if (rootlayout)
    {
        p->rootLayout = WE_MEM_ALLOC(modId, sizeof(SisRootLayout));
        if (!p->rootLayout)
        {
            return SIS_RESULT_ERROR;
        }
        SISif_copySlsRootLayout(rootlayout, p->rootLayout);
    } /* if */

#ifdef WE_LOG_FC  
	WE_LOG_FC_PTR(handle, NULL);
    WE_LOG_FC_END
#endif  

    return SIS_RESULT_OK;
}


/*!
 * \brief Creates a new region
 * 
 * \param modId Module id of the calling module
 * \param region Size and position of the region
 * \param name Name of the region
 * \return Result code
 *****************************************************************************/
SisResult SISlib_createRegion(WE_UINT8        modId,
                           const SisCreateSmilHandle *handle,
                           const SisRegion *region,
                           const char      *name)
{	
    SlsRegionListNode *nodeP;
	SlsCreateSmilData *p = (SlsCreateSmilData*)*handle;
    SisResult          result = SIS_RESULT_OK;		
    
#ifdef WE_LOG_FC
	SisRegion *logRegion = WE_MEM_ALLOC(modId, sizeof(SisRegion));	
	WE_UINT32 logResult;
    WE_LOG_FC_BEGIN(SISlib_createRegion)
	WE_LOG_FC_PTR(handle, NULL);
	if (name != NULL)
	{
		WE_LOG_FC_STRING(name, NULL);
	}
	if (region != NULL)
	{
		WE_LOG_FC_UINT16(logRegion->position.cx.value, NULL);
		WE_LOG_FC_UINT16(logRegion->position.cy.value, NULL);
		WE_LOG_FC_UINT16(logRegion->size.cx.value, NULL);
		WE_LOG_FC_UINT16(logRegion->size.cy.value, NULL);
	}
#endif
#ifdef WE_LOG_FC    
    if (!logRegion || !name)
#endif
#ifndef WE_LOG_FC    
    /* Check parameters */
    if (!region || !name)
#endif
    {
        result = SIS_RESULT_ERROR;
    }
    /* Check state */
    else if (!p)
    {
        result = SIS_RESULT_SMIL_NOT_CREATED;
    }
    else
    {
        /* Set region */
        if (p->regionList)
        {
            nodeP = p->regionList;
            while (nodeP->next)
            {
                nodeP = nodeP->next;
            }
            
			nodeP->next = WE_MEM_ALLOC(modId, sizeof(SlsRegionListNode));
            if (nodeP->next == NULL)
            {
                result = SIS_RESULT_ERROR;
            }
            nodeP = nodeP->next;
        }
        else
        {
            p->regionList = WE_MEM_ALLOC(modId, sizeof(SlsRegionListNode));
            if (p->regionList == NULL)
            {
                result = SIS_RESULT_ERROR;
            }        
            nodeP = p->regionList;
        }
        nodeP->next = NULL;
#ifdef WE_LOG_FC
		nodeP->data = *logRegion;
#endif
#ifndef WE_LOG_FC
        SISif_copySlsRegion(region, &(nodeP->data));
#endif
        nodeP->name = Sis_strdup(modId, name);
        if(nodeP->name == NULL)
        {
            result = SIS_RESULT_ERROR;
        }
    }
#ifdef WE_LOG_FC  
    logResult = (WE_UINT32) result;
	WE_MEM_FREE(modId, logRegion);
    WE_LOG_FC_UINT32 ( logResult, NULL);
    WE_LOG_FC_END
#endif

    return result;
}


/*!
 * \brief Cancels the creation of a SMIL message and frees temporary
 *  resources
 * 
 * \param modId Module id of the calling module 
 * \return Result code
 *****************************************************************************/
SisResult SISlib_cancelCreateSmil(WE_UINT8 modId, 
                               const SisCreateSmilHandle *handle)
{
    SlsSlideList *list;
    SlsRegionListNode *nodeP;
    SlsCreateSmilData *p = (SlsCreateSmilData*)*handle;

#ifdef WE_LOG_FC  
    WE_LOG_FC_BEGIN(SISlib_cancelCreateSmil)	
#endif

    /* Clean up data */
    if (p->rootLayout)
    {
        WE_MEM_FREE(modId, p->rootLayout);
    }

    while (p->regionList)
    {
        nodeP = p->regionList->next;
        WE_MEM_FREE(modId, p->regionList->name);
        WE_MEM_FREE(modId, p->regionList);
        p->regionList = nodeP;
    }
    
    while (p->slideList)
    {
        list = p->slideList->next;
        if (p->slideList->text.type !=  SIS_NOT_USED)
        {
            WE_MEM_FREE(modId, p->slideList->text.src);
            WE_MEM_FREE(modId, p->slideList->text.region);
        }
        if (p->slideList->image.type !=  SIS_NOT_USED)
        {
            WE_MEM_FREE(modId, p->slideList->image.src);
            WE_MEM_FREE(modId, p->slideList->image.region);
        }
        if (p->slideList->audio.type !=  SIS_NOT_USED)
        {
            WE_MEM_FREE(modId, p->slideList->audio.src);
        }

        WE_MEM_FREE(modId, p->slideList);
        p->slideList = list;
    }
    
    WE_MEM_FREE(modId, p);

#ifdef WE_LOG_FC
	WE_LOG_FC_PTR(handle, NULL);
    WE_LOG_FC_END
#endif
    
    return SIS_RESULT_OK;
}


/*!
 * \brief Creates a new slide to the smil message
 * 
 * \param modId Module id of the calling module 
 * \param Slide duration (sec)
 * \param text Text MO, NULL if not used
 * \param image Image MO, NULL if not used
 * \param video Video MO, NULL if not used
 * \param audio Audio MO, NULL if not used
 * \return Result code
 *****************************************************************************/
SisResult SISlib_createSlide(WE_UINT8       modId,
                          const SisCreateSmilHandle *handle,
                          WE_UINT32            duration,
                          const SisMediaObject *text,    
                          const SisMediaObject *image,   
                          const SisMediaObject *video,
                          const SisMediaObject *audio)
{
    SlsCreateSmilData *p = (SlsCreateSmilData*)*handle;
    SlsSlideList      *list = p->slideList;
    SisResult          result = SIS_RESULT_OK;

#ifdef WE_LOG_FC
	WE_UINT32 logResult;
	WE_LOG_FC_BEGIN(SISlib_createSlide)	
	initSlsMediaObject();	
	logSlsMediaObject(WE_LOG_FC_PARAMS_IMPLEMENT);
    WE_LOG_FC_INT32(duration, NULL);
	assignSlsMediaObject(&text, &image, &audio);	
#endif

    /* Check state */
    if (p == NULL)
    {
        result = SIS_RESULT_SMIL_NOT_CREATED;
    }

    /* Check MO parameters */
	else if (text == NULL && image == NULL && audio == NULL && video == NULL)    
    {
        result = SIS_RESULT_MISSING_MEDIA_OBJECTS;
    }
    
    /* Check src and region parameters */
	else if ((text != NULL && text->src  == NULL && text->region == NULL) ||
        (image != NULL && image->src == NULL && image->region == NULL) ||
        (video != NULL && video->src == NULL && video->region == NULL) ||
        (audio != NULL && audio->src == NULL))
	{
        result = SIS_RESULT_MISSING_MEDIA_OBJECT_SRC;
    }
    else
    {
        /* Alloc new node */
        if (p->slideList == NULL)
        {
            p->slideList = WE_MEM_ALLOC(modId, sizeof(SlsSlideList));
            if (p->slideList == NULL)
            {
                return SIS_RESULT_ERROR;
            }
            list = p->slideList;
        }
        else
        {
            while (list->next != NULL)
            {
                list = list->next;
            }
            list->next = WE_MEM_ALLOC(modId, sizeof(SlsSlideList));
            if (list->next == NULL)
            {
                return SIS_RESULT_ERROR;
            }
            list = list->next;      
        }
        list->next = NULL;

        /* Set duration */
        list->duration = duration;


        /* Set text */
        if (!copySlsMediaObj(modId, text, &(list->text)))
		{
			return SIS_RESULT_ERROR;
		} /* if */

		if (!copySlsMediaObj(modId, image, &(list->image)))
		{
			return SIS_RESULT_ERROR;
		} /* if */
        
        if (!copySlsMediaObj(modId, video, &(list->video)))
		{
			return SIS_RESULT_ERROR;
		} /* if */

		if (!copySlsMediaObj(modId, audio, &(list->audio)))
		{
			return SIS_RESULT_ERROR;
		} /* if */

    }
#ifdef WE_LOG_FC  
	logResult = (WE_UINT32) result;
    WE_LOG_FC_UINT32 ( logResult, NULL);
	freeSlsMediaObject();
    WE_LOG_FC_END
#endif

    return result;
}


/*!
 * \brief Generates a SMIL message and frees temporary resources
 * 
 * \param modId Module id of the calling module 
 * \param buffer Pointer to the data buffer
 * \return Result code
 *****************************************************************************/
SisResult SISlib_generateSmil(WE_UINT8 modId, 
    const SisCreateSmilHandle *handle, char **buffer)
{
    WE_UINT32 size = 0;
    char tmp[256];
    char tmpStrIntsz[SIS_INTSZ];
    SlsCreateSmilData *p = (SlsCreateSmilData*)*handle;
    SlsSlideList *list = p->slideList;
    SlsRegionListNode *regionNode = p->regionList;
    SisResult result = SIS_RESULT_OK;
	WE_UINT8 slideCnt = 0;
    
#ifdef WE_LOG_FC
	WE_UINT32 logResult;
    WE_LOG_FC_BEGIN(SISlib_generateSmil)	
#endif

    /* Check state */
    if (p == NULL)
    {
        return SIS_RESULT_SMIL_NOT_CREATED;
    }
    
    /* Get size for buffer */
    size = sizeof(SIS_SMIL_START) + sizeof(SIS_SMIL_END) + 
        sizeof(SIS_HEAD_START) + sizeof(SIS_HEAD_END) +
        sizeof(SIS_LAYOUT_START) + sizeof(SIS_LAYOUT_END) +
        sizeof(SIS_BODY_START) + sizeof(SIS_BODY_END);
    if (p->rootLayout != NULL)
    {
        size += sizeof(SIS_ROOT_LAYOUT_T1) + sizeof(SIS_ROOT_LAYOUT_T2) +
            sizeof(SIS_ROOT_LAYOUT_T3) + sizeof(SIS_ROOT_LAYOUT_T4) + 
            SIS_INTSZ + SIS_INTSZ + SIS_INTSZ;
    }
    while (regionNode != NULL)
    {
        size += sizeof(SIS_REGION_T1) + sizeof(SIS_REGION_T2) + 
            sizeof(SIS_REGION_T3) + sizeof(SIS_REGION_T4) + 
            sizeof(SIS_REGION_T5) + sizeof(SIS_REGION_T6) +
            strlen(regionNode->name) + 4*SIS_INTSZ;
        regionNode = regionNode->next;
    }
    while (list != NULL && (slideCnt < MAX_NO_OF_SLIDES))
    {
        size += sizeof(SIS_PAR_START) + SIS_INTSZ + sizeof(SIS_PAR_END);
        if (list->text.type != SIS_NOT_USED)
        {
            size += sizeof(SIS_TEXT_T1) + sizeof(SIS_TEXT_T2) + 
                sizeof(SIS_TEXT_T3) + sizeof(SIS_TEXT_T4) + sizeof(SIS_TEXT_T5) +
                sizeof(SIS_TEXT_T5) + strlen(list->text.src) + 
                strlen(list->text.region) + 3 * SIS_INTSZ;
        }
        if (list->audio.type != SIS_NOT_USED)
        {
			size += sizeof(SIS_AUDIO_T1) + sizeof(SIS_AUDIO_T2) +
                sizeof(SIS_AUDIO_T3) + sizeof(SIS_AUDIO_T4) + 
                sizeof(SIS_AUDIO_T5) + strlen(list->audio.src) + 2 * SIS_INTSZ;
        }        
		if ((list->image.type != SIS_NOT_USED))
        {
            size += sizeof(SIS_IMAGE_T1) + sizeof(SIS_IMAGE_T2) + 
                sizeof(SIS_IMAGE_T3) + sizeof(SIS_IMAGE_T4) + 
                sizeof(SIS_IMAGE_T4) + strlen(list->image.src) + 
                strlen(list->image.region) + 2 * SIS_INTSZ;
        }
		if ((list->video.type != SIS_NOT_USED))
        {
            size += sizeof(SIS_VIDEO_T1) + sizeof(SIS_VIDEO_T2) + 
                sizeof(SIS_VIDEO_T3) + sizeof(SIS_VIDEO_T4) +
                sizeof(SIS_VIDEO_T4) + strlen(list->video.src) + strlen(list->video.region) + 2*SIS_INTSZ;
        }
        list = list->next;
		slideCnt++;
    }

    /* Alloc space */
    *buffer = WE_MEM_ALLOC(modId, size + 1);  
    if (*buffer == NULL)
    {
        return SIS_RESULT_ERROR;
    }

    /* Copy data */
    strcpy(*buffer, SIS_SMIL_START);
    strcat(*buffer, SIS_HEAD_START);
    strcat(*buffer, SIS_LAYOUT_START);

    /* Copy rootlayout */
    if (p->rootLayout != NULL)
    {
        strcat(*buffer, SIS_ROOT_LAYOUT_T1);
        Sis_itoa(p->rootLayout->cx, tmp);
        strcat(*buffer, tmp);
        strcat(*buffer, SIS_ROOT_LAYOUT_T2);
        Sis_itoa(p->rootLayout->cy, tmp);
        strcat(*buffer, tmp);
        /* Append the root-layout color if defined */
        if (SIS_COLOR_NOT_USED != p->rootLayout->bgColor)
        {
            strcat(*buffer, SIS_ROOT_LAYOUT_T3);
            Sis_itoacol(p->rootLayout->bgColor, tmp);
            strcat(*buffer, tmp);
        }
        strcat(*buffer, SIS_ROOT_LAYOUT_T4);
    }

    /* Copy regions */
    regionNode = p->regionList;
    while (regionNode != NULL)
    {
        strcat(*buffer, SIS_REGION_T1);
        strcat(*buffer, regionNode->name);
        strcat(*buffer, SIS_REGION_T2);
        Sis_itoa(regionNode->data.size.cx, tmp);
        strcat(*buffer, tmp);
        strcat(*buffer, SIS_REGION_T3);
        Sis_itoa(regionNode->data.size.cy, tmp);
        strcat(*buffer, tmp);
        strcat(*buffer, SIS_REGION_T4);
        Sis_itoa(regionNode->data.position.cx, tmp);
        strcat(*buffer, tmp);
        strcat(*buffer, SIS_REGION_T5);
        Sis_itoa(regionNode->data.position.cy, tmp);
        strcat(*buffer, tmp);
        strcat(*buffer, SIS_REGION_T6);
        regionNode = regionNode->next;
    }

    strcat(*buffer, SIS_LAYOUT_END);
    strcat(*buffer, SIS_HEAD_END);
    strcat(*buffer, SIS_BODY_START);

    /* Copy slides */
    list = p->slideList;
    slideCnt = 0;
    while (list != NULL && (slideCnt < MAX_NO_OF_SLIDES))
    {
        /* Copy duration */
        sprintf(tmp, SIS_PAR_START, list->duration);
        strcat(*buffer, tmp);
        
        /* Copy text MO */
        if (list->text.type != SIS_NOT_USED)
        {
            sprintf(tmp, SIS_TEXT_T1, list->text.src, list->text.region);
            strcat(*buffer, tmp);
            /* the begin param */
            if (0 != list->text.beginT)
            {
                sprintf(tmp, SIS_TEXT_T2, list->text.beginT);
                strcat(*buffer, tmp);
            }
            if (0 != list->text.endT)
            {
                sprintf(tmp, SIS_TEXT_T3, list->text.endT);
                strcat(*buffer, tmp);
            }
            if (0 != list->text.duration)
            {
                sprintf(tmp, SIS_TEXT_T4, list->text.duration);
                strcat(*buffer, tmp);
            }
            /* Check if the color parameter needs to be set */
            if (SIS_COLOR_NOT_USED == list->text.color)
            {
                /* Append the end tag */
                strcat(*buffer, SIS_TEXT_T6);
            }
            else
            {
                /* append the param tag and the text end tag */
                strcat(*buffer, SIS_TEXT_T7);
                Sis_itoacol(list->text.color, tmpStrIntsz);
                sprintf(tmp, SIS_TEXT_T5, tmpStrIntsz);
                strcat(*buffer, tmp);
            }
        }        
		if (list->image.type != SIS_NOT_USED)
        {
            sprintf(tmp, SIS_IMAGE_T1, list->image.src, list->image.region);
            strcat(*buffer, tmp);
            /* begin time */
            if (0 != list->image.beginT)
            {
                sprintf(tmp, SIS_IMAGE_T2, list->image.beginT);
                strcat(*buffer, tmp);
            }
            if (0 != list->image.endT)
            {
                sprintf(tmp, SIS_IMAGE_T3, list->image.endT);
                strcat(*buffer, tmp);
            }
            /* Duration */
            if (0 != list->image.duration)
            {
                sprintf(tmp, SIS_IMAGE_T4, list->image.duration);
                strcat(*buffer, tmp);
            }
            /* Append the end tag */
            strcat(*buffer, SIS_IMAGE_T5);
        }
        if (list->audio.type != SIS_NOT_USED)
        {
            sprintf(tmp, SIS_AUDIO_T1, list->audio.src);
            strcat(*buffer, tmp);
            /* begin time */
            if (0 != list->audio.beginT)
            {
                sprintf(tmp, SIS_AUDIO_T2, list->audio.beginT);
                strcat(*buffer, tmp);
            }
            /* End time */
            if (0 != list->audio.endT)
            {
                sprintf(tmp, SIS_AUDIO_T3, list->audio.endT);
                strcat(*buffer, tmp);
            }
            /* Duration */
            if (0 != list->audio.duration)
            {
                sprintf(tmp, SIS_AUDIO_T4, list->audio.duration);
                strcat(*buffer, tmp);
            }
            /* Append the end tag */
            strcat(*buffer, SIS_AUDIO_T5);
        }
		if (list->video.type != SIS_NOT_USED)
        {
            sprintf(tmp, SIS_VIDEO_T1, list->video.src, list->video.region);
            strcat(*buffer, tmp);
            /* begin time */
            if (0 != list->video.beginT)
            {
                sprintf(tmp, SIS_VIDEO_T2, list->video.beginT);
                strcat(*buffer, tmp);
            }
            /* End time */
            if (0 != list->video.endT)
            {
                sprintf(tmp, SIS_VIDEO_T3, list->video.endT);
                strcat(*buffer, tmp);
            }
            /* Duration */
            if (0 != list->image.duration)
            {
                sprintf(tmp, SIS_VIDEO_T4, list->image.duration);
                strcat(*buffer, tmp);
            }
            /* Append the end tag */
            strcat(*buffer, SIS_VIDEO_T5);
        }

        strcat(*buffer, SIS_PAR_END);
        list = list->next;
		slideCnt++;
    }

    strcat(*buffer, SIS_BODY_END);
    strcat(*buffer, SIS_SMIL_END);

    /* Free data */
    result = SISlib_cancelCreateSmil(modId, handle);
#ifdef WE_LOG_FC  
    logResult = (WE_UINT32) result;
	size = strlen(*buffer);
    WE_LOG_FC_UINT32 ( logResult, NULL);
	/*WE_LOG_FC_BYTES ( *buffer, size, NULL);	*/
	WE_LOG_FC_STRING ( *buffer, NULL);	
    WE_LOG_FC_END
#endif

    return result;
}


/*!
 * \brief   Get the parameter value as string from a SlsParamList. 
 * 
 * \param   list, the list of parameters.
 * \param   name, the name of the paramter to retrieve value for.
 * \return  value of the parameter as constant string.
 *****************************************************************************/
const char * SISlib_getParamValueAsString(const SlsParamList * list, const char * name)
{
    
    if (!list) 
    {
        return NULL;
    }
    else
    {
        while (list) 
        {
            if (list->param.name) 
            {
                if (we_cmmn_strcmp_nc(list->param.name, name) == 0) 
                {
                    return list->param.value;
                }
            }

            list = list->next;
        }
         
    }

    return NULL;
}

/*!
 * \brief    Gets the value of a defined color.
 * 
 * \param    list, the list of parameters.
 * \param    name, the name of the parameter to retrieve value for.
 * \return   the value of the color as a value.
 *****************************************************************************/
WE_UINT32 SISlib_getColorValue(const SlsParamList * list, const char * name)
{
    
    if (!list) 
    {
        return SIS_COLOR_NOT_USED;
    }
    else
    {
        while (list) 
        {
            if (list->param.name) 
            {
                if (we_cmmn_strcmp_nc(list->param.name, name) == 0) 
                {
                    return colorSTOI(list->param.value);
                }
            }

            list = list->next;
        }
         
    }

    return SIS_COLOR_NOT_USED;
}


/*!
 * \brief Converts a serielized signal buffer into signal struct
 * The function is called from WE when signals is to be converted.
 * 
 * \param modId The calling module identity
 * \param signal Signal identity
 * \param buffer The signal buffer
 *****************************************************************************/
void *slsConvert(WE_UINT8 module, 
                 WE_UINT16 signal, 
                 void *buffer)
{
    we_dcvt_t  cvt;
    WE_UINT16  length;
 
    if (buffer == NULL)
    {
        return NULL;
    }

    buffer = WE_SIGNAL_GET_USER_DATA (buffer, &length);

    if ((buffer == NULL) || (length == 0))
    {
        return NULL;
    }

    we_dcvt_init (&cvt, WE_DCVT_DECODE, buffer, length, module);

    switch (signal)
    {
    case SIS_OPEN_SMIL_REQ:
        {
            SisOpenSmilReq *data = WE_MEM_ALLOCTYPE (module, SisOpenSmilReq);
            if (!data)
            {
                return NULL;
            }
            if (!Sis_cvt_SlsOpenSmilReq(&cvt, data))
            {
                return NULL;
            }
            return data;
        }     

    case SIS_OPEN_SMIL_RSP:
        {
            SisOpenSmilRsp *data = WE_MEM_ALLOCTYPE (module, SisOpenSmilRsp);
            if (!data)
            {
                return NULL;
            }     
            if (!Sis_cvt_SlsOpenSmilRsp(&cvt, data)) 
            {
                return NULL;
            }
            return data;        
        }
    case SIS_CLOSE_SMIL_REQ:
        {
            WE_UINT32 *data = WE_MEM_ALLOCTYPE (module, WE_UINT32);

            if (!data) 
            {
                return NULL;
            }
            if (!we_dcvt_uint32(&cvt, data)) 
            {
                return NULL;
            }
            return data;

        }        
    case SIS_CLOSE_SMIL_RSP:
        {
            SisCloseSmilRsp *data = WE_MEM_ALLOCTYPE (module, SisCloseSmilRsp);
            if (!data)
            {
                return NULL;
            }
            if (!Sis_cvt_SlsCloseSmilRsp(&cvt, data))
            {
                return NULL;
            }
            
            return data;
        }     
        
    case SIS_GET_SLIDE_INFO_REQ:
        {
            SisGetSlideInfoReq *data = WE_MEM_ALLOCTYPE (module, SisGetSlideInfoReq);
            if (!data)
            {
                return NULL;
            }
            if (!Sis_cvt_SlsGetSlideInfoReq(&cvt, data))
            {
                return NULL;
            }
            return data;
        }     

    case SIS_GET_SLIDE_INFO_RSP:
        {

            SisGetSlideInfoRsp *data = WE_MEM_ALLOCTYPE (module, SisGetSlideInfoRsp);
            if (!data)
            {
                return NULL;
            }           
            if (!Sis_cvt_SlsGetSlideInfoRsp(&cvt, data)) 
            {
                return NULL;
            }
            return data;

        }     
        
    default:
        break;
    }


  return NULL;
} /* slsConvert */


/*!
 * \brief Deallocate SisUnrefItemList.
 *
 * \param module, the calling module.
 * \param list, list to deallocate.
 *****************************************************************************/
void SISif_freeSlsUnrefItemList(WE_UINT8 module, SisUnrefItemList *list)
{
    SisUnrefItemList *tmpList  = list;
    SisUnrefItemList *prevList = NULL;

    while (tmpList) 
    {
        prevList = tmpList;
        tmpList  = tmpList->next;

        if (prevList != list)
        {
            /* don't free list root */
            WE_MEM_FREE(module, prevList);
        }        
    }
}

/*!
 * \brief Deallocate SisRegionList.
 *
 * \param module, the calling module.
 * \param list, list to deallocate.
 *****************************************************************************/
void SISif_freeSlsRegionList(WE_UINT8 module, SisRegionList *list)
{
    SisRegionList *tmpList  = list;
    SisRegionList *prevList = NULL;

    while (tmpList) 
    {
        prevList = tmpList;
        tmpList  = tmpList->next;

        if (prevList != list)
        {
            if (prevList->region.name != NULL)
            {
                WE_MEM_FREE(module, prevList->region.name);
            }
            WE_MEM_FREE(module, prevList);
        }
    }
}


/*!
 * \brief Deallocate SisObjectInfo.
 *
 * \param module, the calling module.
 * \param obj, object to deallocate.
 *****************************************************************************/
void SISif_freeSlsObjectInfo(WE_UINT8 module, SisObjectInfo *obj)
{
    if (obj == NULL)
    {
        return;
    } /* if */
    
    MMSif_freeMmsContentType(module, &obj->contentType, FALSE);
} /* SISif_freeSlsObjectInfo */

/*!
 * \brief Deallocate SisObjectInfoList.
 *
 * \param module, the calling module.
 * \param list, list to deallocate.
 *****************************************************************************/
void SISif_freeSlsObjectInfoList(WE_UINT8 module, SisObjectInfoList *list)
{
    SisObjectInfoList *tmpList  = list;
    SisObjectInfoList *prevList = NULL;

    while (tmpList) 
    {
        prevList = tmpList;
        tmpList  = tmpList->next;

        SISif_freeSlsObjectInfo(module, &prevList->current);
        if (prevList != list)
        {
            /* do not free root item */
            WE_MEM_FREE(module, prevList);
        } /* if */
    }
}


/*!
 * \brief Deallocate SisOpenSmilRsp.
 *
 * \param module, the calling module.
 * \param data, pointer to object to deallocate.
 *****************************************************************************/
void SISif_freeSisOpenSmil(WE_UINT8 module, SisOpenSmilRsp *data)
{
    if(data)
    {       
        if (data->unrefItems)
        {
            SISif_freeSlsUnrefItemList(module, data->unrefItems);
            WE_MEM_FREE(module, data->unrefItems);
            data->unrefItems = NULL;
        }
        if (data->regionList)
        {
            SISif_freeSlsRegionList(module, data->regionList);
            WE_MEM_FREE(module, data->regionList);
            data->regionList = NULL;
        }
        if (data->objectList)
        {
            SISif_freeSlsObjectInfoList(module, data->objectList);
            WE_MEM_FREE(module, data->objectList);
            data->objectList = NULL;
        }
     }
}

/*!
 * \brief Deallocate SlsParamList.
 *
 * \param module, the calling module.
 * \param list, list to deallocate.
 *****************************************************************************/
void SISif_freeSlsParamList(WE_UINT8 module, SlsParamList *list)
{
    SlsParamList *tmpList  = list;
    SlsParamList *prevList = NULL;

    while (tmpList) 
    {
        prevList = tmpList;
        tmpList  = tmpList->next;
        
        if (prevList->param.name != NULL)
        {
            WE_MEM_FREE(module, prevList->param.name);
        }
        if (prevList->param.value != NULL)
        {
            WE_MEM_FREE(module, prevList->param.value);
        }
        
        if (prevList != list)
        {
            /* don't free list root */
            WE_MEM_FREE(module, prevList);
        }
        
    }

}

/*!
 * \brief Deallocate SisSlideElement.
 *
 * \param module, the calling module.
 * \param slideElement, pointer to object to free.
 *****************************************************************************/
void SISif_freeSlsSlideElement(WE_UINT8 module, const SisSlideElement *slideElement)
{
    if (slideElement) 
    {
        
        if (slideElement->alt != NULL)
        {
            WE_MEM_FREE(module, slideElement->alt);
        }
        SISif_freeSlsParamList(module, slideElement->paramList);        
    }
}

/*!
 * \brief Deallocate SisSlideElementList.
 *
 * \param module, the calling module.
 * \param list, list to deallocate.
 *****************************************************************************/
void SISif_freeSlsSlideElementList(WE_UINT8 module, SisSlideElementList *list)
{
    SisSlideElementList *tmpList  = list;
    SisSlideElementList *prevList = NULL;

    while (tmpList) 
    {
        prevList = tmpList;
        tmpList  = tmpList->next;

        SISif_freeSlsSlideElement(module, &prevList->current);
        if (prevList != list)
        {
            /* do not free root item */
            WE_MEM_FREE(module, prevList);
        } /* if */
        
    }
}


/*!
 * \brief Deallocate SisSlideInfoList.
 *
 * \param module, the calling module.
 * \param data, list to deallocate.
 *****************************************************************************/
void SISif_freeSlsSlideInfoList(WE_UINT8 module, SisSlideInfoList *list)
{
    SisSlideInfoList *tmpList  = list;
    SisSlideInfoList *prevList = NULL;

    while (tmpList) 
    {
        prevList = tmpList;
        tmpList  = tmpList->next;

        if (prevList->current.elementList)
        {
            SISif_freeSlsSlideElementList(module, prevList->current.elementList);
            WE_MEM_FREE(module, prevList->current.elementList);
        }
        
        
        if (prevList != list)
        {
            /* do not free root item */
            WE_MEM_FREE(module, prevList);
        } /* if */
    }
}

/*!
 * \brief Deallocate SisGetSlideInfoRsp
 *
 * \param module, the calling module.
 * \param data, pointer to object to deallocate.
 *****************************************************************************/
void SISif_freeSisGetSlideInfo(WE_UINT8 module, const SisGetSlideInfoRsp *data)
{
    if (data) 
    {
        SISif_freeSlsSlideInfoList(module, data->slideInfo);
    }
}


/*!
 * \brief Deallocate a signal buffer
 *
 * This function is used by the WE to free signal memory.
 *
 * Type: Function call
 *
 * \param modId The calling module identity
 * \param signal Signal identity
 * \param p The signal buffer
 *****************************************************************************/
void slsDestruct(WE_UINT8 module, WE_UINT16 signal, void* p)
{
    we_dcvt_t cvt;

    if (p == NULL)
    {
        return;
    }
    
    we_dcvt_init (&cvt, WE_DCVT_FREE, NULL, 0, module);
    
    switch (signal)
    {
    case SIS_OPEN_SMIL_REQ:
        {
            SisOpenSmilReq *openSmilReq = (SisOpenSmilReq*)p;

            if (!Sis_cvt_SlsOpenSmilReq(&cvt, openSmilReq))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
                    "%s(%d): Destruct failed.\n", __FILE__, __LINE__));
               
            }
            
        }
        break;
    case SIS_OPEN_SMIL_RSP:
        {
            SisOpenSmilRsp *openSmilRsp = (SisOpenSmilRsp*)p;
            if (!Sis_cvt_SlsOpenSmilRsp(&cvt, openSmilRsp))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
                    "%s(%d): Destruct failed.\n", __FILE__, __LINE__));
            }
			
		}
        break;
    case SIS_GET_SLIDE_INFO_REQ:
        {
            SisGetSlideInfoReq *getSlideInfoReq = (SisGetSlideInfoReq *)p;

            if (!Sis_cvt_SlsGetSlideInfoReq(&cvt, getSlideInfoReq))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
                    "%s(%d): Destruct failed.\n", __FILE__, __LINE__));
            }

        }
        break;
	case SIS_GET_SLIDE_INFO_RSP:
		{
			SisGetSlideInfoRsp *slideInfoRsp = (SisGetSlideInfoRsp*)p;
						
            if (!Sis_cvt_SlsGetSlideInfoRsp(&cvt, slideInfoRsp))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
                    "%s(%d): Destruct failed.\n", __FILE__, __LINE__));
            }
		
			break;
		}
    case SIS_CLOSE_SMIL_REQ:
        {
            WE_UINT32 *closeSmilReq = (WE_UINT32 *)p;

            if (!we_dcvt_uint32(&cvt, closeSmilReq)) 
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
                    "%s(%d): Destruct failed.\n", __FILE__, __LINE__));
            }
        }
        break;
    case SIS_CLOSE_SMIL_RSP:
        {
            SisCloseSmilRsp *closeSmilRsp = (SisCloseSmilRsp *)p;

            if (!Sis_cvt_SlsCloseSmilRsp(&cvt, closeSmilRsp))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
                    "%s(%d): Destruct failed.\n", __FILE__, __LINE__));
            }

        }
        break;
    default:
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
                    "%s(%d): Destruct failed.\n", __FILE__, __LINE__));
        break;
    }
    we_dcvt_init (&cvt, WE_DCVT_FREE, NULL, 0, module);
    
    WE_MEM_FREE (module, p);

} /* slsDestruct */


/*!
 * \brief Allocs and copies a null terminated string
 * 
 *****************************************************************************/
static char *Sis_strdup(WE_UINT8 modId, const char *s)
{
    char *str2;
    
    if (!s)
    {
        return NULL;
    }
    
    str2 = WE_MEM_ALLOC(modId, strlen(s) + 1);
    if (!str2)
    {
        return NULL;
    }
    strcpy(str2, s);
    return str2;
}


/*!
 * \brief Reverse a string
 * 
 *****************************************************************************/
void Sis_reverse(char *s)
{
    char *j;
    int c;

    j = s + strlen(s) - 1;
    while(s < j) 
    {
        c = *s;
        *s++ = *j;
        *j-- = (char)c;
    }
}


/*!
 * \brief Converts an integer to characters and if the value is relative 
 *        add an %
 * 
 *****************************************************************************/
static void Sis_itoa(SisSizeValue val, char *s)
{
    int i;
    int n = val.value;
    char *c;
    
    c = s;
    if ((i = n) < 0)
    {
        n = -n;
    }
    do 
    {
        *c++ = (char)(n % 10 + '0');
    } while ((n = n / 10) > 0);
    if (i < 0)
    {
        *c++ = '-';
    }

    *c = '\0';

    Sis_reverse(s);

    if (val.type == SIS_REL)
    {
        *c++ = '%';
        *c = '\0';
    }
}



/*! \brief Copies an SisMediaObject struct
 *
 * \param modId modules id 
 * \param source Source data
 * \param dest Destination data
 *****************************************************************************/
static WE_BOOL copySlsMediaObj(WE_UINT8 modId, const SisMediaObject *source, 
	SisMediaObject *dest)
{
	WE_BOOL ret = TRUE;

    if (dest == NULL) 
    {
        /* no destination */
        return FALSE;
    } /* if */
	
	memset(dest, 0x00, sizeof(SisMediaObject));
	if (source == NULL)
	{
		/* no source */
		dest->type = SIS_NOT_USED;
		return TRUE;
	} /* if */
	
	dest->beginT = source->beginT;
	dest->color = source->color;
	dest->endT = source->endT;
    dest->duration = source->duration;
	dest->region = Sis_strdup(modId, source->region);
	dest->src = Sis_strdup(modId, source->src);
	dest->type = source->type;
	
	if ((dest->region == NULL) && (source->region != NULL))
	{
		/* error */
		if (dest->src != NULL)
		{
			WE_MEM_FREE(modId, dest->src);
			dest->src = NULL;
			ret = FALSE;
		} /* if */
	} /* if */

	if ((dest->src == NULL) && (source->src != NULL))
	{
		/* error */
		if (dest->region != NULL)
		{
			WE_MEM_FREE(modId, dest->region);
			dest->region = NULL;
			return FALSE;
		} /* if */
	} /* if */

	return ret;
} /* copySlsMediaObj */


/*! \brief Copies an SisRegion struct
 *
 * \param source Source data
 * \param dest Destination data
 *****************************************************************************/
void SISif_copySlsRegion(const SisRegion *source, SisRegion *dest)
{
    if (source == NULL || dest == NULL) 
    {
        /* no input */
        return;
    } /* if */
    
    dest->position.cx.type = source->position.cx.type;
    dest->position.cx.value= source->position.cx.value;
    dest->position.cy.type = source->position.cy.type;
    dest->position.cy.value= source->position.cy.value;
    dest->size.cx.type = source->size.cx.type;
    dest->size.cx.value= source->size.cx.value;
    dest->size.cy.type = source->size.cy.type;
    dest->size.cy.value= source->size.cy.value;
	dest->fit= source->fit;
} /* copySlsRegion */


/*! \brief Copies an SisRegionList
 *
 * \param source Source data
 * \param dest Destination data
 *****************************************************************************/
void SISif_copySlsRegionList(WE_UINT8 module, const SisRegionList *source, 
    SisRegionList *dest)
{
    if (dest == NULL)
    {
        return;
    } /* if */

    while (source)
    {
        SISif_copySlsRegion(&source->region, &dest->region);
        dest->id = source->id;

        if (source->next != NULL)
        {
            dest->next = WE_MEM_ALLOC(module, sizeof(SisRegionList));
            memset(dest->next, 0x00, sizeof(SisRegionList));
            dest = dest->next;                        
        } /* if */

        source = source->next;
    } /* while */
} /* SISif_copySlsRegionList */


/*! \brief Copies an SisRegion struct
 *
 * \param source Source data
 * \param dest Destination data
 *****************************************************************************/
void SISif_copySlideInfo(WE_UINT8 module, const SisSlideInfo *source, 
    SisSlideInfo *dest)
{
    if (source == NULL || dest == NULL) 
    {
        /* no input */
        return;
    } /* if */
    
    if (source->elementList != NULL)
    {
        dest->elementList = WE_MEM_ALLOC(module, sizeof(SisSlideElementList));
        memset(dest->elementList, 0x00, sizeof(SisSlideElementList));
        SISif_copySlsSlideElementList(module, source->elementList, 
            dest->elementList);
    } /* if */

    dest->duration = source->duration;
} /* SISif_copySlideInfo */

/*! \brief Copies an SisSlideInfoList list.
 *
 * \param module, the calling module.
 * \param source Source data.
 * \param dest Destination data.
 *****************************************************************************/
void SISif_copySlideInfoList(WE_UINT8 module, 
    const SisSlideInfoList *source, SisSlideInfoList *dest)
{
    if (source == NULL || dest == NULL)
    {
        return;
    }

    while (source)
    {
        SISif_copySlideInfo(module, &source->current, &dest->current);
        if (source->next != NULL)
        {
            dest->next = WE_MEM_ALLOC(module, sizeof(SisSlideInfoList));
            memset(dest->next, 0x00, sizeof(SisSlideInfoList));
            dest = dest->next;
        } /* if */
        
        source = source->next;
    } /* while */
} /* SISif_copySlideInfoList */

/*! \brief Copies an SisObjectInfo struct.
 *
 * \param module, the calling module.
 * \param source Source data.
 * \param dest Destination data.
 *****************************************************************************/
void SISif_copyObjectInfo(WE_UINT8 module, const SisObjectInfo *source, 
    SisObjectInfo *dest)
{
    if (source == NULL || dest == NULL)
    {
        return;
    }

    (void)MMSif_copyMmsContentType(module, &dest->contentType, 
        (MmsContentType *)&source->contentType);
    dest->id = source->id;
    dest->size = source->size;
} /* SISif_copyObjectInfo */

/*! \brief Copies an SisObjectInfoList list.
 *
 * \param module, the calling module.
 * \param source Source data.
 * \param dest Destination data.
 *****************************************************************************/
void SISif_copyObjectInfoList(WE_UINT8 module, 
    const SisObjectInfoList *source, SisObjectInfoList *dest)
{
    if (source == NULL || dest == NULL)
    {
        return;
    }

    while (source)
    {
        SISif_copyObjectInfo(module, &source->current, &dest->current);
        if (source->next != NULL)
        {
            dest->next = WE_MEM_ALLOC(module, sizeof(SisObjectInfoList));
            memset(dest->next, 0x00, sizeof(SisObjectInfoList));
            dest = dest->next;
        } /* if */
        
        source = source->next;
    } /* while */
} /* SISif_copyObjectInfoList */


/*! \brief Copies an SisRootLayout struct
 *
 * \param source Source data
 * \param dest Destination data
 *****************************************************************************/
void SISif_copySlsRootLayout(const SisRootLayout *source, SisRootLayout *dest)
{
	if (source == NULL || dest == NULL) 
    {
        /* no input */
        return;
    } /* if */

	dest->bgColor = source->bgColor;
	dest->cx.type = source->cx.type;
	dest->cx.value = source->cx.value;
	dest->cy.type = source->cy.type;
	dest->cy.value = source->cy.value;
} /* copySlsRootLayout */


/*! \brief Copies an SlsParamList list.
 *
 * \param module, the calling module.
 * \param source Source data.
 * \param dest Destination data.
 *****************************************************************************/
static void SISif_copySlsParamList(WE_UINT8 module, const SlsParamList *source, 
    SlsParamList *dest)
{
    if (source == NULL || dest == NULL)
    {
        return;
    } /* if */

    memset(dest, 0x00, sizeof(SlsParamList));
    
    while (source)
    {
       /* dest->param.key = source->param.key; */
        dest->param.value = we_cmmn_strdup(module, source->param.value);
        dest->param.name = we_cmmn_strdup(module, source->param.name);
       
        if (source->next)
        {
            dest->next = WE_MEM_ALLOC(module, sizeof(SlsParamList));
            memset(dest->next, 0x00, sizeof(SlsParamList));
            dest = dest->next;
        } /* if */
        source = source->next;
    } /* while */
} /* SISif_copySlsParamList */


/*! \brief Copies an SisSlideElement struct.
 *
 * \param module, the calling module.
 * \param source Source data.
 * \param dest Destination data.
 *****************************************************************************/
void SISif_copySlsSlideElement(WE_UINT8 module, const SisSlideElement *source, 
    SisSlideElement *dest)
{
    if (source == NULL || dest == NULL)
    {
        return;
    } /* if */

    dest->objectType    = source->objectType;

    dest->objectId      = source->objectId;
    dest->regionId      = source->regionId;

    dest->fgColor = source->fgColor;
    dest->bgColor = source->bgColor;
    
    dest->beginT = source->beginT;
    dest->endT = source->endT;
    
    dest->duration = source->duration;           
    
    dest->alt = we_cmmn_strdup(module, source->alt);

    if (source->paramList != NULL)
    {
        dest->paramList = WE_MEM_ALLOC(module, sizeof(SlsParamList));
        SISif_copySlsParamList(module, source->paramList, dest->paramList);
    } /* if */
    
} /* SISif_copySlsSlideElement */


/*! \brief Copies an SisSlideElementList list.
 *
 * \param module, the calling module.
 * \param source Source data.
 * \param dest Destination data.
 *****************************************************************************/
void SISif_copySlsSlideElementList(WE_UINT8 module, 
    const SisSlideElementList *source, SisSlideElementList *dest)
{
    if (source == NULL || dest == NULL)
    {
        return;
    }

    while (source)
    {
        SISif_copySlsSlideElement(module, &source->current, &dest->current);
        if (source->next != NULL)
        {
            dest->next = WE_MEM_ALLOC(module, sizeof(SisSlideElementList));
            memset(dest->next, 0x00, sizeof(SisSlideElementList));
            dest = dest->next;
        } /* if */
        
        source = source->next;
    }
} /* SISif_copySlsSlideElement */

/*! \brief Retrieves the region for a specific region ID.
 *
 * \param regions
 * \param id The ID to find.
 * \return The region corresponding to the ID or NULL if the region does not
 *         exists
 *****************************************************************************/
SisRegion *SISif_getRegionFromId(SisRegionList *regions, WE_UINT32 id)
{
    while (regions != NULL)
    {
        if (regions->id == id)
        {
            return &(regions->region);
        }
        regions = regions->next;
    }
    return NULL;
}

/*! \brief Retrieves the media object with a specific type
 *	
 * \param elemList
 * \return A reference to a media object or NULL if no object exists.
 *****************************************************************************/
SisSlideElement *SISif_getElementByType(SisSlideElementList *elemList,
    SlsMediaObjectType sType)
{
    /* Go through the list of elements */
    while(elemList != NULL)
    {
        /* Check if this is the correct type */
        if (elemList->current.objectType == sType)
        {
            /* Correct type found return the reference */
            return &(elemList->current);
        }
        elemList = elemList->next;
    }
    return NULL;    
}

/*! \brief Converts an integer representing a color (rgb 8-bit each) into a 
 *         string.
 *
 * \param color The color to convert into a string.
 * \param s the buffer to write to, must be at least 5 bytes.
 *****************************************************************************/
static void Sis_itoacol(WE_UINT32 color, char *s)
{
    /* Set the has-mark first to indicate a hex value */
    s[0] = SIS_HASH_MARK;

    /* Set the color data */
    we_cmmn_byte2hex((unsigned char)(color>>16), &(s[1]));
    we_cmmn_byte2hex((unsigned char)(color>>8), &(s[3]));
    we_cmmn_byte2hex((unsigned char)color, &(s[5]));
    s[7] = WE_EOS;
} /* Sis_itoacol */


  
/*
 * Scripting stuff
 */

#ifdef WE_LOG_FC  

WE_INT32 filehandle;
unsigned char buf[4096], *buf2, *buf3;
WE_UINT32  i, j;
SisCreateSmilHandle handle;
SisRootLayout rootLayout;
SisSize size = { {100, SIS_ABS}, {200, SIS_ABS} };
SisRegion region;
/*SisMediaObject medObj = {SIS_TEXT_PLAIN, "MyRegion", "src"};*/
SisMediaObject textObj  = {SIS_NOT_USED, "    \0", "              \0"};
SisMediaObject imgObj   = {SIS_NOT_USED, "     \0", "         \0"};
SisMediaObject audioObj = {SIS_NOT_USED, "     \0", "         \0"};
SisMediaObject videoObj  = {SIS_NOT_USED, "    \0", "              \0"};

WE_LOG_FC_DISPATCH_MAP_BEGIN(slsif)

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(SISif_startInterface)
SISif_startInterface();
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(SISif_openSmil)
SISif_openSmil(0, 0, 0);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(SISif_closeSmil)
SISif_closeSmil(0, 0);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(SISif_getSlideInfo)
SISif_getSlideInfo(0, 0, 0, 0);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END
 
WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(SISlib_startGetMediaObject)
SISlib_startGetMediaObject(0, &filehandle, 0, 0);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(SISlib_getMediaObject)
j = sizeof(buf);
SISlib_getMediaObject(0, 0, buf, &j, &i, filehandle);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(SISlib_stopGetMediaObject)
SISlib_stopGetMediaObject(filehandle);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(SISlib_createSmil)
SISlib_createSmil(0, &handle, &rootLayout);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(SISlib_cancelCreateSmil)
SISlib_cancelCreateSmil(0, &handle);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(SISlib_createRegion)
SISlib_createRegion(0, &handle, &region, (char*)buf);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

imgObj.type   = SIS_NOT_USED;
imgObj.region = "     \0";
imgObj.src    = "         \0";

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(SISlib_createSlide)
SISlib_createSlide(0, &handle, 0, &textObj, &imgObj, &videoObj, &audioObj);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(SISlib_generateSmil)
SISlib_generateSmil(0, &handle, (char**)&buf2);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_END
#endif 



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
/*! \file Sis_if.h
 * \brief  SMIL service exported functions
 */

#ifndef SIS_IF_H
#define SIS_IF_H

#ifndef SIS_CFG_H
#error You must first include Sis_cfg.h
#endif

#include "Sis_def.h"
#include "Mms_Def.h"


/*--- Macros ---*/

/* 
 * Used in SisSizeValue to define the type of the value 
 */
#define SIS_ABS              0  /*!< Value is absolute */ 
#define SIS_REL              1  /*!< Value is relative */ 

/*! Used when setting colors for root-layout and text when no color needs to be
    set */
#define SIS_COLOR_NOT_USED      (0xFF000000)

/*! Used when no matching bodypart is found for slide element. */

#define SIS_BODYPART_NOT_FOUND  (-1)

/*--- Types ---*/

/*! \enum SisResult Result values sent from the SMIL module. */
typedef enum
{
    /*! The operation is successful and finished. */
    SIS_RESULT_OK,

    /*! Too many consecutive calls to a API function has been made 
     *  without waiting for the outcome of the previous calls.
     */
    SIS_RESULT_BUSY,

    /*! The SMIL has not been created.
     */
    SIS_RESULT_SMIL_NOT_CREATED,

    /*! An unknown error has occurred. */
    SIS_RESULT_ERROR,

    /*! The slide has no media objects. */
    SIS_RESULT_MISSING_MEDIA_OBJECTS,

    /*! The media object has no src. */
    SIS_RESULT_MISSING_MEDIA_OBJECT_SRC,

    /*! The SMIL content is invalid. */
    SIS_RESULT_SMIL_INVALID,

    /*! Read file has been delayed, wait for WE signal WE_FILE_EVENT_READ. */
    SIS_RESULT_FILE_DELAY,

    /*! Could not read from the source file. */
    SIS_RESULT_FILE_READ_FAILURE,

    /*! The buffer was filled during file read operation */
    SIS_RESULT_BUFFER_FULL,

	/*! SMIL parsing failed, XML corrupt */
    SIS_RESULT_PARSING_FAILED,

	/*! The slide has no body part */
    SIS_RESULT_MISSING_BODY_PART,

	/*! The SMIL has to many slides */
    SIS_RESULT_TO_MANY_SLIDES

} SisResult;

/*! \struct SlsValue
 *  \brief Defines a 'window' size use SIS_ABS or SIS_REL for type.
 */
typedef struct
{
    WE_UINT16 value; /*!< The value */
    WE_UINT16 type;  /*!< Absolute or relative (%) position*/
} SisSizeValue;

/*! \struct SisSize
 *  \brief This type is used for either size or position of windows.
 */
typedef struct
{
    SisSizeValue  cx;     /*!< Horizontal size or position */
    SisSizeValue  cy;     /*!< Vertical size or position   */
}SisSize;

/*! \struct SisRootLayout
 *  \brief This type is used to present the root layout data.
 */
typedef struct
{
    WE_UINT32    bgColor;  /*!< The background color for the display */
    SisSizeValue  cx;       /*!< Horizontal size or position */
    SisSizeValue  cy;       /*!< Vertical size or position   */
}SisRootLayout;

/*! \struct SisFit
 *  \brief Value of <fit> attribute, described in W3C SMIL 2.0
 */
typedef enum
{
    SIS_ATTR_FILL	  = 0,	/*!< Scale object to touch the edges of the box */
    SIS_ATTR_HIDDEN	  = 1,	/*!< See W3C SMIL 2.0   */
	SIS_ATTR_MEET	  = 2,	/*!< See W3C SMIL 2.0   */
	SIS_ATTR_SCROLL	  = 3,	/*!< See W3C SMIL 2.0   */
	SIS_ATTR_SLICE	  = 4,	/*!< See W3C SMIL 2.0   */
	SIS_ATTR_NOT_USED = 5	/*!< See W3C SMIL 2.0   */
}SisFit;

/*! \struct SisRegion
 *  \brief Defines the layout for a region. 
 */
typedef struct
{
    char    *name;       /*!< name of region. */
    SisSize  position;   /*!< Position from top/left		   */
    SisSize  size;       /*!< Size of region				   */
	SisFit   fit;		 /*!< fit attribute, used for image */
}SisRegion;

/*! \struct SisRegion
 *  \brief Defines the layout for a region. 
 */
typedef struct SlsRegionListSt
{
    WE_UINT32  id;
    SisRegion   region;
    struct SlsRegionListSt *next;
}SisRegionList;

/*! \struct SlsParam
 *  \brief Defines a parameter. 
 */
typedef struct 
{
    char        *name;
    char        *value;
}SlsParam;

/*! \struct SlsParamList
 *  \brief Defines the parameters in a element. 
 */
typedef struct SlsParamListSt
{
    SlsParam                param;
    struct SlsParamListSt   *next;
}SlsParamList;


/*! \struct SisSlideElement
 *  \brief Defines a slide element. 
 */
typedef struct 
{
    SlsMediaObjectType  objectType;

    WE_UINT32      regionId;
    WE_INT32       objectId;

    WE_UINT32      fgColor;        /*!< Foreground Color, SIS_COLOR_NOT_USED if not used */
    WE_UINT32      bgColor;        /*!< Background Color, SIS_COLOR_NOT_USED if not used*/
    
    WE_UINT32      beginT;         /*!< The start-time, 0 if not used */
    WE_UINT32      endT;           /*!< The stop-time, 0 if not used */
    WE_UINT32      duration;       /*!< Duration of media object */
    char            *alt;           /*!< alt param */
    SlsParamList    *paramList;     /*!< parameters for the media */
}SisSlideElement;

/*! \struct SisSlideElementList
 *  \brief Defines the elements of a slide. 
 */
typedef struct SlsSlideElementListSt
{
    SisSlideElement             current;
    struct SlsSlideElementListSt    *next;
}SisSlideElementList;

/*! \struct SisObjectInfo
 *  \brief  Holds the id, contentType and size of a bodypart.
 */
typedef struct 
{
    WE_UINT32          id;             /*!< Id for this part */     
    MmsContentType      contentType;    /*!< Content type */ 
    WE_UINT32          size;           /*!< Size of MO in bytes */
}SisObjectInfo;

/*! \struct SisObjectInfoList
 *  \brief  Defines a list of information about bodyparts.
 */
typedef struct SlsObjectInfoListSt
{
    SisObjectInfo               current;
    struct SlsObjectInfoListSt  *next;
}SisObjectInfoList;

/*! \struct 
 *  \brief  Contains information about a slide
 */
typedef struct
{
    WE_UINT32          duration;       /*!< Slide duration */
    SisSlideElementList *elementList;
}SisSlideInfo;

/*! \struct SisSlideInfoList
 *  \brief  Defines a list of information about the slides found in the SMIL. 
 */
typedef struct SlsSlideInfoListSt
{
    SisSlideInfo        current;
    struct SlsSlideInfoListSt  *next;
} SisSlideInfoList;


/*
 * !\brief List of unreferenced media objects in the SMIL message
 */
typedef struct SlsUnrefItemListSt
{
    WE_UINT32              objectId;
	struct SlsUnrefItemListSt	*next;          /*!< Next attachment */	
}SisUnrefItemList;

/*! \struct SisMediaObject
 *  \brief  Contains information about a media object (MO).
 * The strings region and src must be NULL terminated.
 */
typedef struct
{
    SisMediaType    type;    /*!< Type of MO */
    char            *region; /*!< Name of the region to use for this MO  */
    char            *src;    /*!< Reference to the MO */
    WE_UINT32      beginT;        /*!< The start-time, 0 if not used */
    WE_UINT32      endT;          /*!< The stop-time, 0 if not used */
    WE_UINT32      duration;      /*!< Duration of media object */
    WE_UINT32      color;   /*!< Color - only used for text color at the time */
} SisMediaObject;

/*! \struct 
 *  \brief Signal data for SIS_openSmil.
 */
typedef struct
{
    WE_UINT32 msgId;   /*!< Message ID */
    WE_UINT32 userData;/*!< User data */
} SisOpenSmilReq;

/*! \struct 
 *  \brief  Signal data for SIS_closeSmil response.
 */
typedef struct 
{    
    WE_UINT32          userData;
}SisCloseSmilReq;

/*! \struct 
 *  \brief  Signal data for SlsGetSlideInfo.
 */
typedef struct
{
    WE_UINT32 startSlide;      /*!< First slide in get seq. */
    WE_UINT32 numberOfSlides;  /*!< Number of slides. */
    WE_UINT32 userData;        /*!< User data. */
}SisGetSlideInfoReq;


/*! \struct 
 *  \brief  Signal data for SIS_openSmil response.
 */
typedef struct
{
    SisResult           result;     /*!< Result of the operation */
    WE_UINT8           noOfSlides; /*!< Number of slides in the SMIL */
    SisRootLayout       rootLayout;	/*!< Window size on the originators device */
	SisUnrefItemList    *unrefItems;/*!< Unreferenced items */
    SisRegionList       *regionList;/*!< A list of specified regions */
    SisObjectInfoList   *objectList;/*!< A list of all available media objects*/
    WE_UINT32          userData;
}SisOpenSmilRsp;

/*! \struct 
 *  \brief  Signal data for SIS_closeSmil response.
 */
typedef struct
{
    SisResult           result; /*!< Result of the operation */
    WE_UINT32          userData;
}SisCloseSmilRsp;

/*! \struct 
 *  \brief  Signal data for SlsGetSlideInfo response.
 */
typedef struct
{
    SisResult               result;    /*!< Result of the operation */
    SisSlideInfoList        *slideInfo; /*!< Information about the slide */
    WE_UINT32              userData;
}SisGetSlideInfoRsp;


/*!
 *  \brief  Handle used internally by SIS during SMIL creation.
 */
typedef void* SisCreateSmilHandle;

/*--- Constants ---*/

/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Signals ---*/

/*!
 * \brief Request signal for SIS_openSmil()
 * Data type: SisOpenSmilReq
 * 
 *****************************************************************************/
#define SIS_OPEN_SMIL_REQ       ((WE_MODID_SIS << 8) + 0x01)

/*!
 * \brief Response signal for SIS_openSmil()
 * Data type: SisOpenSmilRsp
 * 
 *****************************************************************************/
#define SIS_OPEN_SMIL_RSP       ((WE_MODID_SIS << 8) + 0x02)

/*!
 * \brief Request signal for SIS_closeSmil()
 * Data type: SisCloseSmilReq
 * 
 *****************************************************************************/
#define SIS_CLOSE_SMIL_REQ      ((WE_MODID_SIS << 8) + 0x03)

/*!
 * \brief Response signal for SIS_closeSmil()
 * Data type: SisCloseSmilRsp
 * 
 *****************************************************************************/
#define SIS_CLOSE_SMIL_RSP      ((WE_MODID_SIS << 8) + 0x04)

/*!
 * \brief Request signal for SIS_getSlideInfo()
 * Data type: SisGetSlideInfoReq
 * 
 *****************************************************************************/
#define SIS_GET_SLIDE_INFO_REQ  ((WE_MODID_SIS << 8) + 0x05)

/*!
 * \brief Response signal for SIS_getSlideInfo()
 * Data type: SisGetSlideInfoRsp
 * 
 *****************************************************************************/
#define SIS_GET_SLIDE_INFO_RSP  ((WE_MODID_SIS << 8) + 0x06)


/*--- Definitions/Declarations ---*/

/***************** CORE FUNCTIONS ===>>> *************************************/
/*!
 * \brief Starts the SIS API, must be called before any other calls.
 * 
 *****************************************************************************/
void SISif_startInterface(void);

/***************** <<<=== CORE FUNCTIONS *************************************/

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
                    WE_UINT32 userData);

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
                     WE_UINT32 userData);

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
                        WE_UINT32 userData);


/*! \brief Copies an SisRootLayout struct
 *
 * \param source Source data
 * \param dest Destination data
 *****************************************************************************/
void SISif_copySlsRootLayout(const SisRootLayout *source, SisRootLayout *dest);

/*! \brief Copies an SisSlideElement struct.
 *
 * \param module, the calling module.
 * \param source Source data.
 * \param dest Destination data.
 *****************************************************************************/
void SISif_copySlsSlideElement(WE_UINT8 module, const SisSlideElement *source, 
    SisSlideElement *dest);

/*! \brief Copies an SisSlideElementList list.
 *
 * \param module, the calling module.
 * \param source Source data.
 * \param dest Destination data.
 *****************************************************************************/
void SISif_copySlsSlideElementList(WE_UINT8 module, 
    const SisSlideElementList *source, SisSlideElementList *dest);

/*! \brief Copies an SisRegion struct
 *
 * \param source Source data
 * \param dest Destination data
 *****************************************************************************/
void SISif_copySlsRegion(const SisRegion *source, SisRegion *dest);

/*! \brief Copies an SisRegionList
 *
 * \param source Source data
 * \param dest Destination data
 *****************************************************************************/
void SISif_copySlsRegionList(WE_UINT8 module, const SisRegionList *source, 
    SisRegionList *dest);

/*! \brief Copies an SisRegion struct
 *
 * \param source Source data
 * \param dest Destination data
 *****************************************************************************/
void SISif_copySlideInfo(WE_UINT8 module, const SisSlideInfo *source, 
    SisSlideInfo *dest);

/*! \brief Copies an SisSlideInfoList list.
 *
 * \param module, the calling module.
 * \param source Source data.
 * \param dest Destination data.
 *****************************************************************************/
void SISif_copySlideInfoList(WE_UINT8 module, 
    const SisSlideInfoList *source, SisSlideInfoList *dest);

/*! \brief Copies an SisObjectInfo struct.
 *
 * \param module, the calling module.
 * \param source Source data.
 * \param dest Destination data.
 *****************************************************************************/
void SISif_copyObjectInfo(WE_UINT8 module, const SisObjectInfo *source, 
    SisObjectInfo *dest);

/*! \brief Copies an SisObjectInfoList list.
 *
 * \param module, the calling module.
 * \param source Source data.
 * \param dest Destination data.
 *****************************************************************************/
void SISif_copyObjectInfoList(WE_UINT8 module, 
    const SisObjectInfoList *source, SisObjectInfoList *dest);

/*!
 * \brief Deallocate SisUnrefItemList.
 *
 * \param module, the calling module.
 * \param list, list to deallocate.
 *****************************************************************************/
void SISif_freeSlsUnrefItemList(WE_UINT8 module, SisUnrefItemList *list);

/*!
 * \brief Deallocate SisRegionList.
 *
 * \param module, the calling module.
 * \param list, list to deallocate.
 *****************************************************************************/
void SISif_freeSlsRegionList(WE_UINT8 module, SisRegionList *list);

/*!
 * \brief Deallocate SisObjectInfo.
 *
 * \param module, the calling module.
 * \param obj, object to deallocate.
 *****************************************************************************/
void SISif_freeSlsObjectInfo(WE_UINT8 module, SisObjectInfo *obj);

/*!
 * \brief Deallocate SisObjectInfoList.
 *
 * \param module, the calling module.
 * \param list, list to deallocate.
 *****************************************************************************/
void SISif_freeSlsObjectInfoList(WE_UINT8 module, SisObjectInfoList *list);


/*!
 * \brief Deallocate SlsParamList.
 *
 * \param module, the calling module.
 * \param list, list to deallocate.
 *****************************************************************************/
void SISif_freeSlsParamList(WE_UINT8 module, SlsParamList *list);

/*!
 * \brief Deallocate SisSlideElement.
 *
 * \param module, the calling module.
 * \param slideElement, pointer to object to free.
 *****************************************************************************/
void SISif_freeSlsSlideElement(WE_UINT8 module, const SisSlideElement *slideElement);

/*!
 * \brief Deallocate SisSlideElementList.
 *
 * \param module, the calling module.
 * \param list, list to deallocate.
 *****************************************************************************/
void SISif_freeSlsSlideElementList(WE_UINT8 module, SisSlideElementList *list);

/*!
 * \brief Deallocate SisSlideInfoList.
 *
 * \param module, the calling module.
 * \param data, list to deallocate.
 *****************************************************************************/
void SISif_freeSlsSlideInfoList(WE_UINT8 module, SisSlideInfoList *list);

/*!
 * \brief Deallocate SisOpenSmilRsp.
 *
 * \param module, the calling module.
 * \param data, pointer to object to deallocate.
 *****************************************************************************/
void SISif_freeSisOpenSmil(WE_UINT8 module, SisOpenSmilRsp *data);

/*!
 * \brief Deallocate SisGetSlideInfoRsp
 *
 * \param module, the calling module.
 * \param data, pointer to object to deallocate.
 *****************************************************************************/
void SISif_freeSisGetSlideInfo(WE_UINT8 module, const SisGetSlideInfoRsp *data);

/*! \brief Retrieves the region for a specific region ID.
 *
 * \param regions
 * \param id The ID to find.
 * \return The region corresponding to the ID or NULL if the region does not
 *         exists
 *****************************************************************************/
SisRegion *SISif_getRegionFromId(SisRegionList *regions, WE_UINT32 id);

/*! \brief Retrieves the media object with a specific type
 *	
 * \param elemList
 * \return A reference to a media object or NULL if no object exists.
 *****************************************************************************/
SisSlideElement *SISif_getElementByType(SisSlideElementList *elemList,
    SlsMediaObjectType sType);


/***************** CREATE (Lib functions) ===>>> *****************************/

/*!
 * \brief Creates a new SMIL message
 * 
 * \param modId Module id of the calling module
 * \param rootLayout Root layout (window size) for the new message
 * \return Result code
 *****************************************************************************/
SisResult SISlib_createSmil(WE_UINT8 modId, 
                            SisCreateSmilHandle *handle,
                            const SisRootLayout *rootlayout);

/*!
 * \brief Creates a new region
 * 
 * \param modId Module id of the calling module
 * \param region Size and position of the region
 * \param name Name of the region
 * \return Result code
 *****************************************************************************/
SisResult SISlib_createRegion(WE_UINT8 modId, 
                              const SisCreateSmilHandle *handle,
                              const SisRegion *region,
                              const char *name);


/*!
 * \brief Cancels the creation of a SMIL message and frees all
 *  resources
 * 
 * \param modId Module id of the calling module 
 * \return Result code
 *****************************************************************************/
SisResult SISlib_cancelCreateSmil(WE_UINT8 modId,
                                  const SisCreateSmilHandle *handle);

/*!
 * \brief Creates a new slide to the SMIL message
 * 
 * \param modId Module id of the calling module 
 * \param duration Slide duration in seconds
 * \param text Text MO, NULL if not used
 * \param image Image MO, NULL if not used
 * \param video Video MO, NULL if not used
 * \param audio Audio MO, NULL if not used
 * \return Result code
 *****************************************************************************/
SisResult SISlib_createSlide(WE_UINT8  modId,
                             const SisCreateSmilHandle *handle,
                             WE_UINT32 duration,
                             const SisMediaObject *text,
                             const SisMediaObject *image,
                             const SisMediaObject *video,
                             const SisMediaObject *audio);  

/*!
 * \brief Generates a SMIL message and frees all used resources
 * The buffer is allocated in the function and the caller is responsible 
 * for deallocating the buffer.
 * 
 * \param modId Module id of the calling module 
 * \param buffer Pointer to the data buffer
 * \return Result code
 *****************************************************************************/
SisResult SISlib_generateSmil(WE_UINT8 modId,
                              const SisCreateSmilHandle *handle,
                              char **buffer);

/***************** <<<=== CREATE (Lib functions) *****************************/

/*!
 * \brief   Get the parameter value as string from a SlsParamList. 
 * 
 * \param   list, the list of parameters.
 * \param   name, the name of the paramter to retrieve value for.
 * \return  value of the parameter as constant string.
 *****************************************************************************/
const char * SISlib_getParamValueAsString(const SlsParamList * list, const char * name);

/*!
 * \brief    Gets the value of a defined color.
 * 
 * \param    list, the list of parameters.
 * \param    name, the name of the parameter to retrieve value for.
 * \return   the value of the color as a value.
 *****************************************************************************/
WE_UINT32 SISlib_getColorValue(const SlsParamList * list, const char * name);


/***************** EXT. SIGNAL FUNCTIONS ===>>> ******************************/
/*!
 * \brief Converts a serialized signal buffer into signal struct
 * The function is called from WE when signals is to be converted.
 * 
 * \param modId The calling module identity
 * \param signal Signal identity
 * \param buffer The signal buffer
 *****************************************************************************/
void *slsConvert(WE_UINT8 modId,
                 WE_UINT16 signal,
                 void *buffer);

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
void slsDestruct(WE_UINT8 modId,
                 WE_UINT16 signal,
                 void* p);

/***************** <<<=== EXT. SIGNAL FUNCTIONS ******************************/

#endif /* SIS_IF_H */

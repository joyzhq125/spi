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

#ifndef SATYPES_H
#define SATYPES_H


#ifndef SIS_IF_H
#error Sis_if.h must be included berfore Satypes.h!
#endif


/*--- Definitions/Declarations ---*/

/*--- Types ---*/

/*! \enum MeaGetBpRspDest
 *	\brief Tells which FSM to send the response from the MMSif_getBodyPart to.
 */
typedef enum
{
    SIA_GET_BP_RSP_DEST_CRH = 0x01, /*!< The MMV FSM is the originator */
    SIA_GET_BP_RSP_DEST_LOAD        /*!< The MR FSM is the originator */
}SiaGetBpRspDest;

/* ! \struct SiaCharsetItem
 * \brief Structure used for conversion between MMS and WE charset 
 * definitions.
 *
 */
typedef struct
{
    const WE_UINT32 mmsCharset;    /*!< The MMS charset definition */
    const WE_UINT32 msfCharset;    /*!< The WE charset definition */
}SiaCharsetItem;

/*! \struct SiaFit
 *  \brief Value of <fit> attribute, described in W3C SMIL 2.0
 */
typedef enum
{
    SIA_ATTR_FILL	  = 0,	/*!< Scale object to touch the edges of the box */
    SIA_ATTR_HIDDEN	  = 1,	/*!< See W3C SMIL 2.0   */
	SIA_ATTR_MEET	  = 2,	/*!< See W3C SMIL 2.0   */
	SIA_ATTR_SCROLL	  = 3,	/*!< See W3C SMIL 2.0   */
	SIA_ATTR_SLICE	  = 4,	/*!< See W3C SMIL 2.0   */
	SIA_ATTR_NOT_USED = 5	/*!< See W3C SMIL 2.0   */
}SiaFit;

typedef struct
{
    WE_UINT32                  handle;
    SisObjectInfo               obj;
}SiaObjectInfo;


typedef struct SiaObjectInfoListSt
{
    SiaObjectInfo               current;
    struct SiaObjectInfoListSt  *next;
}SiaObjectInfoList;


typedef enum
{
    SIA_MO_START,
    SIA_MO_STOP
}SiaMoAction;

/* \struct SiaMediaTypeItem
 * \brief Used for defining a information needed for a media object.
 *
 */
typedef struct 
{
    const char          *mimeType;      /*!< The mime-type */    
    
    SlsMediaObjectType  slsObjectType;  /*!< The media type, see 
                                        #SlsMediaObjectType. -1 for last item */
}SiaMediaTypeItem;

/* \struct SiaMediaTypeItem
 * \brief Used for defining a information needed for a media object.
 *
 */
typedef struct 
{
    const SisMediaType  slsMediaType;   /*!< The media type, see 
                                                     #SisMediaType */
    const char          *mimeType;      /*!< The mime-type */
    const char          *ending;        /*!< The file ending */
}SiaAttachmentTypeItem;


/*! \enum SiaCharset Supported character sets within the smil module. */
typedef enum
{
    SIA_US_ASCII    = 3,   
    SIA_UTF8        = 106,
    SIA_USC2        = 1000,
    SIA_UTF16BE     = 1013, 
    SIA_UTF16LE     = 1014, 
    SIA_UTF16       = 1015, 
    SIA_ISO_8859_1  = 4,
    SIA_UNKNOWN_CHARSET = 0
} SiaCharset;





/*****************************************************************************/

/*!\struct SiaLinkItem
 * \brief A data-structure for creating a list of links.
 */
typedef struct SiaLinkItemSt
{
    char *url;                  /*!< The link */
    struct SiaLinkItemSt *next; /*!< The next item or NULL */
}SiaLinkItem;


/*********************************************
 * Exported function
 *********************************************/

#endif /* SATYPES_H */

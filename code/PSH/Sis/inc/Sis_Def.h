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
/*! \file Sis_def.h
 * \brief  Definitions for SIS
 */

#ifndef SIS_DEF_H
#define SIS_DEF_H

/*--- Include files ---*/

/*--- Definitions/Declarations ---*/

#define SIS_HASH_MARK       '#'
/*--- Types ---*/

/* ! \struct SlsCharsetItem
 * \brief Structure used for conversion between MMS and WE charset 
 * definitions.
 *
 */
typedef struct
{
    const WE_UINT32 mmsCharset;    /*!< The MMS charset definition */
    const WE_UINT32 msfCharset;    /*!< The WE charset definition */
}SlsCharsetItem;

/*--- Constants ---*/

/*! Object types for the SIS
 *
 */
typedef enum
{
    SIS_OBJECT_TYPE_UNKNOWN,    /*!< Unknown object  */
    SIS_OBJECT_TYPE_TEXT,       /*!< Text object  */
    SIS_OBJECT_TYPE_IMAGE,      /*!< Image object */
    SIS_OBJECT_TYPE_AUDIO,      /*!< Audio object */
    SIS_OBJECT_TYPE_VIDEO,      /*!< Video object */
    SIS_OBJECT_TYPE_REF,
	SIS_OBJECT_TYPE_ATTACMENT,  /*!< Attatcment object */
	SIS_OBJECT_TYPE_DRM         /*!< Used for DRM content */      
}SlsMediaObjectType;

/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Global variables ---*/

/*--- Static variables ---*/

/*--- Prototypes ---*/
/******************************************************************************/

#endif /* SIS_DEF_H */

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

/* !\file mautils.h
 *  \brief Utility functions for MSA.
 */

#ifndef _MAUTILS_H_
#define _MAUTILS_H_

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! The maximum length of a date string */
#define MSA_DATESTRING_LENGTH       29

/*! Swaps the values of to integers */
#define MSA_SWAP(i, j)              (i ^= j, j ^= i, i ^= j)

/*! \struct MsaMediaObjects
 *  
 */
typedef struct
{
    const MsaMediaGroupType objectType;     /*!< The type of object see 
                                                 #MsaMediaGroupType */
    const char              *mimeType;      /*!< Mime type */
    const SisMediaType      sisMediaType;   /*!< The media types as defined by 
                                                 SIS  */
    const char              *fileExtension; /*!< The file extension of the 
                                                 media type  */
}MsaMediaObjects;

/******************************************************************************
 * Prototypes 
 *****************************************************************************/

char *msaCreateFileName(const char *extension);
char *msaCreateFileNameFromType(const char * mimeType);
void msaFreePropItem(MsaPropItem *prop, WE_BOOL dynamicOnly);
WE_UINT32 msaIsValidCharset(MmsCharset charset);
WE_BOOL msaConvertToUTF8(int charset, WE_UINT32 *size, unsigned char **text);
MsaMediaGroupType msaMimeToObjGroup(const char *mimeType);
SisMediaType msaMimeToSis(const char *mimeType);
char *msaSisToMime(SisMediaType sisMediaType);
void msaRemPathFromFileName(char *fileName);
void msaFreeMmsContentTypeParams(MmsAllParams *params);
void msaRemoveMmsParam(MmsAllParams **params, MmsParam paramToRemove);
void msaRemPathFromFileName(char *fileName);
const char *msaGetTextMimeType(void);

#endif


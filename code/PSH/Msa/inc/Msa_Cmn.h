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

/* !\file Msa_Cmn.h
 *  \brief Common data-types and constants used internally by the MTR.
 */

#ifndef MSA_CMN_H
#define MSA_CMN_H

#include "Msa.h"

/******************************************************************************
 * Constants
 *****************************************************************************/

#define WE_MODID_MTR WE_MODID_MSA

#define MTR_ALLOC MSA_ALLOC

#define MTR_FREE MSA_FREE

/*!
 *  NOTE! Since regular expressions are not supported there is special
 *  handling of dynamic schemes.
 */
#define MTR_SUPPORTED_SCHEMES  \
    {{MTR_SCHEME_HTTP, "http://", sizeof("http://") - 1},\
     {MTR_SCHEME_HTTPS, "https://", sizeof("https://") - 1},\
     {MTR_SCHEME_WWW, "www.", sizeof("www.") - 1},\
     {MTR_SCHEME_MAIL, NULL, 0},\
     {MTR_SCHEME_PHONE, NULL, 0},\
     {MTR_SCHEME_WAP, "wap.", sizeof("wap.") - 1},\
     {MTR_SCHEME_NONE, NULL, 0}\
    }

#define IS_SET(flags, flag)   (((flags) & (flag)) == (flag))

/******************************************************************************
 * Data-types
 *****************************************************************************/


/*! \struct MtrParseResult
 */
typedef struct _MtrParseList  
{
    int index;
    MtrScheme scheme;
    char *link;
    int utf8Len;
    WePosition pos;
    WE_BOOL isSelected;
    struct _MtrParseList *next;
    struct _MtrParseList *prev;
}MtrParseResult;

/*! \struct MtrIndexTableEntry
 */
typedef struct
{
    WE_UINT16 utf8Index;
    WE_UINT16 width;
}MtrIndexTableEntry;

/*! \struct MtrInstance
 *  \brief  A data structure for keeping instance data.
 */
typedef struct 
{
    int instance;                   /*!< The instance number for the text box */
    WeGadgetHandle gadgetHandle;   /*!< The handle to the gadget which holds the rendered image */
    WeStringHandle stringHandle;   /*!< The handle to the string to render */
    MtrCreateTextBoxProperties prop;/*!< The create properties from user */

    WeSize boxSize;                /*!< Maximum size to draw in */
    /* Result data */
    MtrParseResult *parse;          /*!< The result from the parsing */
    WeSize actualSize;             /*!< The size of the image after rendering */
    /* State data */
    MtrParseResult *selectedLink;   /*!< Current selected link */
    WeEventType currentEvent;      /*!< The event which caused a redraw */
    WE_BOOL fullScreen;            /*!< Set to TRUE if entire text fits on screen */
    WE_BOOL lostFocus;             /*!< TRUE if the text box should lose focus */
    WE_BOOL redraw;                /*!< TRUE if the rendering is a redraw */
    WE_INT16 rowHeight;            /*!< The height of one row in pixels */
    int totalLength;                /*!< Total number of UTF8-characters in string */
    char *charBuffer;               /*!< A character buffer representation of the widget string */
    MtrIndexTableEntry *indexTable; /*!< The index table */
    int currentIndex;               /*!< Current index in the index table */
    int rowCount;                   /*!< Number of used rows in the index table */
} MtrInstance;

void Msa_InitRowIndices(MtrInstance *pstInst);
void Msa_AddRowIndex(MtrInstance *pstInst, const WE_UINT16 uiIndex, 
    const WE_UINT16 uiWidth);
void Msa_FreeRowIndices(MtrInstance *pstInst);
MtrIndexTableEntry *Msa_GetPrevIndex(const MtrInstance *psInst, int ICurrArrayIndex);
MtrIndexTableEntry *Msa_GetNextIndex(const MtrInstance *psInst, int ICurrArrayIndex);

void Msa_FreeLinks(MtrLinkEntry **ppstData);
void Msa_InsertParseEntry(int iCharPos, const char *pcStr, int iUtf8Len, 
    MtrScheme eScheme, MtrParseResult **ppstParse);
char *Msa_GetStringBufferFromHandle(WeStringHandle uiStrHandle);
char *Msa_StrDup(const char *pcStr);
void Msa_FreeParseResult(MtrParseResult **ppstData);
WE_BOOL Msa_CopyLinkList(const MtrParseResult *pstParse, MtrLinkEntry **ppstLinks);
#endif


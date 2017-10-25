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

/*! \file Msa_Cmn.c
 *  This file contains the control logic for rendering/viewing a text.
 *  PC-LINT OK 2004-02-17 ALYR
 */

/* WE */
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Wid.h"
#include "We_Mem.h"
#include "We_Cmmn.h"

/* MSA */
#include "Msa_Mem.h"

/* MTR */
#include "Msa.h"
#include "Msa_Cmn.h"

/******************************************************************************
 * Constants
 *****************************************************************************/

/******************************************************************************
 * Data-structures
 *****************************************************************************/

/******************************************************************************
 * Static variables
 *****************************************************************************/

/******************************************************************************
 * Prototypes
 *****************************************************************************/

/******************************************************************************
 * Function implementations
 *****************************************************************************/

/*!
 * \brief Duplicate a string
 *
 * \param str The string to duplicate
 * \return The newly allocated string if successful, otherwise NULL.
 *****************************************************************************/
char *Msa_StrDup(const char *pcStr)
{
    return we_cmmn_strdup(WE_MODID_MTR, pcStr);
}

/*!
 * \brief Gets a character buffer from a string handle
 *
 * \param strHandle The string handle to get buffer from
 * \return The newly allocated string if successful, otherwise NULL.
 *****************************************************************************/
char *Msa_GetStringBufferFromHandle(WeStringHandle uiStrHandle)
{
    char *pcTmpStr;
    int iLength;
    
    if (0 >= (iLength = WE_WIDGET_STRING_GET_LENGTH(uiStrHandle, TRUE, 
        WeUtf8)))
    {
        return NULL;
    }
    /* Add space for NULL termination*/
    pcTmpStr = (char *)MTR_ALLOC(sizeof(char) * 
                                                            (unsigned)iLength);
    memset(pcTmpStr, 0, (unsigned int)iLength);
    /* Get string data */
    if (0 > WE_WIDGET_STRING_GET_DATA(uiStrHandle, pcTmpStr, WeUtf8))
    {
        /*lint -e{774} */
        MTR_FREE(pcTmpStr);
        pcTmpStr = NULL;
        return NULL;    
    }
    return pcTmpStr;
    
}

/*!
 * \brief Insert entry in double linked parse list
 *
 * \param charPos Current position in string
 *****************************************************************************/
void Msa_InsertParseEntry
(
    int iCharPos,
    const char *pcStr,
    int iUtf8Len, 
    MtrScheme eScheme,
    MtrParseResult **ppstParse
)
{
    MtrParseResult *pstEntry;
    pstEntry = WE_MEM_ALLOCTYPE(WE_MODID_MTR, MtrParseResult);
    memset(pstEntry, 0, sizeof(MtrParseResult));
    pstEntry->scheme = eScheme;
    pstEntry->link = Msa_StrDup(pcStr);
    pstEntry->utf8Len = iUtf8Len;
    pstEntry->index = iCharPos;
    pstEntry->pos.x = pstEntry->pos.y = 0;
    pstEntry->next = *ppstParse;
    pstEntry->prev = NULL;
    if (NULL != *ppstParse) 
    {
        (*ppstParse)->prev = pstEntry;        
    }
    *ppstParse = pstEntry;
}

/*!
 * \brief 
 *
 * \param data Data to free
 *****************************************************************************/
void Msa_FreeLinks(MtrLinkEntry **ppstData)
{
    MtrLinkEntry *pstEntry;
    while (*ppstData) 
    {
        if ((*ppstData)->link) 
        {
            WE_MEM_FREE(WE_MODID_MTR, (*ppstData)->link);
            (*ppstData)->link = NULL;
        }
        pstEntry = (*ppstData);
        (*ppstData) = (*ppstData)->next;
        pstEntry->next = NULL;
        WE_MEM_FREE(WE_MODID_MTR, pstEntry);
    }
    
}

/*!
 * \brief 
 *
 * \param inst Current instance
 * \return TRUE if succesful, otherwise FALSE.
 *****************************************************************************/
void Msa_FreeParseResult(MtrParseResult **ppstData)
{
    MtrParseResult *pstResult;
    while (*ppstData) 
    {
        if ((*ppstData)->link) 
        {
            MTR_FREE((*ppstData)->link);
            (*ppstData)->link = NULL;
        }
        pstResult = (*ppstData);
        (*ppstData) = (*ppstData)->next;
        pstResult->next = NULL;
        MTR_FREE(pstResult);
    }
    
}

/*!
 * \brief Allocates a new link.
 *
 * \param p The parse result
 * \param l (out) The new list
 * \return TRUE if succesful, otherwise FALSE.
 *****************************************************************************/
/*lint -e{621}*/
static void Msa_InsertLink
(
    const MtrParseResult *pstResult,
    MtrLinkEntry **ppstEntry
)
{
    MtrLinkEntry *pstTemEntry;
    pstTemEntry = WE_MEM_ALLOCTYPE(WE_MODID_MTR, MtrLinkEntry);
    memset(pstTemEntry, 0, sizeof(MtrLinkEntry));
    pstTemEntry->link = Msa_StrDup(pstResult->link);
    pstTemEntry->scheme = pstResult->scheme;
    pstTemEntry->next = *ppstEntry;
    *ppstEntry = pstTemEntry;
}

/*!
 * \brief Copies links from the parse result to a new list.
 *
 * \param parse The parse result
 * \param links (out) The new list
 * \return TRUE if succesful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL Msa_CopyLinkList
(
    const MtrParseResult *pstParse,
    MtrLinkEntry **ppstLinks
)
{
    MtrParseResult *pResult;
    if (!pstParse || !ppstLinks) 
    {
        return FALSE;
    }
    /* Since the list is backwards and we build a stack the new list
     * will be the right order */
    /*lint -e{605}*/
    pResult = (MtrParseResult*)pstParse;
    /* Start copy */
    while (pResult) 
    {
        Msa_InsertLink((const MtrParseResult*)pResult, ppstLinks);
        pResult = pResult->next;
    }
    return TRUE;
}


void Msa_InitRowIndices(MtrInstance *pstInst)
{
#define MTR_MAX_ROWS 100
    pstInst->rowCount = 0;
    pstInst->indexTable = (MtrIndexTableEntry*)
        MTR_ALLOC(sizeof(MtrIndexTableEntry) * MTR_MAX_ROWS);
    memset(pstInst->indexTable, 0, sizeof(MtrIndexTableEntry) * MTR_MAX_ROWS);
}

void Msa_AddRowIndex
(
    MtrInstance *pstInst,
    const WE_UINT16 uiIndex, 
    const WE_UINT16 uiWidth
)
{
    MtrIndexTableEntry *pstTmp;
    if (pstInst->rowCount && (0 == (pstInst->rowCount % MTR_MAX_ROWS))) 
    {
        /* Not enough rows. Gimme more... */
        pstTmp = (MtrIndexTableEntry *)MTR_ALLOC(sizeof(MtrIndexTableEntry) * 
            (WE_UINT32)(MTR_MAX_ROWS + pstInst->rowCount));
        memcpy(pstTmp, pstInst->indexTable, 
            sizeof(MtrIndexTableEntry) * (WE_UINT32)pstInst->rowCount);
        memset(pstTmp + pstInst->rowCount, 0, 
            sizeof(MtrIndexTableEntry) * MTR_MAX_ROWS);
        MTR_FREE(pstInst->indexTable);
        pstInst->indexTable = pstTmp;
    }
    pstInst->indexTable[pstInst->rowCount].utf8Index = uiIndex;
    pstInst->indexTable[pstInst->rowCount++].width = uiWidth;
}

void Msa_FreeRowIndices(MtrInstance *pstInst)
{
    MTR_FREE(pstInst->indexTable);
    pstInst->indexTable = NULL;
    pstInst->rowCount = 0;
}

MtrIndexTableEntry *Msa_GetPrevIndex
(
    const MtrInstance *pstInst,
    int iCurrArrayIndex
)
{
    if (0 < iCurrArrayIndex && iCurrArrayIndex < (pstInst->rowCount - 1)) 
    {
        return &pstInst->indexTable[iCurrArrayIndex - 1];
    }
    return &pstInst->indexTable[iCurrArrayIndex];
}

MtrIndexTableEntry *Msa_GetNextIndex(const MtrInstance *pstInst, int iCurrArrayIndex)
{
    if (0 < iCurrArrayIndex  && iCurrArrayIndex < (pstInst->rowCount - 1)) 
    {
        return &pstInst->indexTable[iCurrArrayIndex + 1];
    }
    return &pstInst->indexTable[iCurrArrayIndex];
}

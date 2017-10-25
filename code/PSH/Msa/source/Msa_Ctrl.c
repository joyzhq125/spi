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

/*! \file mtr_ctrl.c
 *  This file contains the control logic for rendering/viewing a text.
 *  PC-LINT OK 2004-02-17 ALYR
 */

/* WE */
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Wid.h"
#include "We_Mem.h"

/* MSA */
#include "Msa_Mem.h"

/* MTR */
#include "Msa.h"
#include "Msa_Cmn.h"
#include "Msa_Par.h"
#include "Msa_Rend.h"
#include "Msa_Ctrl.h"

/******************************************************************************
 * Constants
 *****************************************************************************/

 /******************************************************************************
 * Data-structures
 *****************************************************************************/

/******************************************************************************
 * Static variables
 *****************************************************************************/
/*lint -e{621}*/
static MtrInstance *mtrTextBoxes[MTR_MAX_INSTANCES];

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static MtrInstance *createInstance(WeStringHandle stringHandle,
    const MtrCreateTextBoxProperties *prop);
                                   
static void deleteInstance(MtrInstance **inst);

/******************************************************************************
 * Function implementations
 *****************************************************************************/

/*!
 * \brief Initializes the MTR component
 *****************************************************************************/
void mtrCtrlInit(void)
{
    memset(mtrTextBoxes, 0, sizeof(MtrInstance*) * MTR_MAX_INSTANCES);
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MTR, 
        "MTR CTRL: initialized\n"));
}

/*!
 * \brief Terminates the MTR component
 *****************************************************************************/
void mtrCtrlTerminate(void)
{
    /* Delete all instances */
    deleteInstance(NULL);
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MTR, "MTR CTRL: terminated\n"));

}

/*!
 * \brief Creates a new text box instance.
 *
 * \param stringHandle The string handle of the string to show.
 * \param prop The user defined properties of the text box.
 * \return The new instance or NULL if no new instance could be created.
 *****************************************************************************/
static MtrInstance *createInstance(WeStringHandle stringHandle,
                                   const MtrCreateTextBoxProperties *prop)
{
    int i;
    /* Find an empty slot. */
    for(i = 0; i < MTR_MAX_INSTANCES; i++)
    {
        if (NULL == mtrTextBoxes[i])
        {
            mtrTextBoxes[i] = WE_MEM_ALLOCTYPE(WE_MODID_MTR, MtrInstance);
            memset(mtrTextBoxes[i], 0, sizeof(MtrInstance));
            mtrTextBoxes[i]->instance = i;
            memcpy(&mtrTextBoxes[i]->boxSize, &prop->size, sizeof(WeSize));
            mtrTextBoxes[i]->stringHandle = stringHandle;
            memcpy(&(mtrTextBoxes[i]->prop), prop, 
                sizeof(MtrCreateTextBoxProperties));
            Msa_InitRowIndices(mtrTextBoxes[i]);
            return mtrTextBoxes[i];
        }
    }
    return NULL;
}

/*!
 * \brief Deallocates a #MtrInstance struct.
 * 
 * \param inst The instance to free. If inst is NULL then all instances will
 * be removed.
 *****************************************************************************/
static void deleteInstance(MtrInstance **inst)
{
    int i;
    if (inst) 
    {
        if (*inst)
        {
            if ((*inst)->gadgetHandle > 0) 
            {
                (void)WE_WIDGET_RELEASE((*inst)->gadgetHandle);
            }
            Msa_FreeParseResult(&((*inst)->parse));
            Msa_FreeRowIndices(*inst);
            if ((*inst)->charBuffer) 
            {
                MTR_FREE((*inst)->charBuffer);
            }
            MTR_FREE(*inst);
            *inst = NULL;
        }
    }
    else
    {
        /* Remove all instances */
        for(i = 0; i < MTR_MAX_INSTANCES; i++)
        {
            deleteInstance(&mtrTextBoxes[i]);
        }        
    }
}

/*!
 * \brief Create a text box
 * 
 * \param stringHandle Handle of the text string to display in the text box. 
 * \param prop Properties of the text box. See #MtrCreateTextBoxProperties.
 * \return MTR_RESULT_OK on success, otherwise the proper error code.
 *****************************************************************************/
MtrResult mtrCreateTextBox(WeStringHandle stringHandle, 
    const MtrCreateTextBoxProperties *prop, MtrCreateRspData **rsp)
{
    MtrInstance *inst;
    WE_UINT32 flags;
    *rsp = NULL;

    /* We don't accept empty strings */
    if (WE_WIDGET_STRING_GET_LENGTH(stringHandle, 0, WeUtf8) <= 0) 
    {
        *rsp = NULL;
        return MTR_RESULT_ERROR;
    }
    /* Create a new text box instance */
    if (NULL == (inst = createInstance(stringHandle, prop)))
    {
        return MTR_RESULT_BUSY;
    }
    /* Save flags */
    flags = prop->flags;
    /* Only parse if user wants to */
    if (IS_SET(flags, MTR_PROP_RENDER_LINKS) ||
        IS_SET(flags, MTR_PROP_RETURN_LINKS)) 
    {
        if (mtrParse(inst) < 0) 
        {
            deleteInstance(&inst);
            return MTR_RESULT_ERROR;
        }
    }
    /* Render screen */
    if (mtrRender(inst) < 0) 
    {
        deleteInstance(&inst);
        return MTR_RESULT_ERROR;        
    }
    /* Create response data */
    *rsp = WE_MEM_ALLOCTYPE(WE_MODID_MTR, MtrCreateRspData);
    memset(*rsp, 0, sizeof(MtrCreateRspData));
    (*rsp)->instance = inst->instance;
    (*rsp)->gadgetHandle = inst->gadgetHandle;
    memcpy(&(*rsp)->gadgetSize, &inst->actualSize, sizeof(WeSize));
    if (IS_SET(flags, MTR_PROP_RETURN_LINKS))
    {
        if (!Msa_CopyLinkList(inst->parse, &(*rsp)->links))
        {
            return MTR_RESULT_ERROR;
        }
    }
    return MTR_RESULT_OK;
}

/*!
 * \brief Deletes a text box.
 * 
 * \param callerFsm Caller FSM
 * \param returnSignal The signal to return when done.
 * \param instance The text box instance.
 *****************************************************************************/
MtrResult mtrDeleteTextBox(int instance)
{
    if (instance < 0 || instance >= MTR_MAX_INSTANCES) 
    {
        return MTR_RESULT_ERROR;
    }
    deleteInstance(&mtrTextBoxes[instance]);
    return MTR_RESULT_OK;
}

/*!
 * \brief Checks if any link is active (is selected) in current text box.
 *        The function will allocate data for the MtrLinkEntry link if
 *        the functions returns TRUE, otherwise no memory will be allocated
 *        and link will be set to NULL.
 * \param instance The text box instance.
 * \param link The returned link if active, otherwise NULL.
 * \return TRUE if a link is active, otherwise FALSE.
 *****************************************************************************/
WE_BOOL mtrHasActiveLink(int instance, MtrLinkEntry **link)
{
    MtrInstance *inst = NULL;

    if (NULL == link) 
    {
        return FALSE;
    }
    *link = NULL;
    if (instance < 0 || instance >= MTR_MAX_INSTANCES) 
    {
        return FALSE;
    }
    inst = mtrTextBoxes[instance];
    if (NULL != inst->selectedLink) 
    {
        *link = (MtrLinkEntry*)MTR_ALLOC(sizeof(MtrLinkEntry));
        (*link)->link = Msa_StrDup(inst->selectedLink->link);
        (*link)->scheme = inst->selectedLink->scheme;
        (*link)->next = NULL;
        return TRUE;
    }
    return FALSE;
}

/*!
 * \brief Deallocates a MtrCreateRspData struct.
 * 
 * \param data The data to free.
 *****************************************************************************/
void mtrFreeMtrCreateRspData(MtrCreateRspData **data)
{
    Msa_FreeLinks(&((*data)->links));
    WE_MEM_FREE(WE_MODID_MTR, *data);
    *data = NULL;
}

/*!
 * \brief Deallocates a #MtrLinkEntry struct.
 * 
 * \param data The data to free.
 *****************************************************************************/
void mtrFreeMtrLinkEntry(MtrLinkEntry **data)
{
    Msa_FreeLinks(data);
}


/*!
 * \brief Redraws the text box according to current event.
 * 
 * \param instance The text box instance.
 * \param event The event that causes the text box to redraw.
 * \param lostFocus (out) This parameter will be set to TRUE if the text box
 *                  will loose focus due to the event.
*****************************************************************************/
MtrResult mtrRedraw(int instance, WeEventType event, WE_BOOL *lostFocus)
{
    MtrInstance *inst;
    if (instance < 0 || instance >= MTR_MAX_INSTANCES) 
    {
        return MTR_RESULT_ERROR;
    }
    if (NULL == (inst = mtrTextBoxes[instance]))
    {
        return MTR_RESULT_ERROR;
    }
    inst->currentEvent = event;
    inst->redraw = TRUE;
    if (FALSE == mtrRender(inst))
    {
        return MTR_RESULT_ERROR;
    }
    *lostFocus = inst->lostFocus;
    return MTR_RESULT_OK;
}


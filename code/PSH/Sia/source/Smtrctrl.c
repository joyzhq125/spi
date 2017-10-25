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

/*! \file smtr_ctrl.c
 *  This file contains the control logic for rendering/viewing a text.
 *  PC-LINT OK 2004-02-17 ALYR
 */

/* WE */
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Wid.h"
#include "We_Mem.h"

/* SMA */
#include "Samem.h"

/* SMTR */
#include "Smtr.h"
#include "Smtrcmn.h"
#include "Smtrpar.h"
#include "Smtrrend.h"
#include "Smtrctrl.h"

/* MEA */
#include "Msa_Ctrl.h"

/******************************************************************************
 * Constants
 *****************************************************************************/
/*!< Defines the maximum number of concurrent text boxes */
#define SMTR_MAX_INSTANCES MTR_MAX_INSTANCES 

/******************************************************************************
 * Data-structures
 *****************************************************************************/

/******************************************************************************
 * Static variables
 *****************************************************************************/
/*lint -e{621}*/
static SmtrInstance *smtrTextBoxes[SMTR_MAX_INSTANCES];

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static SmtrInstance *createInstance(WeStringHandle stringHandle,
    const SmtrCreateTextBoxProperties *prop);
                                   
static void deleteInstance(SmtrInstance **inst);

/******************************************************************************
 * Function implementations
 *****************************************************************************/

/*!
 * \brief Initializes the SMTR component
 *****************************************************************************/
void smtrCtrlInit(void)
{
    memset(smtrTextBoxes, 0, sizeof(SmtrInstance*) * SMTR_MAX_INSTANCES);
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SMTR, 
        "SMTR CTRL: initialized\n"));
}

/*!
 * \brief Terminates the SMTR component
 *****************************************************************************/
void smtrCtrlTerminate(void)
{
    /* Delete all instances */
    deleteInstance(NULL);
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SMTR, "SMTR CTRL: terminated\n"));

}

/*!
 * \brief Creates a new text box instance.
 *
 * \param stringHandle The string handle of the string to show.
 * \param prop The user defined properties of the text box.
 * \return The new instance or NULL if no new instance could be created.
 *****************************************************************************/
static SmtrInstance *createInstance(WeStringHandle stringHandle,
                                   const SmtrCreateTextBoxProperties *prop)
{
    int i;
    /* Find an empty slot. */
    for(i = 0; i < SMTR_MAX_INSTANCES; i++)
    {
        if (NULL == smtrTextBoxes[i])
        {
            smtrTextBoxes[i] = WE_MEM_ALLOCTYPE(WE_MODID_SMTR, SmtrInstance);
            memset(smtrTextBoxes[i], 0, sizeof(SmtrInstance));
            smtrTextBoxes[i]->instance = i;
            memcpy(&smtrTextBoxes[i]->boxSize, &prop->size, sizeof(WeSize));
            smtrTextBoxes[i]->stringHandle = stringHandle;
            memcpy(&(smtrTextBoxes[i]->prop), prop, 
                sizeof(SmtrCreateTextBoxProperties));
            smtrInitRowIndices(smtrTextBoxes[i]);
            return smtrTextBoxes[i];
        }
    }
    return NULL;
}

/*!
 * \brief Deallocates a #SmtrInstance struct.
 * 
 * \param inst The instance to free. If inst is NULL then all instances will
 * be removed.
 *****************************************************************************/
static void deleteInstance(SmtrInstance **inst)
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
            smtrFreeParseResult(&((*inst)->parse));
            smtrFreeRowIndices(*inst);
            if ((*inst)->charBuffer) 
            {
                SMTR_FREE((*inst)->charBuffer);
            }
            SMTR_FREE(*inst);
            *inst = NULL;
        }
    }
    else
    {
        /* Remove all instances */
        for(i = 0; i < SMTR_MAX_INSTANCES; i++)
        {
            deleteInstance(&smtrTextBoxes[i]);
        }        
    }
}

/*!
 * \brief Create a text box
 * 
 * \param stringHandle Handle of the text string to display in the text box. 
 * \param prop Properties of the text box. See #SmtrCreateTextBoxProperties.
 * \return SMTR_RESULT_OK on success, otherwise the proper error code.
 *****************************************************************************/
SmtrResult smtrCreateTextBox(WeStringHandle stringHandle, 
    const SmtrCreateTextBoxProperties *prop, SmtrCreateRspData **rsp)
{
    SmtrInstance *inst;
    WE_UINT32 flags;
    *rsp = NULL;

    /* We don't accept empty strings */
    if (WE_WIDGET_STRING_GET_LENGTH(stringHandle, 0, WeUtf8) <= 0) 
    {
        *rsp = NULL;
        return SMTR_RESULT_ERROR;
    }
    /* Create a new text box instance */
    if (NULL == (inst = createInstance(stringHandle, prop)))
    {
        return SMTR_RESULT_BUSY;
    }
    /* Save flags */
    flags = prop->flags;
    /* Only parse if user wants to */
    if (IS_SET(flags, SMTR_PROP_RENDER_LINKS) ||
        IS_SET(flags, SMTR_PROP_RETURN_LINKS)) 
    {
        if (smtrParse(inst) < 0) 
        {
            deleteInstance(&inst);
            return SMTR_RESULT_ERROR;
        }
    }
    /* Render screen */
    if (smtrRender(inst) < 0) 
    {
        deleteInstance(&inst);
        return SMTR_RESULT_ERROR;        
    }
    /* Create response data */
    *rsp = WE_MEM_ALLOCTYPE(WE_MODID_SMTR, SmtrCreateRspData);
    memset(*rsp, 0, sizeof(SmtrCreateRspData));
    (*rsp)->instance = inst->instance;
    (*rsp)->gadgetHandle = inst->gadgetHandle;
    memcpy(&(*rsp)->gadgetSize, &inst->actualSize, sizeof(WeSize));
    if (IS_SET(flags, SMTR_PROP_RETURN_LINKS))
    {
        if (!smtrCopyLinkList(inst->parse, &(*rsp)->links))
        {
            return SMTR_RESULT_ERROR;
        }
    }
    return SMTR_RESULT_OK;
}

/*!
 * \brief Deletes a text box.
 * 
 * \param callerFsm Caller FSM
 * \param returnSignal The signal to return when done.
 * \param instance The text box instance.
 *****************************************************************************/
SmtrResult smtrDeleteTextBox(int instance)
{
    if (instance < 0 || instance >= SMTR_MAX_INSTANCES) 
    {
        return SMTR_RESULT_ERROR;
    }
    deleteInstance(&smtrTextBoxes[instance]);
    return SMTR_RESULT_OK;
}

/*!
 * \brief Checks if any link is active (is selected) in current text box.
 *        The function will allocate data for the SmtrLinkEntry link if
 *        the functions returns TRUE, otherwise no memory will be allocated
 *        and link will be set to NULL.
 * \param instance The text box instance.
 * \param link The returned link if active, otherwise NULL.
 * \return TRUE if a link is active, otherwise FALSE.
 *****************************************************************************/
WE_BOOL smtrHasActiveLink(int instance, SmtrLinkEntry **link)
{
    SmtrInstance *inst = NULL;

    if (NULL == link) 
    {
        return FALSE;
    }
    *link = NULL;
    if (instance < 0 || instance >= SMTR_MAX_INSTANCES) 
    {
        return FALSE;
    }
    inst = smtrTextBoxes[instance];
    if (NULL != inst->selectedLink) 
    {
        *link = (SmtrLinkEntry*)SMTR_ALLOC(sizeof(SmtrLinkEntry));
        (*link)->link = smtrStrDup(inst->selectedLink->link);
        (*link)->scheme = inst->selectedLink->scheme;
        (*link)->next = NULL;
        return TRUE;
    }
    return FALSE;
}

/*!
 * \brief Deallocates a SmtrCreateRspData struct.
 * 
 * \param data The data to free.
 *****************************************************************************/
void smtrFreeSmtrCreateRspData(SmtrCreateRspData **data)
{
    smtrFreeLinks(&((*data)->links));
    WE_MEM_FREE(WE_MODID_SMTR, *data);
    *data = NULL;
}

/*!
 * \brief Deallocates a #SmtrLinkEntry struct.
 * 
 * \param data The data to free.
 *****************************************************************************/
void smtrFreeSmtrLinkEntry(SmtrLinkEntry **data)
{
    smtrFreeLinks(data);
}


/*!
 * \brief Redraws the text box according to current event.
 * 
 * \param instance The text box instance.
 * \param event The event that causes the text box to redraw.
 * \param lostFocus (out) This parameter will be set to TRUE if the text box
 *                  will loose focus due to the event.
*****************************************************************************/
SmtrResult smtrRedraw(int instance, WeEventType event, WE_BOOL *lostFocus)
{
    SmtrInstance *inst;
    if (instance < 0 || instance >= SMTR_MAX_INSTANCES) 
    {
        return SMTR_RESULT_ERROR;
    }
    if (NULL == (inst = smtrTextBoxes[instance]))
    {
        return SMTR_RESULT_ERROR;
    }
    inst->currentEvent = event;
    inst->redraw = TRUE;
    if (FALSE == smtrRender(inst))
    {
        return SMTR_RESULT_ERROR;
    }
    *lostFocus = inst->lostFocus;
    return SMTR_RESULT_OK;
}


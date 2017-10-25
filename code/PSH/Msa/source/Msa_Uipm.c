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

/*!\file mauipm.c
 * \brief Progress meter.
 */

/* WE */
#include "We_Lib.h"
#include "We_Wid.h"
#include "We_Int.h"
#include "We_Core.h"
#include "We_Def.h"

/* MMS */
#include "Mms_Def.h"

/* MSA */
#include "Msa_Def.h"
#include "Msa_Rc.h"
#include "Msa_Sig.h"
#include "Msa_Intsig.h"
#include "Msa_Types.h"
#include "Msa_Uisig.h"
#include "Msa_Intsig.h"
#include "Msa_Uicmn.h"
#include "Msa_Uipm.h"

/******************************************************************************
 * Constants
 *****************************************************************************/

/* The height of the progress meter in per cent of the display height */
#define MSA_BAR_SIZE_IN_PER_CENT        10
/* The time between each tick for the progress meter */
#define MSA_PM_TICK_TIME                2000  /* Q04A */
/* The number of steps for the progress meter gadget*/
#define MSA_PM_TICKS                    25

/******************************************************************************
 * Data-types
 *****************************************************************************/
/*! \struct MsaPmView
 *  Data structure for the progress meter */
typedef struct
{
    /* The form */
    WeWindowHandle formHandle;
    /* Gadget handles */
    WeGadgetHandle progressGadget;
    WeGadgetHandle textGadget;
    WE_UINT16      yPos;
}MsaPmView;

/******************************************************************************
 * Global variables
 *****************************************************************************/
static MsaPmView         msaPmView;

/******************************************************************************
 * Function implementations
 *****************************************************************************/

/*!
 * \brief Create the initial data-structures needed for MSA to run.
 *
 *****************************************************************************/
void msaInitPm()
{
    memset(&msaPmView, 0, sizeof(MsaPmView));
}

/*!
 * \brief Free the resources allocated in msaInitGui.
 *
 *****************************************************************************/
void msaTerminatePm()
{
    msaPmViewRemove();
}

/*!
 * \brief Creates a progress meter "dialog".
 *
 * \param messageStrId The message that is displayed in the PM dialog.
 * \param manualTick Tick the progress meter manually.
 * \param allowCancel Tells whether a cancel signal is sent when the user wants
 *                    to cancel the ongoing operation.
 * \param fsm The fsm that receives a signal when the End User wants to cancel
 *            the ongoing operation.
 * \param signal The signal that is sent to the fsm.
 * \return TRUE if successful, otherwise FALSE. If the operation failed, call
 *         msaPmViewRemove for a clean-up.
 *****************************************************************************/
WE_BOOL msaPmViewShow(WE_UINT32 messageStrId, WE_BOOL manualTick, 
    int allowCancel, int fsm, int signal)
{
    WePosition pos;
    WeSize     tSize;  /* Text size */
    WeSize     pSize;  /* Progress meter size */
    WeSize     dSize;  /* Display size */

    if (0 != msaPmView.formHandle)
    {
        return FALSE;
    }

    if (0 == (msaPmView.formHandle = WE_WIDGET_FORM_CREATE(WE_MODID_MSA, 
        NULL, 0, 0)))
    {
        return FALSE;
    }
    /* Add text */
    (void)WE_WIDGET_GET_INSIDE_AREA(msaPmView.formHandle, &pos, &dSize);
    tSize = pSize = dSize;
    tSize.height = 0;
    tSize.width = (short)(tSize.width - 2*MSA_GADGET_DEF_PADDING);
    if (0 == (msaPmView.textGadget = WE_WIDGET_STRING_GADGET_CREATE(
        WE_MODID_MSA, MSA_GET_STR_ID(messageStrId), NULL, tSize.width,
        FALSE, 0 ,0)))
    {
        return FALSE;
    }
    (void)WE_WIDGET_GET_SIZE(msaPmView.textGadget, &tSize);
    /* Create progress meter gadget */
    pSize.height = (WE_UINT16)((pSize.height*MSA_BAR_SIZE_IN_PER_CENT)/100); /* Use 20 per cent */
    pSize.width = tSize.width;
    if (0 == (msaPmView.progressGadget = WE_WIDGET_BAR_CREATE(WE_MODID_MSA,
        WeProgressBar, MSA_PM_TICKS, 0, 0, &pSize, 0, 0)))
    {
        return FALSE;
    }
    pos.x = MSA_GADGET_DEF_PADDING;
    pos.y = (WE_UINT16)((dSize.height - MSA_GADGET_DEF_PADDING - 
        tSize.height - tSize.height)/2);
    msaPmView.yPos = pos.y;
    if (0 > WE_WIDGET_WINDOW_ADD_GADGET(msaPmView.formHandle, 
        msaPmView.textGadget, &pos)) /* Q04A */
    {
        return FALSE;
    }
    pos.y = (WE_UINT16)(pos.y + MSA_GADGET_DEF_PADDING + tSize.height);    
    if (0 > WE_WIDGET_WINDOW_ADD_GADGET(msaPmView.formHandle, 
        msaPmView.progressGadget, &pos)) /* Q04A */
    {
        return FALSE;
    }
    if (!manualTick)
    {
        WE_TIMER_SET(WE_MODID_MSA, MSA_PM_TIMER_ID, MSA_PM_TICK_TIME);
    }
    /* Create cancel action if needed */
    if (allowCancel)
    {
        if (0 == msaCreateAction(msaPmView.formHandle, WeCancel, NULL, 
            fsm, signal, MSA_STR_ID_CANCEL))
            return FALSE;
    }
    /* Show the progress meter view */
    return msaDisplayWindow(msaPmView.formHandle, msaGetPosLeftTop());
}

/*!\brief Removes the progress meter dialog.
 *
 *****************************************************************************/
void msaPmViewRemove()
{
    /* Delete the form and its associated actions */
    if (0 == msaPmView.formHandle)
    {
        return;
    }
    
    /* Remove gadgets */
    if (0 != msaPmView.progressGadget)
    {
        (void)WE_WIDGET_RELEASE(msaPmView.progressGadget);
    }

    if (0 != msaPmView.textGadget)
    {
        (void)WE_WIDGET_RELEASE(msaPmView.textGadget);
    }
    if (!msaDeleteWindow(msaPmView.formHandle))
    {
        /* Delete the window if is is not registered */
        if (!WE_WIDGET_REMOVE(msaGetScreenHandle(), msaPmView.formHandle))
        {
            (void)WE_WIDGET_RELEASE(msaPmView.formHandle);
        }
    }
    /* Reset timer */
    WE_TIMER_RESET(WE_MODID_MSA, MSA_PM_TIMER_ID);
    /* Delete obsolete handles */
    memset(&msaPmView, 0, sizeof(MsaPmView));
}

/*!\brief Handles timer events for the progress meter dialog.
 *
 *****************************************************************************/
void msaPmHandleTimer()
{
    int value;
    int maxValue;
    int pageValue;
    if (0 != msaPmView.progressGadget)
    {
        (void)WE_WIDGET_BAR_GET_VALUES(msaPmView.progressGadget, 
            &value, &maxValue, &pageValue);
        value++;
        (void)WE_WIDGET_BAR_SET_VALUES(msaPmView.progressGadget, 
            value%MSA_PM_TICKS, maxValue, pageValue);
        WE_TIMER_SET(WE_MODID_MSA, MSA_PM_TIMER_ID, MSA_PM_TICK_TIME);
    }
}

/*!\brief Handles timer events for the progress meter dialog.
 *
 * \param perCent The progress in "per cent".
 * \param str The string to display or NULL if the string should be unchanged.
 *****************************************************************************/
void msaPmSetProgress(unsigned int perCent, const char *str)
{
    int value;
    int maxValue;
    int pageValue;
    WeSize size;
    WePosition pos;

    WeStringHandle strHandle;
    if (0 != msaPmView.progressGadget)
    {
        (void)WE_WIDGET_BAR_GET_VALUES(msaPmView.progressGadget, 
            &value, &maxValue, &pageValue);
        value = (maxValue*(int)perCent)/100;
        (void)WE_WIDGET_BAR_SET_VALUES(msaPmView.progressGadget, 
            value, maxValue, pageValue);
        if ((NULL != str) && (0 != msaPmView.textGadget))
        {
            if (0 == (strHandle = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, 
                str, WeUtf8, (int)strlen(str) + 1, 0)))
            {
                (void)WE_WIDGET_RELEASE(strHandle);
                return;
            }
            (void)WE_WIDGET_STRING_GADGET_SET(msaPmView.textGadget, strHandle, 
                FALSE);
            (void)WE_WIDGET_GET_SIZE(msaPmView.textGadget, &size);
            pos.y = (WE_INT16)(msaPmView.yPos + MSA_GADGET_DEF_PADDING + size.height);
            pos.x = MSA_GADGET_DEF_PADDING;
            (void)WE_WIDGET_SET_POSITION(msaPmView.progressGadget, &pos);
            (void)WE_WIDGET_RELEASE(strHandle);
        }
    }
}

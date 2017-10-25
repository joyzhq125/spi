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

/* !\file mauicols.c
 * \brief A color selector component.
 */

/* WE */
#include "We_Wid.h"
#include "We_Int.h"
#include "We_Mem.h"
#include "We_Lib.h"
#include "We_Def.h"
#include "We_Cfg.h"

/* MMS */
#include "Mms_Def.h"

/* MSA */
#include "Msa_Intsig.h"
#include "Msa_Rc.h"
#include "Msa_Def.h"
#include "Msa_Types.h"
#include "Msa_Uicmn.h"
#include "Msa_Mem.h"
#include "Msa_Uisig.h"
#include "Msa_Uicols.h"

/******************************************************************************
 * Const
 *****************************************************************************/
#define MSA_COLOR_SEL_SIZE              (30)
#define MSA_COLOR_SEL_PADDING           (5)
#define MSA_COLOR_SEL_WIDTH             (MSA_COLOR_SEL_SIZE + MSA_COLOR_SEL_PADDING)
#define MSA_COLOR_SEL_HEIGHT            (MSA_COLOR_SEL_SIZE + MSA_COLOR_SEL_PADDING)
#define MSA_COLOR_SEL_BORDER_SIZE       (2)

/******************************************************************************
 * Data-structures
 *****************************************************************************/

/*!\struct MsaGadgetColItem
 * \brief Data-structure to keep track of image gadget handles and its 
 *        corresponding color.
 */
typedef struct
{
    WeGadgetHandle handle;     /*!< The gadget handle */
    WE_UINT32      color;      /*!< The color that is associated with the 
                                     gadget */
}MsaGadgetColItem;

/* !\struct MsaColsForm 
 *  \brief Data-structure for radio button input 
 */
typedef struct
{
    WeWindowHandle     winHandle;      /*!< The window handle */
    MsaGadgetColItem    *colors;        /*!< A list of gadgets and its 
                                              corresponding color */
    unsigned int        gadgetsPerRow;  /*!< The total number of colors on one row */
    unsigned int        colorCount;     /*!< The total numer of colors 
                                             displayed to the End User */
    MsaStateMachine     fsm;            /*!< Callback fsm */
    int                 signal;         /*!< Callback signal */
}MsaColsForm;

/******************************************************************************
 * Static data
 *****************************************************************************/
static MsaColsForm *msaColsForm;

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static WE_BOOL createImages(const WE_UINT32 *colors, WE_UINT32 colorCount, 
    const MsaColsForm *form, WE_UINT32 hideColor, unsigned int *gadgetsPerRow);
static void msaHandleOkFormAction(WeWindowHandle winHandle);
static void msaHandleBackFormAction(WeWindowHandle winHandle);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Initiates the color selector handling.
 *
 *****************************************************************************/
void msaColorSelectorInit(void)
{
    /* Set all instance pointers to zero */
    msaColsForm = NULL;
}

/*!
 * \brief Terminates and de-allocates the input form handling.
 *
 *****************************************************************************/
void msaColorSelectorTerminate(void)
{
    if (NULL != msaColsForm)
    {
        msaDeleteColorSelector();
    }
}

/*! \brief Handles key press on the left arrow.
 *	
 *****************************************************************************/
static void handleLeft(WeWindowHandle notUsed)
{
    unsigned int i;
    (void)notUsed;
    if (NULL != msaColsForm)
    {
        for(i = 0; i < msaColsForm->colorCount; i++)
        {
            if (WE_WIDGET_HAS_FOCUS(msaColsForm->colors[i].handle))
            {
                if (i > 0)
                {
                    (void)WE_WIDGET_SET_IN_FOCUS(
                        msaColsForm->colors[i - 1].handle, 1);
                }
                return;
            }
        }
    }
}

/*! \brief Handles key press on the right arrow.
 *	
 *****************************************************************************/
static void handleRight(WeWindowHandle notUsed)
{
    unsigned int i;
    (void)notUsed;
    if (NULL != msaColsForm)
    {
        for(i = 0; i < msaColsForm->colorCount; i++)
        {
            if (WE_WIDGET_HAS_FOCUS(msaColsForm->colors[i].handle))
            {
                if (i + 1 < msaColsForm->colorCount)
                {
                    (void)WE_WIDGET_SET_IN_FOCUS(
                        msaColsForm->colors[i + 1].handle, 1);
                }
                return;
            }
        }
    }
}

/*! \brief Handles key press on the up arrow.
 *	
 *****************************************************************************/
static void handleUp(WeWindowHandle notUsed)
{
    unsigned int i;
    int p;

    (void)notUsed;

    if (NULL != msaColsForm)
    {
        for(i = 0; i < msaColsForm->colorCount; i++)
        {
            if (WE_WIDGET_HAS_FOCUS(msaColsForm->colors[i].handle))
            {
                p = i - msaColsForm->gadgetsPerRow;
                if (p >= 0)
                {
                    (void)WE_WIDGET_SET_IN_FOCUS(
                        msaColsForm->colors[i - msaColsForm->gadgetsPerRow].handle,
                        1);
                }
                return;
            }
        }
    }
}

/*! \brief Handles key press on the down arrow.
 *	
 *****************************************************************************/
static void handleDown(WeWindowHandle notUsed)
{
    unsigned int i;
    (void)notUsed;
    if (NULL != msaColsForm)
    {
        for(i = 0; i < msaColsForm->colorCount; i++)
        {
            if (WE_WIDGET_HAS_FOCUS(msaColsForm->colors[i].handle))
            {
                if (i + msaColsForm->gadgetsPerRow < msaColsForm->colorCount)
                {
                    (void)WE_WIDGET_SET_IN_FOCUS(
                        msaColsForm->colors[i + msaColsForm->gadgetsPerRow].handle, 
                        1);
                }
                return;
            }
        }
    }
}

/*! \brief Creates a matrix of possible colors to input
 *
 * \param colors A list of colors to display to the End User.
 * \param colorCount The total number of colors in the list
 * \param form The current instance
 * \param hideColor A color to exclude from the list
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL createImages(const WE_UINT32 *colors, WE_UINT32 colorCount, 
    const MsaColsForm *form, WE_UINT32 hideColor, unsigned int *gadgetsPerRow)
{
    unsigned int    i;
    unsigned int    p;
    WeSize         size = {MSA_COLOR_SEL_SIZE, MSA_COLOR_SEL_SIZE};
    WeSize         size2 = {MSA_COLOR_SEL_SIZE - MSA_COLOR_SEL_BORDER_SIZE, 
                             MSA_COLOR_SEL_SIZE - MSA_COLOR_SEL_BORDER_SIZE};
    WeSize         dSize;
    WePosition     pos = {1, 1};
    WePosition     currPos = {MSA_COLOR_SEL_PADDING, MSA_COLOR_SEL_PADDING};
    WeColor        black = {0, 0, 0};
    WeColor        col;
    WeBrushHandle  brush;
    WeImageHandle  handle;
    WeGadgetHandle gadgetHandle;
    WePosition     winPos;
    /* Get the current display width */
    (void)WE_WIDGET_GET_INSIDE_AREA(msaColsForm->winHandle , &winPos, &dSize); /* Q04A */
    *gadgetsPerRow = (unsigned int)(dSize.width / (MSA_COLOR_SEL_PADDING + MSA_COLOR_SEL_WIDTH));
    /* Create images */
    for (p = i = 0; i < colorCount; ++i)
    {
        /* Do not display the color to hide */
        if (hideColor == colors[i])
        {
            continue;
        }
        /* Create the image */
        if (0 == (handle = WE_WIDGET_IMAGE_CREATE_EMPTY(WE_MODID_MSA, &size, 0)))
        {
            return FALSE;
        }
        brush = WE_WIDGET_GET_BRUSH(handle);
        /* Set the current color */
        msaIntToWeColor(colors[i], &col);
        if (WE_WIDGET_SET_COLOR(brush, &col, 1) < 0)
        {
            (void)WE_WIDGET_RELEASE(handle);
            return FALSE;
        }
        if (WE_WIDGET_SET_COLOR(brush, &black, 0) < 0)
        {
            (void)WE_WIDGET_RELEASE(handle);
            return FALSE;
        }
        /* Paint the image  rect */
        if (WE_WIDGET_DRAW_RECT(handle, &pos, &size2, 1) < 0)
        {
            (void)WE_WIDGET_RELEASE(handle);
            return FALSE;
        }
        if (0 == (gadgetHandle = WE_WIDGET_IMAGE_GADGET_CREATE(WE_MODID_MSA, 
            handle, &size, WeImageZoom100, WE_GADGET_PROPERTY_FOCUS, 0)))
        {
            (void)WE_WIDGET_RELEASE(handle);
            return FALSE;
        }
        (void)WE_WIDGET_RELEASE(handle);
        /* Release the gadgetHandle later, the handle is needed in order to
           find the focus */
        msaColsForm->colors[p].handle = gadgetHandle;
        /* Check so that there are room left on the "line" */
        if (p > 0 && (p % *gadgetsPerRow) == 0)
        {
            currPos.y += (WE_UINT16)MSA_COLOR_SEL_HEIGHT;
            currPos.x = MSA_COLOR_SEL_PADDING;
        }

        /* Bind the image to the form */
        if (WE_WIDGET_WINDOW_ADD_GADGET(form->winHandle, gadgetHandle, 
            &currPos) < 0) /* Q04A */
        {
            return FALSE;
        }

        /* Update the color table */
        form->colors[p].color = colors[i];
        ++p;
        /* Next item one step to the right */
        /*lint -e{571,713} */
        currPos.x = (WE_UINT16)(currPos.x + MSA_COLOR_SEL_WIDTH + 
            MSA_COLOR_SEL_PADDING);
    }
    return TRUE;
}

/*! \brief Handles the [OK] action for the color selector form.
 *
 * \param notUsed The handle of the form that generated the action.
 *****************************************************************************/
static void msaHandleOkFormAction(WeWindowHandle notUsed)
{
    WE_UINT32 color = 0;
    unsigned int i;

    (void)notUsed;

    /* Find the selected item */
    for (i = 0; i < msaColsForm->colorCount; ++i)
    {
        if (WE_WIDGET_HAS_FOCUS(msaColsForm->colors[i].handle))
        {
            color = msaColsForm->colors[i].color;
            break;
        }
    }
    /* Send the response */
    (void)MSA_SIGNAL_SENDTO_U(msaColsForm->fsm, msaColsForm->signal, color);
    /* Delete the form */
    msaDeleteColorSelector();
}

/*! \brief Handles [Back] action for the color selection form
 *
 * \param notUsed The handle of the form that generated the action.
 *****************************************************************************/
static void msaHandleBackFormAction(WeWindowHandle notUsed)
{
    (void)notUsed;
    /* Delete the instance */
    msaDeleteColorSelector();
}

/*! \brief Creates a color selector form, i.e., a form that the End User can
 *         select a color from a couple of predefined ones.
 *         The form is automaticaly removed if the creation failed or if [ok]
 *         or [back] action was received.
 *
 * \param colors A list of colors to display
 * \param colorCount The total number of items in the list
 * \param hideColor A color to exclude from the list 
 * \param selectedColor The color to be selected
 * \param fsm The callback FSM
 * \param signal The callback signal.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaCreateColorSelector(const WE_UINT32 *colors, unsigned int colorCount, 
    WE_UINT32 hideColor, WE_UINT32 selectedColor, MsaStateMachine fsm, 
    int signal)
{
    unsigned int i;
    if (NULL != msaColsForm)
    {
        return FALSE;
    }
    /* Setup instance data */
    msaColsForm = MSA_ALLOC(sizeof(MsaColsForm));
    memset(msaColsForm, 0, sizeof(MsaColsForm));
    msaColsForm->colorCount = colorCount;
    msaColsForm->fsm        = fsm;
    msaColsForm->signal     = signal;
    /* Create the form */
    if (0 == (msaColsForm->winHandle = WE_WIDGET_FORM_CREATE(
        WE_MODID_MSA, NULL, 0, 0)))
    {
        (void)WE_WIDGET_RELEASE(msaColsForm->winHandle);
        MSA_FREE(msaColsForm);
        msaColsForm = NULL;
        return FALSE;
    }

    /* Ok action */
    if (0 == msaCreateAction(msaColsForm->winHandle, WeOk, 
        msaHandleOkFormAction, 0, 0, MSA_STR_ID_OK))
    {
        msaDeleteColorSelector();
        return FALSE;
    }
    /* Back action */
    if (0 == msaCreateAction(msaColsForm->winHandle, WeCancel, 
        msaHandleBackFormAction, 0, 0, MSA_STR_ID_CANCEL))
    {
        msaDeleteColorSelector();
        return FALSE;
    }
    /* Create images */
    msaColsForm->colors = MSA_ALLOC(sizeof(MsaGadgetColItem)*colorCount);
    memset(msaColsForm->colors, 0, sizeof(MsaGadgetColItem)*colorCount);

    if (!createImages(colors, colorCount, msaColsForm, hideColor, 
        &msaColsForm->gadgetsPerRow))
    {
        msaDeleteColorSelector();
        return FALSE;
    }

    /* Set title*/
    if (!msaSetTitle(msaColsForm->winHandle, 
        MSA_GET_STR_ID(MSA_STR_ID_SELECT_COLOR), 0))
    {
        msaDeleteColorSelector();
        return FALSE;
    }
    /* Get navigation key notifications */
    if (!msaRegisterUserEvent(msaColsForm->winHandle, WeKey_Left, handleLeft, 
        MSA_NOTUSED_FSM, 0) || 
        !msaRegisterUserEvent(msaColsForm->winHandle, WeKey_Right, handleRight, 
        MSA_NOTUSED_FSM, 0) ||
        !msaRegisterUserEvent(msaColsForm->winHandle, WeKey_Up, handleUp, 
        MSA_NOTUSED_FSM, 0) ||
        !msaRegisterUserEvent(msaColsForm->winHandle, WeKey_Down, handleDown, 
        MSA_NOTUSED_FSM, 0))
    {
        msaDeleteColorSelector();
        return FALSE;
    }
    /* Display form */
    if (!msaDisplayWindow(msaColsForm->winHandle, msaGetPosLeftTop()))
    {
        msaDeleteColorSelector();
        return FALSE;
    }

    /* Set the correct item in focus */
    for (i = 0; i < colorCount; ++i)
    {
        if (selectedColor == msaColsForm->colors[i].color)
        {
            (void)WE_WIDGET_SET_IN_FOCUS(msaColsForm->colors[i].handle, 1);
            break;
        }
    }
    return TRUE;
}

/*! brief Deletes the current color selector instance
 *
 *****************************************************************************/
void msaDeleteColorSelector(void)
{
    unsigned int i;
    if (NULL != msaColsForm)
    {
        /* Remove the GUI */
        if (0 != msaColsForm->winHandle)
        {
            (void)msaDeleteWindow(msaColsForm->winHandle);
        }
        /* Free all open handles */
        for (i = 0; i < msaColsForm->colorCount; ++i)
        {
            if (0 != msaColsForm->colors[i].handle)
            {
                (void)WE_WIDGET_RELEASE(msaColsForm->colors[i].handle);
            }
        }
        /* Free the instance data */
        MSA_FREE(msaColsForm->colors);
        MSA_FREE(msaColsForm);
        msaColsForm = NULL;
    }
}

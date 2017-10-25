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

/*!\file mauiform.c
 * \brief Common input forms.
 */

/* WE */
#include "We_Wid.h"
#include "We_Int.h"
#include "We_Mem.h"
#include "We_Lib.h"
#include "We_Def.h"
#include "We_Log.h"
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
#include "Msa_Uiform.h"
#include "Msa_Uisig.h"

/******************************************************************************
 * Const
 *****************************************************************************/
const WePosition msaInputPos = {0, 0};

/*! The maximum number of concurrent input form items */
#define MSA_MAX_INPUT_FORMS     4

/*! \enum MsaInputFormAction
 *  Actions related to the input form */
typedef enum
{
    MSA_INPUT_FORM_NO_OP,  /* !< */
    MSA_INPUT_FORM_OK,     /* !< The End User presses ok */
    MSA_INPUT_FORM_CANCEL  /* !< The End USer presses cancel */
}MsaInputFormAction;

/******************************************************************************
 * Data-structures
 *****************************************************************************/

/* !\struct MsaInputForm 
 *  \brief Data-structure for radio button input 
 */
typedef struct
{
    WeWindowHandle winHandle;      /* !< The window handle */
    WeGadgetHandle gadgetHandle;   /* !< The gadget handle */
    MsaInputItem    item;           /* !< See #MsaInputItem */
} MsaInputForm;

/******************************************************************************
 * Static data
 *****************************************************************************/
static MsaInputForm *msaInputForm[MSA_MAX_INPUT_FORMS];

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static void msaHandleOkFormAction(WeWindowHandle winHandle);
static void msaHandleBackFormAction(WeWindowHandle winHandle);
static void deleteInputForm(WeWindowHandle winHandle,
                            MsaInputFormAction action);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Initiates the input form handling.
 *
 *****************************************************************************/
void msaInitForms(void)
{
    /* Set all instance pointers to zero */
    memset(msaInputForm, 0, sizeof(MsaInputForm*)*MSA_MAX_INPUT_FORMS);
}

/*!
 * \brief Terminates and de-allocates the input form handling.
 *
 *****************************************************************************/
void msaTerminateForms(void)
{
    memset(msaInputForm, 0, sizeof(MsaInputForm*)*MSA_MAX_INPUT_FORMS);
    /* Delete all instances */
    deleteInputForm(0, MSA_INPUT_FORM_NO_OP);
}

/*!
 * \brief Finds the instance matching a widget action.
 * 
 * \param winHandle The window that generated the action.
 * \return A pointer to the instance (with reference to the winHandle) or NULL
 *         if not valid.
 *****************************************************************************/
static MsaInputForm *getInstance(WeWindowHandle winHandle)
{
    int i;
    /* Go through all slots and find the current corresponding instance */
    for(i = 0; i < MSA_MAX_INPUT_FORMS; i++)
    {
        /* Return the matching instance */
        if ((NULL != msaInputForm[i]) && (msaInputForm[i]->winHandle == 
            winHandle))
        {
            return msaInputForm[i];
        }
    }
    return NULL;
}

/*!
 * \brief Creates a new instance.
 *
 * \param item Data for the new instance.
 * \return The new instance or NULL if no new instance could be created.
 *****************************************************************************/
static MsaInputForm *createInstance(const MsaInputItem *item)
{
    int i;
    /* Find an empty slot */
    for(i = 0; i < MSA_MAX_INPUT_FORMS; i++)
    {
        if (NULL == msaInputForm[i])
        {
            msaInputForm[i] = MSA_ALLOC(sizeof(MsaInputForm));
            memset(msaInputForm[i], 0, sizeof(MsaInputForm));
            memcpy(&(msaInputForm[i]->item), item, sizeof(MsaInputItem));
            return msaInputForm[i];
        }
    }
    return NULL;
}

/*!
 * \brief Creates a input form.
 *
 * \param item The type of input form to create.
 * \param titleStrHandle The title of the form.
 * \return TRUE of successful, otherwise FALSE:
 *****************************************************************************/
WE_BOOL msaCreateInputForm(const MsaInputItem *item, 
    WE_UINT32 titleStrHandle)
{
    int i;
    WeStringHandle strHandle;
    MsaInputForm *inst;
    WeActionHandle actionHandle;
	/*"a" is used to produce a place for the cursor*/
	char *temp="a";
	char *text2;
	WeStringHandle strHandle2 = 0;
    const WeSize maxSize = {MSA_GADGET_DEF_HEIGHT, MSA_GADGET_DEF_WIDTH};
    WeSize newSize = {0, 0};
    
    actionHandle = 0;
    
    if (NULL == (inst = createInstance(item)))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) Input form instance is busy!\n", __FILE__, __LINE__));
        return FALSE;
    }
    /* Do not copy the list of elements */
    inst->item.items = NULL;
    /* Create gadget */
    switch(item->type)
    {
    case MSA_SINGLE_LINE_INPUT:
        /* Create string handle */
        if (NULL != item->text)
        {
            if (0 == (strHandle = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, 
                item->text, WeUtf8, strlen(item->text) + 1, 0)))
            {
                return FALSE;
            }
			
			/*create extra string handle to include cursor*/
			text2 = we_cmmn_strcat (WE_MODID_MSA, item->text, temp);
			if (0 == (strHandle2 = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, 
				text2, WeUtf8, strlen(text2) + 1, 0)))
			{
				return FALSE;
			}
			MSA_FREE(text2);
			/*----------------*/
        }
        else
        {
            if (0 == (strHandle = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, 
                "", WeUtf8, 1, 0)))
            {
                return FALSE;
            }
        }
        if (0 == (inst->winHandle = WE_WIDGET_FORM_CREATE(
            WE_MODID_MSA, NULL, 0, 0)))
        {
            (void)WE_WIDGET_RELEASE(strHandle);
            return FALSE;
        }
        if (0 == (inst->gadgetHandle = 
            WE_WIDGET_TEXT_INPUT_CREATE(WE_MODID_MSA, 
            msaGetEmptyStringHandle(), strHandle, item->textType, 
            NULL, 0, (int)item->maxTextSize, TRUE, NULL, 
            WE_GADGET_PROPERTY_FOCUS, 0))) /* Q04A */
        {
            (void)WE_WIDGET_RELEASE(strHandle);
            (void)WE_WIDGET_RELEASE(inst->winHandle);
            inst->winHandle = 0;
        }
		/* Calculate optimal size of gadget */
		if (NULL != item->text)	{
			msaCalculateStringGadgetSize(inst->winHandle, inst->gadgetHandle,
				strHandle2, (WePosition*)&msaInputPos, (WeSize*)&maxSize, 
				&newSize, FALSE);
			/*release the extra strHandle2 if strHandel2 get a value*/
			(void)WE_WIDGET_RELEASE(strHandle2);
		}
		else
			msaCalculateStringGadgetSize(inst->winHandle, inst->gadgetHandle,
			strHandle, (WePosition*)&msaInputPos, (WeSize*)&maxSize, 
			&newSize, FALSE);

        (void)WE_WIDGET_SET_SIZE(inst->gadgetHandle, &newSize);

        (void)WE_WIDGET_RELEASE(strHandle);
        if (0 > WE_WIDGET_WINDOW_ADD_GADGET(inst->winHandle, 
            inst->gadgetHandle, &msaInputPos)) /* Q04A */
        {
            (void)WE_WIDGET_RELEASE(inst->winHandle);
            inst->winHandle = 0;
            (void)WE_WIDGET_RELEASE(inst->gadgetHandle);
            inst->gadgetHandle = 0;
            return FALSE;
        }
        break;
    case MSA_TEXT_INPUT:
        /* Create string handle */
        if (NULL != item->text)
        {
            if (0 == (strHandle = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, 
                item->text, WeUtf8, strlen(item->text) + 1, 0)))
            {
                return FALSE;
            }
        }
        else
        {
            if (0 == (strHandle = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, 
                "", WeUtf8, 1, 0)))
            {
                return FALSE;
            }
        }
        /* Create editor */
        if (0 == (inst->winHandle = WE_WIDGET_EDITOR_CREATE(
            WE_MODID_MSA, msaGetEmptyStringHandle(), strHandle,
            item->textType, 0, 0, (int)item->maxTextSize, 
            (MSA_TEXT_INPUT == item->type), NULL, 0, 0))) /* Q04A */
        {
            (void)WE_WIDGET_RELEASE(strHandle);
            return FALSE;
        }
        (void)WE_WIDGET_RELEASE(strHandle);
        break;
    case MSA_RADIO_BUTTON_INPUT:
        /* Create radio button list */
        if (0 == (inst->winHandle = WE_WIDGET_MENU_CREATE(
            WE_MODID_MSA, WeExclusiveChoice, NULL, NULL,
            WE_CHOICE_ELEMENT_STRING_1 | WE_CHOICE_ELEMENT_ICON, 
            0, 0, 0, 0)))
        {
            return FALSE;
        }
        /* Add items to the list */
        for(i = 0; 0 != item->items[i]; i++)
        {
            if (0 > WE_WIDGET_CHOICE_SET_ELEMENT(inst->winHandle,
                i, 0, item->items[i], 0, 0, 0, 0, 0, FALSE)) /* Q04A */
            {
                (void)WE_WIDGET_RELEASE(inst->winHandle);
                return FALSE;
            }
        }
        /* Set selected index */
        if ((inst->item.selectedItem < i) && (i >= 0))
        {
            (void)WE_WIDGET_CHOICE_SET_ELEM_STATE(inst->winHandle, 
                    inst->item.selectedItem, WE_CHOICE_ELEMENT_SELECTED);
        }
        else
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) erroneous selected index!\n", __FILE__, __LINE__));
        }
        break;
    case MSA_SELECT_INPUT:
        /* Q04A */
        if (0 == (inst->winHandle = WE_WIDGET_MENU_CREATE(
            WE_MODID_MSA, WeImplicitChoice, NULL, NULL, 
            WE_CHOICE_ELEMENT_STRING_1, 0, 0, 0, 0)))
        {
            return FALSE;
        }
        /* Add items to the list */
        for(i = 0; 0 != item->items[i]; i++)
        {
            if (0 > WE_WIDGET_CHOICE_SET_ELEMENT(inst->winHandle,
                i, 0, item->items[i], 0, 0, 0, 0, 0, FALSE)) /* Q04A */
            {
                (void)WE_WIDGET_RELEASE(inst->winHandle);
                return FALSE;
            }
        }
        /* Set selected index */
        if ((inst->item.selectedItem < i) && (i >= 0))
        {
            (void)WE_WIDGET_CHOICE_SET_ELEM_STATE(inst->winHandle, 
                    inst->item.selectedItem, WE_CHOICE_ELEMENT_SELECTED);
        }
        else
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) erroneous selected index!\n", __FILE__, __LINE__));
        }
        break;
    }
    /* Actions */
    if (MSA_SELECT_INPUT == item->type)
    {
        /* Select action */ /* Q04A */
        if (!msaCreateAction(inst->winHandle, WeSelect,
            msaHandleOkFormAction, 0, 0, MSA_STR_ID_OK))
        {
            return FALSE;
        }
        /* Back action */
        if (0 == msaCreateAction(inst->winHandle, WeBack, msaHandleBackFormAction, 
            0, 0, MSA_STR_ID_BACK))
        {
            return FALSE;
        }
    }
    else
    {
        if (0 == msaCreateAction(inst->winHandle, WeOk, msaHandleOkFormAction, 
            0, 0, MSA_STR_ID_OK))
        {
            return FALSE;
        }
        /* Cancel action */
        if (0 == msaCreateAction(inst->winHandle, WeCancel, msaHandleBackFormAction, 
            0, 0, MSA_STR_ID_CANCEL))
        {
            return FALSE;
        }
    }

    /* Set title*/
    if (!msaSetTitle(inst->winHandle, titleStrHandle, 0))
    {
        return FALSE;
    }
    /* Display form */
    return msaDisplayWindow(inst->winHandle, msaGetPosLeftTop());
}

/*!
 *  \brief Deletes the current input form.
 *
 *  \param winHandle The window handle of the instance to delete or 0 to delete
 *                  all instances.
 *  \param action Tells the reason why the function was called. See
 *                #MsaInputFormAction for possible values.
 *****************************************************************************/
static void deleteInputForm(WeWindowHandle winHandle, 
    MsaInputFormAction action)
{
    MsaInputForm *currItem;
    int i;
    void *ptr = NULL;
    unsigned int value = 0;
    MsaInputActionBackCallBack callback;

    /*lint -e{611} */
    callback = (MsaInputActionBackCallBack)NULL;

    if (0 != winHandle)
    {
        if (0 != (currItem = getInstance(winHandle)))
        {
            /*  Save values before we deallocates the instance in case 
             *  we need them later */
            callback = currItem->item.backCallback;
            value = currItem->item.uintValue;
            ptr = currItem->item.dataPtr;

            /* Remove the gadget if it has been created */
            if (0 != currItem->gadgetHandle)
            {
                (void)WE_WIDGET_RELEASE(currItem->gadgetHandle);
            }
            /* Remove the window and its actions */
            (void)msaDeleteWindow(currItem->winHandle);
            /* Delete the instance */
            /*lint -e{774}*/
            MSA_FREE(currItem);
            for(i = 0; i < MSA_MAX_INPUT_FORMS; i++)
            {
                if (currItem == msaInputForm[i])
                {
                    msaInputForm[i] = NULL;
                    break;
                }
            }
        }
    }
    else
    {
        /* Delete all instances */
        for (i = 0; i < MSA_MAX_INPUT_FORMS; i++)
        {
            if (NULL != msaInputForm[i])
            {
                if (0 != msaInputForm[i]->gadgetHandle)
                {
                    (void)WE_WIDGET_RELEASE(msaInputForm[i]->gadgetHandle);
                }
                /* Remove the window and its actions */
                (void)msaDeleteWindow(msaInputForm[i]->winHandle);
                MSA_FREE(msaInputForm[i]);
                msaInputForm[i] = NULL;
            }
        }
    }
    /* The window is closed, tell the user about it */
    if ((NULL != callback) && (MSA_INPUT_FORM_CANCEL == action))
    {
        /* Call callback, preserve original data parameters */
        callback(ptr, value);
    }
}

/*!
 *  \brief Handles "Back" actions for the input form.
 *  \param winHandle The window handle of the current form
 *****************************************************************************/
static void msaHandleBackFormAction(WeWindowHandle winHandle)
{
    deleteInputForm(winHandle, MSA_INPUT_FORM_CANCEL);
}

/*!
 *  \brief Handles "Ok" actions for the input form.
 *  \param winHandle The window handle of the current form
 *****************************************************************************/
static void msaHandleOkFormAction(WeWindowHandle winHandle)
{
    char *strData;
    int intData;
    MsaInputForm *inst;

    if (NULL != (inst = getInstance(winHandle)))
    {
        strData = NULL;
        intData = 0;
        /* Get data from the form */
        switch(inst->item.type)
        {
        case MSA_SINGLE_LINE_INPUT:
            strData = msaGetTextFromGadget(inst->gadgetHandle);
            break;
        case MSA_TEXT_INPUT:
            strData = msaGetTextFromGadget(inst->winHandle);
            break;
        case MSA_SELECT_INPUT:
        case MSA_RADIO_BUTTON_INPUT:
            if ((intData = msaGetSelectedChoiceIndex(inst->winHandle)) < 0)
            {
                WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                    "(%s) (%d) Erroneous choice index\n", __FILE__, __LINE__));
                return;
            }
            break;
        }
        /* Check if a callback is registered */
        if (NULL != inst->item.callback)
        {
            /* Call callback, check if the form can be closed down*/
            if (inst->item.callback(strData, (unsigned)intData, 
                inst->item.dataPtr, inst->item.uintValue))
            {
                deleteInputForm(winHandle, MSA_INPUT_FORM_OK);
            }
        }
        else
        {
            /* No callback is registered, close the form */
            deleteInputForm(winHandle, MSA_INPUT_FORM_OK);
        }
    }
}

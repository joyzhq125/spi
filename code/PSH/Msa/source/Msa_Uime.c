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

/* !\file mauime.c
 *  \brief UI for the message editor.
 */

/* WE */
#include "We_Wid.h"
#include "We_Int.h"
#include "We_Def.h"
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Mem.h"
#include "We_Core.h"

/* MMS */
#include "Mms_Def.h"

/* MSA */
#include "Msa_Def.h"
#include "Msa_Env.h"
#include "Msa_Cfg.h"
#include "Msa_Rc.h"

#include "Msa_Types.h"
#include "Msa_Mem.h"
#include "Msa_Intsig.h"
#include "Msa_Uicmn.h"
#include "Msa_Se.h"
#include "Msa_Mr.h"
#include "Msa_Me.h"
#include "Msa_Uimenu.h"
#include "Msa_Uime.h"
#include "Msa_Se.h"
#include "Msa_Conf.h"
#include "Msa_Uisig.h"
#include "Msa_Uidia.h"
#include "Msa_Addr.h"
#include "Msa_Pbh.h"

/******************************************************************************
 * Constants
 *****************************************************************************/

/******************************************************************************
 * Data-types
 *****************************************************************************/

/*! \struct MsaMePropView
 *  MM property view */
typedef struct
{
    /* Form */
    WeWindowHandle formHandle;
    /* Gadgets */
    WeGadgetHandle toGadgetHandle;
    WeGadgetHandle ccGadgetHandle;
    WeGadgetHandle bccGadgetHandle;
    WeGadgetHandle subjectGadgetHandle;
    WeGadgetHandle optionsGadgetHandle;
    WeGadgetHandle contentGadgetHandle;
    WeGadgetHandle sendGadgetHandle;
}MsaMePropView;

/*! \struct MsaMeAddrInput
 *
 */
typedef struct
{
    WeWindowHandle winHandle;
#ifdef MSA_CFG_USE_EXT_PB
    WeGadgetHandle phoneBookHandle;
#endif 
    WeGadgetHandle textInputHandle;
    MsaMeProperty   type;
    int             editIndex;
}MsaMeAddrInputView;

/*! \struct MsaMeAddrListView
 *
 */
typedef struct
{
    WeWindowHandle winHandle;
    MsaMeProperty   type;
    MsaAddrItem     **items;
}MsaMeAddrListView;

/******************************************************************************
 * Internal data-structures for the GUI.
 *****************************************************************************/
static MsaMePropView        msaMePropView;
static MsaMeAddrInputView   msaMeAddrInputView;
static MsaMeAddrListView    msaMeAddrListView;

/******************************************************************************
 * Prototypes
 *****************************************************************************/

static void navMenuSignalConverter(MsaNavAction actionType);
static void handlePropBackAction(WeWindowHandle winHandle);
static char *createRecipStr(const MsaAddrItem *items);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Init function.
 *****************************************************************************/
void msaMeUiInit()
{
    /* Initiate data-structures */
    memset(&msaMePropView, 0, sizeof(MsaMePropView));
    memset(&msaMeAddrInputView, 0, sizeof(MsaMeAddrInputView));
    memset(&msaMeAddrListView, 0, sizeof(MsaMeAddrListView));
}

/*!
 * \brief Back action handler for message prop view.
 *****************************************************************************/
static void handlePropBackAction(WeWindowHandle winHandle)
{
    WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
        "(%s) (%d) Back action received 0x(%x)", __FILE__, __LINE__, 
        winHandle));
    (void)winHandle;
    (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_DEACTIVATE);
}

/*!
 * \brief Creates a text input gadget.
 *
 * \param win The handle of destination window for the new text input gadget.
 * \param gadget A pointer to a gadget handle, the created gadget handle is
 *               written here.
 * \param pos   The position of the text input gadget.
 * \return      TRUE if the gadget was created successfully, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL createInputGadget(WeWindowHandle win, WeGadgetHandle *gadget, 
    WePosition *pos)
{
    WeSize size;
    WePosition tmpPos;

    WE_WIDGET_GET_INSIDE_AREA(win, &tmpPos, &size);
    /* Create gadget */
    if (0 == (*gadget = WE_WIDGET_STRING_GADGET_CREATE(WE_MODID_MSA, 
        msaGetEmptyStringHandle(), NULL, (WE_UINT16)(size.width - 
        2*MSA_GADGET_DEF_PADDING), TRUE, WE_GADGET_PROPERTY_FOCUS, 0)))
    {
        return FALSE;
    }
    (void)WE_WIDGET_GET_SIZE(*gadget, &size);
    pos->x = (WE_UINT16)(tmpPos.x + MSA_GADGET_DEF_PADDING);
    /* Bind gadget to form */
    if (0 > WE_WIDGET_WINDOW_ADD_GADGET(win, *gadget, pos)) /* Q04A */
    {
        return FALSE;
    }
    pos->y = (WE_UINT16)(pos->y + size.height + MSA_GADGET_DEF_PADDING);
    return TRUE;
}

/*!
 * \brief Handles ok action for the property form.
 *
 * \param winHandle The handle of the window that generated the action.
 *****************************************************************************/
static void handlePropOk(WeWindowHandle winHandle)
{
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
        "(%s) (%d) Ok actions received 0x%x\n", __FILE__, __LINE__, winHandle));
    (void)winHandle;
    if ((0 != msaMePropView.toGadgetHandle) && 
        WE_WIDGET_HAS_FOCUS(msaMePropView.toGadgetHandle))
    {
        (void)MSA_SIGNAL_SENDTO_U(MSA_ME_FSM, MSA_SIG_ME_EDIT_PROPERTY, 
            MSA_ME_TO);
    }
    else if((0 != msaMePropView.ccGadgetHandle) && 
        WE_WIDGET_HAS_FOCUS(msaMePropView.ccGadgetHandle))
    {
        (void)MSA_SIGNAL_SENDTO_U(MSA_ME_FSM, MSA_SIG_ME_EDIT_PROPERTY, 
            MSA_ME_CC);
    }
    else if((0 != msaMePropView.bccGadgetHandle) && 
        WE_WIDGET_HAS_FOCUS(msaMePropView.bccGadgetHandle))
    {
        (void)MSA_SIGNAL_SENDTO_U(MSA_ME_FSM, MSA_SIG_ME_EDIT_PROPERTY, 
            MSA_ME_BCC);
    }    
    else if((0 != msaMePropView.subjectGadgetHandle) && 
        WE_WIDGET_HAS_FOCUS(msaMePropView.subjectGadgetHandle))
    {
        (void)MSA_SIGNAL_SENDTO_U(MSA_ME_FSM, MSA_SIG_ME_EDIT_PROPERTY, 
            MSA_ME_SUBJECT);
    }
    else if((0 != msaMePropView.optionsGadgetHandle) && 
        WE_WIDGET_HAS_FOCUS(msaMePropView.optionsGadgetHandle))
    {
        (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_EDIT_PROPERTIES);
    }
    else if((0 != msaMePropView.contentGadgetHandle) && 
        WE_WIDGET_HAS_FOCUS(msaMePropView.contentGadgetHandle))
    {
        (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_PROP_NEXT);
    }
    else if ((0 != msaMePropView.sendGadgetHandle) && 
        WE_WIDGET_HAS_FOCUS(msaMePropView.sendGadgetHandle))
    {
        (void)MSA_SIGNAL_SENDTO_UU(MSA_ME_FSM, MSA_SIG_ME_SEND, MSA_ME_FSM, 
            MSA_SIG_ME_PROP_SEND_DONE);
    }
    else
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
            "(%s) (%d) No valid focus for message properties!\n", __FILE__, 
            __LINE__));
    }
}


/*!
 * \brief Creates the MM property edit view.
 *
 * \param propItem The current message properties.
 * \param mode Current start up mode for ME.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaMeCreatePropView(const MsaPropItem *propItem, 
    MsaMeStartupMode mode)
{
    WePosition pos = {MSA_GADGET_DEF_PADDING, MSA_GADGET_DEF_PADDING};
    WeSize size = {0, 0};
    char *str;

    if (0 != msaMePropView.formHandle)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) The property view instance is busy!\n", __FILE__, 
            __LINE__));
        return FALSE;
    }

    memset(&msaMePropView, 0, sizeof(MsaMePropView));
    /* Create form*/
    msaMePropView.formHandle = WE_WIDGET_FORM_CREATE(WE_MODID_MSA, NULL, 
        WE_WINDOW_PROPERTY_TITLE, 0);
    if (0 == msaMePropView.formHandle)
        return FALSE;

    /* Create actions */
    if (0 == msaCreateAction(msaMePropView.formHandle, WeOk,
        handlePropOk, 0, 0, MSA_STR_ID_OK))
    {
        return FALSE;
    }
    if (0 == msaCreateAction(msaMePropView.formHandle, WeBack, 
        handlePropBackAction, 0, 0, MSA_STR_ID_BACK))
    {
        return FALSE;
    }
    if (0 == msaCreateAction(msaMePropView.formHandle, WeMenu, 
        NULL, MSA_ME_FSM, MSA_SIG_ME_NAV_ACTIVATE, MSA_STR_ID_MENU))
    {
        return FALSE;
    }
    /* Create gadgets */
    /* To */
    if (!createInputGadget(msaMePropView.formHandle, 
        &(msaMePropView.toGadgetHandle), &pos))
    {
        return FALSE;
    }
    /* To */
    str = createRecipStr(propItem->to);
    if (!msaSetStringGadgetValue(msaMePropView.toGadgetHandle, 
        MSA_GET_STR_ID(MSA_STR_ID_TO), str))
    {
        MSA_FREE(str);
        return FALSE;
    }
    MSA_FREE(str);

    /* Cc */
    if (!createInputGadget(msaMePropView.formHandle, 
        &(msaMePropView.ccGadgetHandle), &pos))
    {
        return FALSE;
    }
    str = createRecipStr(propItem->cc);
    if (!msaSetStringGadgetValue(msaMePropView.ccGadgetHandle,
        MSA_GET_STR_ID(MSA_STR_ID_CC), str))
    {
        MSA_FREE(str);
        return FALSE;
    }
    MSA_FREE(str);

    /* Bcc */
    if (!createInputGadget(msaMePropView.formHandle, 
        &(msaMePropView.bccGadgetHandle), &pos))
    {
        return FALSE;
    }
    str = createRecipStr(propItem->bcc);
    if (!msaSetStringGadgetValue(msaMePropView.bccGadgetHandle,
        MSA_GET_STR_ID(MSA_STR_ID_BCC), str))
    {
        MSA_FREE(str);
        return FALSE;
    }
    MSA_FREE(str);

    /* Subject */
    if (!createInputGadget(msaMePropView.formHandle, 
        &(msaMePropView.subjectGadgetHandle), &pos))
    {
        return FALSE;
    }
    if (!msaSetStringGadgetValue(msaMePropView.subjectGadgetHandle, 
        MSA_GET_STR_ID(MSA_STR_ID_SUBJECT), propItem->subject))
    {
        return FALSE;
    }
    
    /* Options */
    if (0 == (msaMePropView.optionsGadgetHandle = 
        WE_WIDGET_STRING_GADGET_CREATE(WE_MODID_MSA, 
        MSA_GET_STR_ID(MSA_STR_ID_OPTIONS), NULL, 0, TRUE, 
        WE_GADGET_PROPERTY_FOCUS, 0)))
    {
        return FALSE;
    }
    /* Bind gadget to form */
    if (0 > WE_WIDGET_WINDOW_ADD_GADGET(msaMePropView.formHandle, 
        msaMePropView.optionsGadgetHandle, &pos)) /* Q04A */
    {
        return FALSE;
    }
    if (WE_WIDGET_GET_SIZE(msaMePropView.optionsGadgetHandle, &size) < 0) 
    {
        return FALSE;
    }
    pos.y = (WE_INT16)((int)pos.y + (int)size.height + 
        MSA_GADGET_DEF_PADDING);
    /* Content */
    if (mode == MSA_ME_STARTUP_FORWARD)
    {
        /* Send button */
        if (0 == (msaMePropView.sendGadgetHandle = 
            WE_WIDGET_STRING_GADGET_CREATE(WE_MODID_MSA, 
            MSA_GET_STR_ID(MSA_STR_ID_SEND), NULL, 0, TRUE, 
            WE_GADGET_PROPERTY_FOCUS, 0)))
        {
            return FALSE;
        }
        /* Bind gadget to form */
        if (0 > WE_WIDGET_WINDOW_ADD_GADGET(msaMePropView.formHandle, 
            msaMePropView.sendGadgetHandle, &pos)) /* Q04A */
        {
            return FALSE;
        } 
    }
    else
    {
        /* Edit content in forward mode */
        if (0 == (msaMePropView.contentGadgetHandle = 
            WE_WIDGET_STRING_GADGET_CREATE(WE_MODID_MSA, 
            MSA_GET_STR_ID(MSA_STR_ID_EDIT_CONTENT), NULL, 0, TRUE, 
            WE_GADGET_PROPERTY_FOCUS, 0)))
        {
            return FALSE;
        }
        /* Bind gadget to form */
        if (0 > WE_WIDGET_WINDOW_ADD_GADGET(msaMePropView.formHandle, 
            msaMePropView.contentGadgetHandle, &pos)) /* Q04A */
        {
            return FALSE;
        } 
    }
    /* Title */
    (void)msaSetTitle(msaMePropView.formHandle, 
        MSA_GET_STR_ID(MSA_STR_ID_ME_TITLE), 0);
    /* Display form */
    return msaDisplayWindow(msaMePropView.formHandle, msaGetPosLeftTop());
}

/*! 
 * \brief Creates a string representation of a collection of recipients 
 *
 * \param items The collection of recipients
 * \return The string representation ("4 recipients"/"Mr. Foo Bar")
 *****************************************************************************/
static char *createRecipStr(const MsaAddrItem *items)
{
    char *str;
    char *newStr;
    unsigned int count;
    if (NULL != items)
    {
        if ((count = Msa_AddrItemCount(items)) > 1)
        {
            if (NULL == (str = msaGetStringFromHandle(MSA_GET_STR_ID(
                MSA_STR_ID_NO_OF_RECIPIENTS))))
            {
                return NULL;
            }

            newStr = MSA_ALLOC(strlen(str) + 1 + 4);
            sprintf(newStr, " %d%s", count, str);
            /*lint -e{774} */
            MSA_FREE(str);
            return newStr;
        }
        else
        {
            str = (NULL != items->name) ? items->name: items->address;
            newStr = MSA_ALLOC(strlen(str) + 1);
            strcpy(newStr, str);
            return newStr;
        }
    }
    return NULL;
}

/*!
 * \brief Sets property view values.
 *
 * \param propItem A reference to a MsaMePropItem.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaMeSetPropertyValues(const MsaPropItem *propItem)
{
    char *str;
    if (0 != msaMePropView.formHandle)
    {
        /* To */
        str = createRecipStr(propItem->to);
        if (!msaSetStringGadgetValue(msaMePropView.toGadgetHandle, 
            MSA_GET_STR_ID(MSA_STR_ID_TO), str))
        {
            MSA_FREE(str);
            return FALSE;
        }
        MSA_FREE(str);
        /* Cc */
        str = createRecipStr(propItem->cc);
        if (!msaSetStringGadgetValue(msaMePropView.ccGadgetHandle,
            MSA_GET_STR_ID(MSA_STR_ID_CC), str))
        {
            MSA_FREE(str);
            return FALSE;
        }
        MSA_FREE(str);
        /* Bcc */
        str = createRecipStr(propItem->bcc);
        if (!msaSetStringGadgetValue(msaMePropView.bccGadgetHandle,
            MSA_GET_STR_ID(MSA_STR_ID_BCC), str))
        {
            MSA_FREE(str);
            return FALSE;
        }
        MSA_FREE(str);
        /* Subject */
        if (!msaSetStringGadgetValue(msaMePropView.subjectGadgetHandle, 
            MSA_GET_STR_ID(MSA_STR_ID_SUBJECT), propItem->subject))
        {
            return FALSE;
        }
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*!
 * \brief Deletes the message property view.
 *****************************************************************************/
void msaMeDeletePropView()
{
    /* Delete window and handles */
    if (0 == msaMePropView.formHandle)
        return;
    (void)msaDeleteWindow(msaMePropView.formHandle);
    /* Delete gadgets */
    if (0 != msaMePropView.bccGadgetHandle)
    {
        (void)WE_WIDGET_RELEASE(msaMePropView.bccGadgetHandle);
    }
    if (0 != msaMePropView.ccGadgetHandle)
    {
        (void)WE_WIDGET_RELEASE(msaMePropView.ccGadgetHandle);
    }
    if (0 != msaMePropView.subjectGadgetHandle)
    {
        (void)WE_WIDGET_RELEASE(msaMePropView.subjectGadgetHandle);
    }
    if (0 != msaMePropView.toGadgetHandle)
    {
        (void)WE_WIDGET_RELEASE(msaMePropView.toGadgetHandle);
    }
    if (0 != msaMePropView.optionsGadgetHandle)
    {
        (void)WE_WIDGET_RELEASE(msaMePropView.optionsGadgetHandle);
    }
    if (0 != msaMePropView.sendGadgetHandle)
    {
        (void)WE_WIDGET_RELEASE(msaMePropView.sendGadgetHandle);
    }
    if (0 != msaMePropView.contentGadgetHandle)
    {
        (void)WE_WIDGET_RELEASE(msaMePropView.contentGadgetHandle);
    }    
    /* Delete obsolete handles */
    memset(&msaMePropView, 0, sizeof(MsaMePropView));
}

/*!
 * \brief Signal converter for the navigation menu.
 * 
 * \param actionType The type of action, MSA_NAV_ACTION_BACK or 
 *  MSA_NAV_ACTION_OK.
 *****************************************************************************/

static void navMenuSignalConverter(MsaNavAction actionType)
{
    MsaNavMenuItem item;
    
    if (actionType == MSA_NAV_ACTION_OK)
    {
        item = msaGetNavMenuItemNew();
        (void)MSA_SIGNAL_SENDTO(item.fsm, item.sig);
    } /* if */
    /* remove navigation menu */
    (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_NAV_DEACTIVATE);
}

/*!
 * \brief Creates a navigation menu for the message editor. Only called from 
 *        the property view!
 * 
 * \return TRUE if the menu was created successfully, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaMeCreateNavMenu(MsaNavMenuItem *items)
{
    unsigned int i;
    
    /* Create navigation menu */
    if (!msaCreateNavMenu(0, navMenuSignalConverter))
    {
        msaRemoveNavMenu();
        return FALSE;
    }

    /* Add menu items */
    for (i = 0; items[i].strId != 0; i++)
    {
        if (!msaAddNavMenuItemNew(&items[i]))
        {
            msaRemoveNavMenu();
            return FALSE;
        }
    } /* for */

    /* Show the menu */
    if (!msaShowNavMenu())
    {
        msaRemoveNavMenu();
        return FALSE;
    }

    return TRUE;
}

/*!
 * \brief Deletes the navigation menu for the message editor.
 * 
 *****************************************************************************/
void msaMeDeleteNavMenu()
{
    msaRemoveNavMenu();
}

/*! \brief Creates a string from a #MsaAddrItem.
 *
 * \param item The current item
 * \return A string handle or 0.
 *****************************************************************************/
static WeStringHandle addrItemToStrHandle(const MsaAddrItem *item)
{
    char *str;

    str = (NULL != item->name) ? item->name: item->address;
    return WE_WIDGET_STRING_CREATE(WE_MODID_MSA, str, WeUtf8, 
        (int)(strlen(str) + 1), 0);
}

/*! \brief Handles OK actions from the addr 
 *
 * \param winHandle The window that the actin originates from.
 *****************************************************************************/
static void handleAddrInputOk(WeWindowHandle winHandle)
{
    char *addr;
    MsaAddrType addrType;
    MsaAddrItem *item = NULL;

    WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
        "(%s) (%d) Addr input view [Ok] action received 0x(%x)", 
        __FILE__, __LINE__, winHandle));
    (void)winHandle;

#ifdef MSA_CFG_USE_EXT_PB
    if ((0 != msaMeAddrInputView.phoneBookHandle ) && 
        WE_WIDGET_HAS_FOCUS(msaMeAddrInputView.phoneBookHandle))
    {
        /* Launch phone book */
        msaPbhLaunchPb((WE_UINT16)msaMeAddrInputView.type);
        (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_ADDR_INPUT_DEACTIVATE);
    }
    else 
#endif
    if ((0 != msaMeAddrInputView.textInputHandle) && 
        WE_WIDGET_HAS_FOCUS(msaMeAddrInputView.textInputHandle))
    {
        if (NULL == (addr = 
            msaGetTextFromGadget(msaMeAddrInputView.textInputHandle)))
        {
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_INVALID_ADDRESS),
                MSA_DIALOG_ERROR);
            return;
        }
        /* Verify entry */
        if (MSA_ADDR_TYPE_NONE == (addrType = Msa_GetAddrType(addr)))
        {
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_INVALID_RECIP_ADDRESS), 
                MSA_DIALOG_ERROR);
            return;
        }
        /* Add item */
        if (!Msa_AddrItemInsert(&item, NULL, addrType, addr))
        {
            /*lint -e{774} */
            MSA_FREE(addr);
            (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_ADDR_INPUT_DEACTIVATE);
            return;
        }
        /*lint -e{774} */
        MSA_FREE(addr);
        /* */
        if (msaMeAddrInputView.editIndex == 0)
        {
            (void)MSA_SIGNAL_SENDTO_UP(MSA_ME_FSM, MSA_SIG_ME_ADD_RECEPIENTS, 
                msaMeAddrInputView.type, item);
        }
        else
        {
            (void)MSA_SIGNAL_SENDTO_UUP(MSA_ME_FSM, MSA_SIG_ME_CHANGE_RECEPIENT, 
                msaMeAddrInputView.type, 
                (unsigned)msaMeAddrInputView.editIndex - 1, item);
        }

        (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_ADDR_INPUT_DEACTIVATE);
    }
}

/*! \brief Creates an address field input
 *
 * \param type The address field, #MSA_ME_TO, MSA_ME_CC or #MSA_ME_BCC.
 * \param item The item to display or NULL if it is a new item.
 * \param The index of the item to edit or 0 if it is a new item.
 *****************************************************************************/
WE_BOOL msaMeCreateAddrInputView(MsaMeProperty type, const MsaAddrItem *item, 
    unsigned int editIndex)
{
    WePosition pos = {MSA_GADGET_DEF_PADDING, MSA_GADGET_DEF_PADDING};
    WePosition tmpPos;
    WeStringHandle strHandle;
    unsigned int i;
    WeSize dSize;
    WeSize size;

    if (0 != msaMeAddrInputView.winHandle)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) The address list view instance is busy!\n", __FILE__, 
            __LINE__));
        return FALSE;
    }

    memset(&msaMeAddrInputView, 0, sizeof(MsaMeAddrInputView));
    msaMeAddrInputView.type = type;
    msaMeAddrInputView.editIndex = (int)editIndex;
    /* Create form*/
    if (0 == (msaMeAddrInputView.winHandle = WE_WIDGET_FORM_CREATE(
        WE_MODID_MSA, NULL, WE_WINDOW_PROPERTY_TITLE, 0)))
    {
        return FALSE;
    }

    /* Create actions */
    if (0 == msaCreateAction(msaMeAddrInputView.winHandle, WeOk, 
        handleAddrInputOk, 0, 0, MSA_STR_ID_OK))
    {
        return FALSE;
    }
    if (0 == msaCreateAction(msaMeAddrInputView.winHandle, WeBack, 
        NULL, MSA_ME_FSM, MSA_SIG_ME_ADDR_INPUT_DEACTIVATE, MSA_STR_ID_BACK))
    {
        return FALSE;
    }
    /* String gadget to display in the input*/
    strHandle = 0;
    if ((editIndex != 0) && (NULL != item))
    {
        for(i = 0; (i != (editIndex - 1)) && (NULL != item); i++, 
            item = item->next)
            ;
        if ((NULL != item) && (item->address))
        {
            if (0 == (strHandle = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, 
                item->address, WeUtf8, (int)strlen(item->address) + 1, 
                0)))
            {
                return FALSE;
            }
        }
    }
    /* Create gadgets */
    /* Address input */
    (void)WE_WIDGET_GET_INSIDE_AREA(msaMeAddrInputView.winHandle, &tmpPos, 
        &dSize);
    if (0 == (msaMeAddrInputView.textInputHandle = 
        WE_WIDGET_TEXT_INPUT_CREATE(WE_MODID_MSA, 
        msaGetEmptyStringHandle(), strHandle, WeText, NULL, 0, 
        MSA_ME_MAX_ADDRESS_SIZE, TRUE, NULL, WE_GADGET_PROPERTY_FOCUS, 0)))
    {
        if (0 != strHandle)
        {
            (void)WE_WIDGET_RELEASE(strHandle);
        }
        return FALSE;
    }
    /* Adjust the widget width according to the display width. The height
       has already been decided by the integration */
    (void)WE_WIDGET_GET_SIZE(msaMeAddrInputView.textInputHandle, &size);
    size.width = (WE_UINT16)(dSize.width - 2*MSA_GADGET_DEF_PADDING);
    (void)WE_WIDGET_SET_SIZE(msaMeAddrInputView.textInputHandle, &size);

    if (0 != strHandle)
    {
        /* Don't need it anymore */
        (void)WE_WIDGET_RELEASE(strHandle);
    }
    
    /* Bind gadget to form */
    if (0 > WE_WIDGET_WINDOW_ADD_GADGET(msaMeAddrInputView.winHandle, 
        msaMeAddrInputView.textInputHandle, &pos)) /* Q04A */
    {
        (void)WE_WIDGET_RELEASE(msaMeAddrInputView.textInputHandle);
        return FALSE;
    } 

#ifdef MSA_CFG_USE_EXT_PB
    /* Launch phone book */
    pos.y = (WE_UINT16)(pos.y + size.height + MSA_GADGET_DEF_PADDING);

    if (0 == (msaMeAddrInputView.phoneBookHandle  = 
        WE_WIDGET_STRING_GADGET_CREATE(WE_MODID_MSA, 
        MSA_GET_STR_ID(MSA_STR_ID_PHONE_BOOK), NULL, 0, TRUE, 
        WE_GADGET_PROPERTY_FOCUS, 0)))
    {
        return FALSE;
    }
    /* Bind gadget to form */
    if (0 > WE_WIDGET_WINDOW_ADD_GADGET(msaMeAddrInputView.winHandle, 
        msaMeAddrInputView.phoneBookHandle, &pos)) /* Q04A */
    {
        return FALSE;
    } 
#endif
    /* Title */
    switch(type)
    {
    case MSA_ME_TO:
        (void)msaSetTitle(msaMeAddrInputView.winHandle, 
            MSA_GET_STR_ID(MSA_STR_ID_TO), 0);
        break;
    case MSA_ME_CC:
        (void)msaSetTitle(msaMeAddrInputView.winHandle, 
            MSA_GET_STR_ID(MSA_STR_ID_CC), 0);
        break;
    case MSA_ME_BCC:
    (void)msaSetTitle(msaMeAddrInputView.winHandle, 
            MSA_GET_STR_ID(MSA_STR_ID_BCC), 0);
        break;
    }

    /* Display form */
    return msaDisplayWindow(msaMeAddrInputView.winHandle, msaGetPosLeftTop());
}

/*! \brief
 *
 * \param items The current list
 *****************************************************************************/
WE_BOOL msaMeUpdateAddrListView(const MsaAddrItem *items)
{
    unsigned int i;
    WeStringHandle strHandle;
    if (Msa_AddrItemCount(items) > 0)
    {
        if (0 != msaMeAddrListView.winHandle)
        {
            /* Delete old items */
            while(WE_WIDGET_CHOICE_SIZE(msaMeAddrListView.winHandle) > 1)
            {
                (void)WE_WIDGET_CHOICE_REMOVE_ELEMENT(
                    msaMeAddrListView.winHandle, 1);
            }
            /* Add items to the list */
            for(i = 1; NULL != items; i++, items = items->next)
            {
                if (0 == (strHandle = addrItemToStrHandle(items)))
                {
                    return FALSE;
                }
                if (0 > WE_WIDGET_CHOICE_SET_ELEMENT(msaMeAddrListView.winHandle,
                    (int)i, 0, strHandle, 0, 0, 0, 0, 0, FALSE)) /* Q04A */
                {
                    (void)WE_WIDGET_RELEASE(strHandle);
                    return FALSE;
                }
                (void)WE_WIDGET_RELEASE(strHandle);
            }
        }
    }
    else
    {
        msaMeDeleteAddrListView();
    }
    return TRUE;
}

/*! \brief Deletes the address input view
 *
 *****************************************************************************/
void msaMeDeleteAddrInputView(void)
{
    if (0 != msaMeAddrInputView.winHandle)
    {
        (void)msaDeleteWindow(msaMeAddrInputView.winHandle);
#ifdef MSA_CFG_USE_EXT_PB
        if (0 != msaMeAddrInputView.phoneBookHandle)
        {
            (void)WE_WIDGET_RELEASE(msaMeAddrInputView.phoneBookHandle);
        }
#endif
        if (0 != msaMeAddrInputView.textInputHandle)
        {
            (void)WE_WIDGET_RELEASE(msaMeAddrInputView.textInputHandle);
        }
        memset(&msaMeAddrInputView, 0, sizeof(MsaMeAddrInputView));
    }
}

/*! \brief Handles the event where the End User pressed [Ok] in address list
 *         view.
 *
 * \param winHandle
 *****************************************************************************/
static void handleAddrListOk(WeWindowHandle winHandle)
{
    int editIndex;
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
        "(%s) (%d) Ok action received in address list view.\n", __FILE__, 
        __LINE__, winHandle));
    (void)winHandle;
    if ((editIndex = msaGetSelectedChoiceIndex(msaMeAddrListView.winHandle)) 
        >= 0)
    {
        (void)msaMeCreateAddrInputView(msaMeAddrListView.type, 
            *msaMeAddrListView.items, (unsigned)editIndex);
    }
    (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_ADDR_LIST_DEACTIVATE);
}

/*! \brief Displays a list of addresses.
 *
 * \param type The address field to change.
 * \param items A list of items to display.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaMeCreateAddrListView(MsaMeProperty type, MsaAddrItem **items)
{
    msaMeAddrListView.type  = type;
    msaMeAddrListView.items = items;

    if (0 == (msaMeAddrListView.winHandle = WE_WIDGET_MENU_CREATE(
            WE_MODID_MSA, WeImplicitChoice, NULL, NULL, 
            WE_CHOICE_ELEMENT_STRING_1, 0, 0, 0, 0))) /* Q04A */
    {
        return FALSE;
    }
    /* Q04A */
    if (!msaCreateAction(msaMeAddrListView.winHandle, WeSelect, 
        handleAddrListOk, 0, 0, MSA_STR_ID_EDIT))
    {
        return FALSE;
    }
    if (!msaCreateAction(msaMeAddrListView.winHandle, WeBack, NULL, 
        MSA_ME_FSM, MSA_SIG_ME_ADDR_LIST_DEACTIVATE, MSA_STR_ID_BACK))
    {
        return FALSE;
    }
    if (!msaCreateAction(msaMeAddrListView.winHandle, WeMenu, NULL, 
        MSA_ME_FSM, MSA_SIG_ME_ADDR_LIST_MENU_ACTIVATE, 0))
    {
        return FALSE;
    }
    /* The "add recipients item" */
    if (0 > WE_WIDGET_CHOICE_SET_ELEMENT(msaMeAddrListView.winHandle,
        0, 0, MSA_GET_STR_ID(MSA_STR_ID_ADD_RECIPIENT_QUEST), 0, 0, 0, 0, 0, 
        FALSE)) /* Q04A */
    {
        return FALSE;
    }
    /* Insert the other items */
    if (!msaMeUpdateAddrListView(*items))
    {
        return FALSE;
    }
    /* Set the title */
    (void)msaSetTitle(msaMeAddrListView.winHandle, 
        MSA_GET_STR_ID(MSA_STR_ID_RECIPIENTS), 0);

    return msaDisplayWindow(msaMeAddrListView.winHandle, msaGetPosLeftTop());
}

/*! \brief
 *
 *****************************************************************************/
void msaMeDeleteAddrListView(void)
{
    if (0 != msaMeAddrListView.winHandle)
    {
        (void)msaDeleteWindow(msaMeAddrListView.winHandle);
        msaMeAddrListView.winHandle = 0;
    }
}

/*! \brief Action-handler for the address list navigation menu.
 *
 * \param action The type of action that was generated.
 *****************************************************************************/
static void addrListNavMenuHandler(MsaNavAction action)
{
    if (MSA_NAV_ACTION_OK == action)
    {
        if (MSA_ME_ADDR_LIST_DELETE == msaGetNavMenuItem())
        {
            /* Delete the current item */
            (void)MSA_SIGNAL_SENDTO_UU(MSA_ME_FSM, MSA_SIG_ME_DELETE_ADDR_ITEM,
                msaMeAddrListView.type, 
                (unsigned)(msaGetSelectedChoiceIndex(
                msaMeAddrListView.winHandle) - 1));
            (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, 
                MSA_SIG_ME_ADDR_LIST_MENU_DEACTIVATE);
        }
        else if (MSA_ME_ADDR_LIST_BACK == msaGetNavMenuItem())
        {
            /* Deactivate all */
            (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, 
                MSA_SIG_ME_ADDR_LIST_DEACTIVATE);
            (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, 
                MSA_SIG_ME_ADDR_LIST_MENU_DEACTIVATE);
        }
    }
    else if (MSA_NAV_ACTION_BACK == action)
    {
        (void)MSA_SIGNAL_SENDTO(MSA_ME_FSM, MSA_SIG_ME_ADDR_LIST_MENU_DEACTIVATE);
    }
}

/*! \brief Creates a navigation menu for the address list view
 *
 *****************************************************************************/
WE_BOOL msaMeCreateAddrListMenu(void)
{
    if (!msaCreateNavMenu(msaGetEmptyStringHandle(), addrListNavMenuHandler))
    {
        return FALSE;
    }
    else if (!msaAddNavMenuItem(MSA_GET_STR_ID(MSA_STR_ID_DELETE), 
        MSA_ME_ADDR_LIST_DELETE))
    {
        return FALSE;
    }
    else if (!msaAddNavMenuItem(MSA_GET_STR_ID(MSA_STR_ID_BACK), 
        MSA_ME_ADDR_LIST_BACK))
    {
        return FALSE;
    }
    else if (!msaShowNavMenu())
    {
        return FALSE;
    }
    return TRUE;
}

/*! \brief Deletes the navigation menu for the address list view
 *
 *****************************************************************************/
void msaMeDeleteAddrListMenu(void)
{
    msaRemoveNavMenu();
}

/*!
 * \brief Returns 
 * \return TRUE if the property view is active
 *****************************************************************************/
WE_BOOL msaUiMeIsPropertyViewActive(void)
{
    return (0 != msaMePropView.formHandle);
} /* msaUiMeIsPropertyViewActive */

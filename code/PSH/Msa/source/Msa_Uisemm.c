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

/* !\file mauisemm.c
 *  \brief The main menu for the SMIL editor.
 */

/* WE */
#include "We_Mem.h"
#include "We_Wid.h"
#include "We_Int.h"
#include "We_Lib.h"
#include "We_Mem.h"

/* MMS */
#include "Mms_Def.h"

/* MSA */
#include "Msa_Def.h"
#include "Msa_Types.h"
#include "Msa_Cfg.h"
#include "Msa_Rc.h"
#include "Msa_Intsig.h"
#include "Msa_Se.h"
#include "Msa_Mr.h"
#include "Msa_Me.h"
#include "Msa_Uise.h"
#include "Msa_Uimenu.h"
#include "Msa_Uicmn.h"
#include "Msa_Uiform.h"
#include "Msa_Uisemm.h"

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static void navMenuSignalConverter(MsaNavAction actionType);
static int createMenuItems(int slideCount, int initiated, MsaMeStartupMode mode, 
    MmsFileType type);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Signal converter callback function for the context sensitive menu.
 *
 * \param actionType The type of action that was generated.
 *****************************************************************************/
static void navMenuSignalConverter(MsaNavAction actionType)
{
    MsaInputItem inputItem;
    char buffer[MSA_CFG_DURATION_SIZE + 1];

    memset(&inputItem, 0, sizeof(MsaInputItem));
    if (MSA_NAV_ACTION_BACK == actionType)
    {
        (void)MSA_SIGNAL_SENDTO(MSA_SE_FSM, MSA_SIG_SE_NAV_MENU_DEACTIVATE);
    }

    if (MSA_NAV_ACTION_OK == actionType)
    {
        switch(msaGetNavMenuItem())
        {
        case MSA_SE_DELETE_SLIDE:
            (void)MSA_SIGNAL_SENDTO(MSA_SE_FSM, MSA_SIG_SE_DELETE_SLIDE);
            break;
        case MSA_SE_INSERT_PAGE:
            (void)MSA_SIGNAL_SENDTO(MSA_SE_FSM, MSA_SIG_SE_INSERT_SLIDE);
            break;
        case MSA_SE_TEXT_FG_COLOR:
            (void)MSA_SIGNAL_SENDTO(MSA_SE_FSM, MSA_SIG_SE_SET_TEXT_COLOR);
            break;
        case MSA_SE_TEXT_FG_COLOR_ALL:
            (void)MSA_SIGNAL_SENDTO(MSA_SE_FSM, MSA_SIG_SE_SET_TEXT_COLOR_ALL);
            break;
        case MSA_SE_TEXT_BG_COLOR:
            break;
        case MSA_SE_ITEM_TIMING:
            (void)MSA_SIGNAL_SENDTO_U(MSA_SE_FSM, MSA_SIG_SE_SET_ITEM_TIMING, 
                msaSeGetActiveObjType());
            break;
        case MSA_SE_DELETE_OBJECT:
            if (MSA_MEDIA_GROUP_NONE != msaSeGetActiveObjType())
            {
                (void)MSA_SIGNAL_SENDTO_U(MSA_SE_FSM, MSA_SIG_SE_REM_OBJECT, 
                    msaSeGetActiveObjType());
            }
            break;        
        case MSA_SE_SAVE:
            (void)MSA_SIGNAL_SENDTO_U(MSA_SE_FSM, MSA_SIG_SE_SAVE, 0);
            break;
        case MSA_SE_SAVE_AS_TEMPLATE:
            (void)MSA_SIGNAL_SENDTO(MSA_SE_FSM, MSA_SIG_SE_SAVE_AS_TEMPLATE);
            break;
        case MSA_SE_SAVE_TEMPLATE:
            (void)MSA_SIGNAL_SENDTO(MSA_SE_FSM, MSA_SIG_SE_SAVE_TEMPLATE);
            break;
        case MSA_SE_SEND:
            (void)MSA_SIGNAL_SENDTO(MSA_SE_FSM, MSA_SIG_SE_SEND);
            break;
        case MSA_SE_PREVIEW_SMIL:
            (void)MSA_SIGNAL_SENDTO(MSA_SE_FSM, MSA_SIG_SE_PREVIEW_SMIL);
            break;
        case MSA_SE_DURATION:
            inputItem.type = MSA_SINGLE_LINE_INPUT;
            inputItem.textType = WeNumeric;
            inputItem.maxTextSize = MSA_CFG_DURATION_SIZE;
            sprintf(buffer,"%d", msaSeGetDuration()/1000);
            inputItem.text = buffer;
            inputItem.callback = msaSeHandleDuration;
            (void)msaCreateInputForm(&inputItem, 
                MSA_GET_STR_ID(MSA_STR_ID_DURATION));
            break;
        case MSA_SE_BACK:
            (void)MSA_SIGNAL_SENDTO(MSA_SE_FSM, MSA_SIG_SE_DEACTIVATE);
            break;
        case MSA_SE_BG_COLOR:
            (void)MSA_SIGNAL_SENDTO(MSA_SE_FSM, MSA_SIG_SE_SET_BG_COLOR);
            break;
        }
        (void)MSA_SIGNAL_SENDTO(MSA_SE_FSM, MSA_SIG_SE_NAV_MENU_DEACTIVATE);
    }
} /* navMenuSignalConverter */

/*!
 * \brief Creates menu items for the navigation menu for the SE.
 *
 * \param slideCount The total number of slides.
 * \param initiated TRUE if the current slide has any media objects attached,
 *                  otherwise FALSE.
 * \param mode The current startup mode, see #MsaMeStartupMode.
 * \return TRUE on OK else FALSE
 *****************************************************************************/
static int createMenuItems(int slideCount, int initiated, MsaMeStartupMode mode,
    MmsFileType type)
{
    MsaSeGadgetDef gadget;

    gadget = msaSeGetActiveGadget();
    /* Text color */
    if (MSA_SE_GADGET_TEXT == msaSeGetActiveGadget())
    {
        if (!msaAddNavMenuItem(MSA_GET_STR_ID(MSA_STR_ID_TEXT_COLOR),
            MSA_SE_TEXT_FG_COLOR))
        {
            return FALSE;
        }
    }

     /* Text color All */
    if (MSA_SE_GADGET_TEXT == msaSeGetActiveGadget())
    {
        if (!msaAddNavMenuItem(MSA_GET_STR_ID(MSA_STR_ID_TEXT_COLOR_ALL),
            MSA_SE_TEXT_FG_COLOR_ALL))
        {
            return FALSE;
        }
    }



    /* Delete object */
    if ((gadget == MSA_SE_GADGET_IMAGE) || (gadget == MSA_SE_GADGET_TEXT) ||
        (gadget == MSA_SE_GADGET_AUDIO))
    {
        if (!msaAddNavMenuItem(MSA_GET_STR_ID(MSA_STR_ID_REM_MEDIA_OBJ),
            MSA_SE_DELETE_OBJECT))
        {
            return FALSE;
        }
    }
    /* Insert page */
    if (initiated)
    {
        if (!msaAddNavMenuItem(MSA_GET_STR_ID(MSA_STR_ID_INSERT_SLIDE),
            MSA_SE_INSERT_PAGE))
        {
            return FALSE;
        }
    }
    /* Delete slide*/
    if (slideCount > 1)
    {
        if (!msaAddNavMenuItem(MSA_GET_STR_ID(MSA_STR_ID_DELETE_SLIDE),
            MSA_SE_DELETE_SLIDE))
        {
            return FALSE;
        }
    }
    /* Send message */
    if ((slideCount > 0) && (initiated) && MSA_ME_ALLOW_SEND(mode, type))
    {
        if (!msaAddNavMenuItem(MSA_GET_STR_ID(MSA_STR_ID_SEND), MSA_SE_SEND))
        {
            return FALSE;
        }
    }

     /* Save message */
    if ((slideCount > 0) && (initiated))
    {
        if (!msaAddNavMenuItem(MSA_GET_STR_ID(MSA_STR_ID_SAVE), 
            (MSA_ME_STARTUP_NEW_TEMPLATE == mode) ? MSA_SE_SAVE_AS_TEMPLATE: 
             MSA_SE_SAVE))
        {
            return FALSE;
        }
    }
    /* Save message as template, note that new templates uses "save" */
    if ((slideCount > 0) && (initiated) && MSA_ME_ALLOW_SAVE_AS_TEMPLATE(mode))
    {
        if (!msaAddNavMenuItem(MSA_GET_STR_ID(MSA_STR_ID_SAVE_AS_TEMPLATE), 
            MSA_SE_SAVE_AS_TEMPLATE))
        {
            return FALSE;
        }
    }
    /* Save template, note that new templates uses "save" */
    if ((slideCount > 0) && (initiated) && MSA_ME_ALLOW_SAVE_TEMPLATE(mode))
    {
        if (!msaAddNavMenuItem(MSA_GET_STR_ID(MSA_STR_ID_SAVE_TEMPLATE), 
            MSA_SE_SAVE_TEMPLATE))
        {
            return FALSE;
        }
    }

    /* Preview SMIL */
    if ((slideCount > 0) && (initiated))
    {
        if (!msaAddNavMenuItem(MSA_GET_STR_ID(MSA_STR_ID_PREVIEW_SMIL), 
            MSA_SE_PREVIEW_SMIL))
        {
            return FALSE;
        }
    }
    /* Duration */    
    if (!msaAddNavMenuItem(MSA_GET_STR_ID(MSA_STR_ID_DURATION), 
        MSA_SE_DURATION))
    {
        return FALSE;
    }
    /* Item timing */
    if ((slideCount > 0) && (initiated) && 
        (MSA_SE_GADGET_AUDIO == msaSeGetActiveGadget() || 
         MSA_SE_GADGET_TEXT == msaSeGetActiveGadget() || 
         MSA_SE_GADGET_IMAGE == msaSeGetActiveGadget()))
    {
        if (!msaAddNavMenuItem(MSA_GET_STR_ID(MSA_STR_ID_ITEM_TIMING),
            MSA_SE_ITEM_TIMING))
        {
            return FALSE;
        }
    }
    /* Background color */
    if (!msaAddNavMenuItem(MSA_GET_STR_ID(MSA_STR_ID_BG_COLOR), 
        MSA_SE_BG_COLOR))
    {
        return FALSE;
    }
    /* Back*/    
    if (!msaAddNavMenuItem(MSA_GET_STR_ID(MSA_STR_ID_BACK), 
        MSA_SE_BACK))
    {
        return FALSE;
    }
    return TRUE;
} /* createMenuItems */

/*!
 * \brief Creates a navigation menu for the SE.
 *
 * \param slideNo The index of the current slide.
 * \param slideCount The total number of slides.
 * \param initiated TRUE if the menu was created successfully, otherwise 
 *                  FALSE.
 * \param mode The current startup mode, see #MsaMeStartupMode.
 * \return TRUE on OK else FALSE
 *****************************************************************************/
WE_BOOL msaSeCreateNavMenu(int slideCount, int initiated,
    MsaMeStartupMode mode, MmsFileType type)
{
    /* The menu is dependant on which object that is currently active */
    msaSeSetActiveObj();
    
    /* Create navigation menu */
    if (!msaCreateNavMenu(0, navMenuSignalConverter))
    {
        msaRemoveNavMenu();
        return FALSE;
    }else if (!createMenuItems(slideCount, initiated, mode, type))
    {
        msaRemoveNavMenu();
        return FALSE;
    }else if (!msaShowNavMenu())
    {
        msaRemoveNavMenu();
        return FALSE;
    }
    return TRUE;
} /* msaSeCreateNavMenu */

/*!
 * \brief Deletes the navigation menu.
 *****************************************************************************/
void msaSeDeleteNavMenu(void)
{
    msaRemoveNavMenu();
} /* msaSeDeleteNavMenu */

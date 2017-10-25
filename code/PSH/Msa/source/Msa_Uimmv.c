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

/* !\file mauimmv.c
 */

/* WE */
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Wid.h"
#include "We_Act.h"
#include "We_Mem.h"

/* MMS */
#include "Mms_Cfg.h"
#include "Mms_Def.h"

/* SIS */
#include "Sis_Cfg.h"
#include "Sis_If.h"

/* MSA */
#include "Msa_Rc.h"
#include "Msa_Cfg.h"
#include "Msa_Types.h"
#include "Msa_Intsig.h"
#include "Msa_Utils.h"
#include "Msa_Uicmn.h"
#include "Msa_Uimenu.h"
#include "Msa_Mmv.h"
#include "Msa_Uimmv.h"
#include "Msa_Mob.h"
#include "Msa_Mem.h"


/******************************************************************************
 * Data-structures
 *****************************************************************************/

/*!\struct MsaMvPropView
 *  \brief Message property viewer widget handles 
 */
typedef struct
{
    WE_UINT32  audioHandle;
}MsaUiMmvWindow;

/*!\struct MsaMmvMenu
 *	
 */
typedef struct 
{
    WE_UINT32              actionCount;    /* The number of object actions menu entries */
}MsaMmvMenu;

/******************************************************************************
 * Variables
 *****************************************************************************/

static MsaUiMmvWindow msaUiMmvWindow;
static MsaMmvMenu     *msaMmvMenu;

/******************************************************************************
 * Prototypes
 *****************************************************************************/

static WE_BOOL createMenuItems(const we_act_action_entry_t *actions, 
    WE_UINT32 count);


/*!
 * \brief Signal converter for the navigation menu.
 * 
 * \param actionType The type of action, MSA_NAV_ACTION_BACK or 
 *  MSA_NAV_ACTION_OK.
 *****************************************************************************/
static void navMenuSignalConverter(MsaNavAction actionType)
{
    if (actionType == MSA_NAV_ACTION_BACK)
    {
        (void)MSA_SIGNAL_SENDTO(MSA_MMV_FSM, MSA_SIG_MMV_NAV_DEACTIVATE);
        return;
    }
    if (actionType == MSA_NAV_ACTION_OK)
    {
        if ((unsigned int)msaGetNavMenuItem() < msaMmvMenu->actionCount)
        {
            /* Handle object actions */
            (void)MSA_SIGNAL_SENDTO_U(MSA_MMV_FSM, MSA_SIG_MMV_OBJ_ACTION, 
                msaGetNavMenuItem());
        }
        else
        {
            /* Handle the "back" alternative */
            /* Delete menu */
            (void)MSA_SIGNAL_SENDTO(MSA_MMV_FSM, MSA_SIG_MMV_NAV_DEACTIVATE);
            /* Stop MMV */
            (void)MSA_SIGNAL_SENDTO(MSA_MMV_FSM, MSA_SIG_MMV_DEACTIVATE);
            /* Stop MOB */
            (void)MSA_SIGNAL_SENDTO(MSA_MOB_FSM, MSA_SIG_MOB_DEACTIVATE);
        }
    }
}            

/*!
 * \brief Create and view attachment menu 
 * \brief mo The current media object
 * 
 * \return TRUE upon success
 *****************************************************************************/
static WE_BOOL createMenuItems(const we_act_action_entry_t *actions, 
    WE_UINT32 count)
{
    unsigned int i;
    for (i = 0; i < count; i++)
    {
        if (!msaAddNavMenuItem(MSA_GET_STR_ID(actions[i].string_id), i))
        {
            return FALSE;
        }
    }

    /* Back */
    if (!msaAddNavMenuItem(MSA_GET_STR_ID(MSA_STR_ID_BACK), i))
    {
        return FALSE;
    }      

    return TRUE;
}

/*!
 * \brief Stop playing audio
 *****************************************************************************/
void msaUiMmvStopAudio(void)
{
    if (msaUiMmvWindow.audioHandle)
    {
        (void)WE_WIDGET_SOUND_STOP(msaUiMmvWindow.audioHandle); 
        (void)WE_WIDGET_RELEASE(msaUiMmvWindow.audioHandle);
    }
    msaUiMmvWindow.audioHandle = 0;
}

/*!
 * \brief Check if audio widget is allocated/active
 * \return TRUE if active
 *****************************************************************************/
WE_BOOL msaUiMmvAudioActive(void)
{
    return msaUiMmvWindow.audioHandle > 0;
}

/*!
 * \brief Create audio widget and play audio
 * \param data  audio data 
 * \param type  media type
 * \param size  audio data size
 * \return TRUE upon success
 *****************************************************************************/
WE_BOOL msaUiMmvPlayAudio(const MsaMoDataItem *mo)
{
    WeDirectData directData;
    WeCreateData createData;
    createData.directData = &directData;
    if (msaUiMmvWindow.audioHandle)
    {
        msaUiMmvStopAudio();
    }

    directData.data = (const char*)mo->data;
    directData.dataSize = (int)mo->size;
    msaUiMmvWindow.audioHandle = WE_WIDGET_SOUND_CREATE(WE_MODID_MSA,
        &createData, (char *)mo->type->strValue, WeResourceBuffer);

    if (!msaUiMmvWindow.audioHandle)
    {
        return FALSE;
    }

    /* nothing to do if sound is unable to play */
    (void)WE_WIDGET_SOUND_PLAY(msaUiMmvWindow.audioHandle);

    return TRUE;
}

/*!
 * \brief Create and view attachment menu 
 * \return TRUE upon success
 *****************************************************************************/
WE_BOOL msaUiMmvCreateMenu(const we_act_action_entry_t  *actions, 
    WE_UINT32 actionCount)
{
    /* Allocate data for the structure */
    MSA_CALLOC(msaMmvMenu, sizeof(MsaMmvMenu));

    msaMmvMenu->actionCount = actionCount;

    if (!msaCreateNavMenu(0, navMenuSignalConverter))
    {
        msaRemoveNavMenu();
        return FALSE;  
    }
    else if (!createMenuItems(actions, actionCount))
    {
        msaRemoveNavMenu();
        return FALSE;
    }
    else if (!msaShowNavMenu())
    {
        msaRemoveNavMenu();
        return FALSE;
    }

    return TRUE;
}

/*!
 * \brief Deletes the navigation menu for the multipart/mixed msg viewer.
 *
 *****************************************************************************/
void msaUiMmvDeleteMenu(void)
{
    MSA_FREE(msaMmvMenu);
    msaMmvMenu = NULL;
    msaRemoveNavMenu();
}

/*!
 * \brief Initiates the data-structures for the multipart/mixed message viewer.
 *****************************************************************************/
void msaMmvUiInit(void)
{
    memset(&msaUiMmvWindow, 0, sizeof(MsaUiMmvWindow));
}

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

/*!\file mauimenu.h
 * \brief Common functions for the UI.
 */

#ifndef _MAUIMENU_H_
#define _MAUIMENU_H_

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! \enum MsaNavAction
 *  Actions related to the context dependent menu */
typedef enum
{
    MSA_NAV_ACTION_OK,      /* !< The End User presses ok */
    MSA_NAV_ACTION_BACK     /* !< The End User presses back */
}MsaNavAction;


/******************************************************************************
 * Data-types
 *****************************************************************************/
typedef void (*MsaNavCallBack)(MsaNavAction actionType);
typedef WE_BOOL (*MsaNavVisibilityCallBack)(int entryId);

/*! \struct MsaNavMenuItems
 *  \brief Holds all needed data for a navigation menu entry
 */
typedef struct {
    int             entryId;/*!< Unique wid for this menu entry */
    WE_UINT32      strId;  /*!< String to display for menu item */
    MsaStateMachine fsm;    /*!< Callback FSM */
    int             sig;    /*!< Callback signal */
    MsaNavVisibilityCallBack visibilityCallBack; /*!< Callback function used to
                                                      decide whether to show 
                                                      this menu entry */
}MsaNavMenuItem;

/******************************************************************************
 * Prototypes
 *****************************************************************************/

void msaInitNavMenu(void);
void msaTerminateNavMenu(void);

WE_BOOL msaCreateNavMenu(WeStringHandle titleStrHandle, 
    MsaNavCallBack signalHandler);
int msaAddNavMenuItem(WeStringHandle itemStrId, int contextValue);
WE_BOOL msaAddNavMenuItemNew(const MsaNavMenuItem *navItem);
int msaGetNavMenuItem(void);
MsaNavMenuItem msaGetNavMenuItemNew(void);
WE_BOOL msaShowNavMenu(void);
void msaRemoveNavMenu(void);

#endif

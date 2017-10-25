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

/*!\file mauimenu.c
 * \brief Context sensitive menu handling.
 */

/* WE */
#include "We_Wid.h"
#include "We_Log.h"
#include "We_Lib.h"

/* MMS */
#include "Mms_Def.h"

/* MSA */
#include "Msa_Rc.h"
#include "Msa_Def.h"
#include "Msa_Types.h"
#include "Msa_Intsig.h"
#include "Msa_Uicmn.h"
#include "Msa_Uimenu.h"
#include "Msa_Uisig.h"

/******************************************************************************
 * Constants
 *****************************************************************************/

 /*! Maximum number of items in a context dependent menu.
 */
#define MSA_MAX_CONTEXT_LIST_ITEMS      16

/******************************************************************************
 * Data-types
 *****************************************************************************/

typedef enum
{
    MSA_NAV_MENU_TYPE_ORIGINAL,
    MSA_NAV_MENU_TYPE_NEW
}MsaNavMenuEntryType;


/*! \struct MsaNavMenu
 *  Data-structure for a context sensitive menu */
typedef struct
{
    WeWindowHandle menuHandle;
    /*! List to keep track of which items that are presented in the list */
    MsaNavMenuEntryType type[MSA_MAX_CONTEXT_LIST_ITEMS];
    union 
    {
        int             listItems[MSA_MAX_CONTEXT_LIST_ITEMS];
        MsaNavMenuItem  listItemsNew[MSA_MAX_CONTEXT_LIST_ITEMS];
    }item;
    int             listIndex;
    /*! Destination signal */
    MsaNavCallBack  signalHandler;
}MsaNavMenu;

/******************************************************************************
 * Prototypes
 *****************************************************************************/

/******************************************************************************
 * Global variables
 *****************************************************************************/
static MsaNavMenu        msaNavMenu;

/******************************************************************************
 * Function implementations
 *****************************************************************************/
/*!
 * \brief Initiates data structures for the navigation menu.
 *
 *****************************************************************************/
void msaInitNavMenu(void)
{
    memset(&msaNavMenu, 0, sizeof(MsaNavMenu));
}

/*!
 * \brief Deallocates all resources used by the navigation menu.
 *
 *****************************************************************************/
void msaTerminateNavMenu(void)
{
    /* Terminate just in case */
    msaRemoveNavMenu();
    /* Clear obsolete data */
    memset(&msaNavMenu, 0, sizeof(MsaNavMenu));
}

/*! \brief Handles [Ok] action for the navigation menu.
 *
 *  \param winHandle The window that generated the action.
 *****************************************************************************/
static void handleOkAction(WeWindowHandle winHandle)
{
    if (winHandle != msaNavMenu.menuHandle)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) erroneous window handle in event\n", __FILE__, 
            __LINE__));
    }
    if (NULL != msaNavMenu.signalHandler)
    {
        msaNavMenu.signalHandler(MSA_NAV_ACTION_OK);
    }
}

/*! \brief Handles [Back] action for the navigation menu.
 *
 *  \param winHandle The window that generated the action.
 *****************************************************************************/
static void handleBackAction(WeWindowHandle winHandle)
{
    if (winHandle != msaNavMenu.menuHandle)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) erroneous window handle in event\n", __FILE__, 
            __LINE__));
    }
    if (NULL != msaNavMenu.signalHandler)
    {
        msaNavMenu.signalHandler(MSA_NAV_ACTION_BACK);
    }
}

/*!\brief Creates the data-structures for a context sensitive menu.
 *
 * \param titleStrHandle The title of the menu, if 0 no title is displayed.
 * \param signalHandler The navigation callback function 
 *****************************************************************************/
WE_BOOL msaCreateNavMenu(WeStringHandle titleStrHandle, 
    MsaNavCallBack signalHandler)
{
    if (0 != msaNavMenu.menuHandle)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) instance is busy!\n", __FILE__, __LINE__));
        return FALSE;
    }

    memset(&msaNavMenu, 0, sizeof(MsaNavMenu));
    /* Set destination signal converter */
    msaNavMenu.signalHandler = signalHandler;
    /* Create the menu */
    if (0 == (msaNavMenu.menuHandle = WE_WIDGET_MENU_CREATE(WE_MODID_MSA,
        WeImplicitChoice, 0, 0, WE_CHOICE_ELEMENT_STRING_1, 0, 0, 0, 0))) /* Q04A */
    {
        return FALSE;
    }
    /* Actions */

    /* Q04A */
    if (!msaCreateAction(msaNavMenu.menuHandle, WeSelect, handleOkAction, 0,
        0, MSA_STR_ID_OK))
    {
        return FALSE;
    }
    
    if (!msaCreateAction(msaNavMenu.menuHandle, WeBack, handleBackAction, 0, 
        0, MSA_STR_ID_BACK))
    {
        return FALSE;
    }

    /* Add title only if titleStrId is valid */
    if (0 != titleStrHandle)
    {
        (void)msaSetTitle(msaNavMenu.menuHandle, titleStrHandle, 0);
    }

    return TRUE;
}

/*!\brief Adds a item at the end of a previously created menu.
 *
 * \param itemStrHandle The string that are displayed to the user.
 * \param contextValue A value that will be returned when msaGetNavMenuItem is
 *                    called.
 * \return TRUE if the item is generated successfully, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaAddNavMenuItem(WeStringHandle itemStrHandle, int contextValue)
{
    if (msaNavMenu.listIndex >= MSA_MAX_CONTEXT_LIST_ITEMS)
    {
        return FALSE;
    }
    if (0 > WE_WIDGET_CHOICE_SET_ELEMENT(msaNavMenu.menuHandle, 
        msaNavMenu.listIndex, 0, itemStrHandle, 0, 0, 0, 0, 0, FALSE)) /* Q04A */
    {
        return FALSE;
    }
    msaNavMenu.type[msaNavMenu.listIndex] = MSA_NAV_MENU_TYPE_ORIGINAL;
    msaNavMenu.item.listItems[msaNavMenu.listIndex] = contextValue;
    msaNavMenu.listIndex++;
    return TRUE;
}

/*!\brief Adds a item at the end of a previously created menu, extended version
 *
 * \param navItem All data for the menu item to be created
 * \return TRUE if the item is generated successfully, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaAddNavMenuItemNew(const MsaNavMenuItem *navItem)
{
    if (msaNavMenu.listIndex >= MSA_MAX_CONTEXT_LIST_ITEMS) 
    {
        return FALSE;
    }

    if ((NULL != navItem->visibilityCallBack) &&
        (FALSE == (navItem->visibilityCallBack)(navItem->entryId)))
    {
        return TRUE;
    } /* if */

    if (0 > WE_WIDGET_CHOICE_SET_ELEMENT(msaNavMenu.menuHandle, 
        msaNavMenu.listIndex, 0, MSA_GET_STR_ID(navItem->strId), 0, 0, 0, 0, 
        0, FALSE)) /* Q04A */
    {
        return FALSE;
    }
    msaNavMenu.type[msaNavMenu.listIndex] = MSA_NAV_MENU_TYPE_NEW;
    msaNavMenu.item.listItemsNew[msaNavMenu.listIndex] = *navItem;
    msaNavMenu.listIndex++;
    return TRUE;
}

/*!\brief Returns the selected item in a menu.
 *
 * \return The item that is currently selected or -1 if no selected item could
 *         be extracted.
 *****************************************************************************/
int msaGetNavMenuItem()
{
    int index;
    if (0 == msaNavMenu.menuHandle)
    {
        return -1;
    }
    index = msaGetSelectedChoiceIndex(msaNavMenu.menuHandle);
    if ((index < 0) || (index >= MSA_MAX_CONTEXT_LIST_ITEMS))
    {
        return -1;
    }
    /*
     *	Insert code here for checking item type and returning the correct item
     */
    return msaNavMenu.item.listItems[index];
}

/*!
 * \brief   Returns the selected item in a menu - with extended information.
 *          msaAddNavMenuItemNew() must've been used instead of 
 *          msaAddNavMenuItem()!
 * 
 * \return  The item that is currently selected or {0,0,0,0} if no selected 
 *          item could be extracted.
 *****************************************************************************/
MsaNavMenuItem msaGetNavMenuItemNew(void)
{
    MsaNavMenuItem error = {0};
    int index;

    if (0 == msaNavMenu.menuHandle)
    {
        return error;
    }
    index = msaGetSelectedChoiceIndex(msaNavMenu.menuHandle);
    if ((index < 0) || (index >= MSA_MAX_CONTEXT_LIST_ITEMS))
    {
        return error;
    }
    return msaNavMenu.item.listItemsNew[index];
} /* msaGetNavMenuItemNew */

/*!\brief Displayes a previously created menu.
 *
 * \return TRUE if the menu could be displayed. otherwise FALSE.
 *
 *****************************************************************************/
WE_BOOL msaShowNavMenu()
{
    if (0 == msaNavMenu.menuHandle)
    {
        return FALSE;
    }
    return msaDisplayWindow(msaNavMenu.menuHandle, msaGetPosLeftTop());
}

/*!\brief Removes a previously created navigation menu.
 *
 *****************************************************************************/
void msaRemoveNavMenu()
{
    /* Deregister window and remove actions */
    (void)msaDeleteWindow(msaNavMenu.menuHandle);
    /* Delete invalid handles and list item data */
    memset(&msaNavMenu, 0, sizeof(msaNavMenu));
}

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

/* WE */
#include "We_Wid.h"
#include "We_Lib.h"
#include "We_Mem.h"


/* SIS */
#include "Sis_cfg.h"
#include "Sis_if.h"
#include "Sis_def.h"

/* SMA */
#include "Sia_rc.h"
#include "Sia_cfg.h"
#include "Sia_def.h"
#include "Saintsig.h"
#include "Sauisig.h"
#include "Saintsig.h"
#include "Satypes.h"
#include "Sauiform.h"
#include "Saui.h"
#include "Samenu.h"
#include "Samem.h"

/******************************************************************************
 * Constants
 *****************************************************************************/
#define IS_SET(flags, flag)   (((flags) & (flag)) == (flag))

/******************************************************************************
 * Data-types
 *****************************************************************************/

typedef enum
{
    SIA_MENU_TYPE_ORIGINAL,
    SIA_MENU_TYPE_NEW
}SiaMenuEntryType;


/*! \struct SiaMenu
 *  Data-structure for a context sensitive menu */
typedef struct
{
    WeWindowHandle handle;
    /*! List to keep track of which items that are presented in the list */
    SiaMenuEntryType type[SIA_MAX_CONTEXT_LIST_ITEMS];
    union 
    {
        int             listItems[SIA_MAX_CONTEXT_LIST_ITEMS];
        SiaMenuItem  listItemsNew[SIA_MAX_CONTEXT_LIST_ITEMS];
    }item;
    int             listIndex;
    /*! Destination signal */
    SiaCallBack  signalHandler;
}SiaMenu;

typedef struct 
{
    int          handle;        /* handle, negative if not used */
    SiaMenu  *navMenuPtr;
} MenuInstances;

/******************************************************************************
 * Global variables
 *****************************************************************************/
static MenuInstances         navMenuInstances[SIA_MAX_MENUS + 1];


/******************************************************************************
 * Prototypes
 *****************************************************************************/
static SiaMenu *getMenuInstanceByWindowHandle(WeWindowHandle handle);
static SiaMenuHandle createMenuInstance(SiaMenu **menu);
static SiaMenu *getMenuInstanceByHandle(SiaMenuHandle handle);
static void removeMenuInstance(SiaMenuHandle handle);

/******************************************************************************
 * Function implementations
 *****************************************************************************/
/*!
 * \brief Initiates data structures for the navigation menu.
 *
 *****************************************************************************/
void smaInitMenu(void)
{
    memset(navMenuInstances, 0, (sizeof(MenuInstances) * SIA_MAX_MENUS + 1));
}

/*!
 * \brief Deallocates all resources used by the navigation menu.
 *
 *****************************************************************************/
void smaTerminateMenu(void)
{
    int i;
    
    for (i=0; i < SIA_MAX_MENUS; i++)
    {
        if (navMenuInstances[i].navMenuPtr != NULL)
        {
            smaRemoveMenu((SiaMenuHandle)i);
        } /* if */
    }
    memset(navMenuInstances, 0, (sizeof(MenuInstances) * SIA_MAX_MENUS + 1));
}

/*! \brief Handles [Ok] action for the navigation menu.
 *
 *  \param winHandle The window that generated the action.
 *****************************************************************************/
static void handleOkAction(WeWindowHandle winHandle)
{
    SiaMenu *menu;

    menu = getMenuInstanceByWindowHandle(winHandle);

    if (NULL != menu && menu->signalHandler != NULL)
    {
        menu->signalHandler(SIA_MENU_ACTION_OK);
    }
}

/*! \brief Handles [Back] action for the navigation menu.
 *
 *  \param winHandle The window that generated the action.
 *****************************************************************************/
static void handleBackAction(WeWindowHandle winHandle)
{
    SiaMenu *menu;

    menu = getMenuInstanceByWindowHandle(winHandle);

    if (NULL != menu && menu->signalHandler != NULL)
    {
        menu->signalHandler(SIA_MENU_ACTION_BACK);
    }
}


/*! \brief Handles [Back] action for the navigation menu.
 *
 *  \param winHandle The window that generated the action.
 *****************************************************************************/
static void handleMenuAction(WeWindowHandle winHandle)
{
    SiaMenu *menu;

    menu = getMenuInstanceByWindowHandle(winHandle);

    if (NULL != menu && menu->signalHandler != NULL)
    {
        menu->signalHandler(SIA_MENU_ACTION_MENU);
    }
}

/*!\brief Creates the data-structures for a context sensitive menu.
 *
 * \param titleStrHandle The title of the menu, if 0 no title is displayed.
 * \param signalHandler The navigation callback function 
 *****************************************************************************/
SiaMenuHandle smaCreateMenu(WeStringHandle titleStrHandle, 
    SiaCallBack signalHandler, int actions, int showAttributes, 
    SiaMenuHandle parent, WE_UINT32 parentIndex)
{
    SiaMenu *menu;
    SiaMenu *parentMenu;
    WeWindowHandle parentHandle = 0;
    SiaMenuHandle handle;
    
    if (0 == (handle = createMenuInstance(&menu)))
    {
        return (SiaSlideFormHandle)0;
    } /* if */

    parentMenu = getMenuInstanceByHandle(parent);
    
    if (parentMenu != NULL)
    {
        parentHandle = parentMenu->handle; 
    } 
    else
    {
        parentHandle = 0;
        parentIndex = 0;
    } /* if */


    memset(menu, 0, sizeof(SiaMenu));
    /* Set destination signal converter */
    menu->signalHandler = signalHandler;

    /* Create the menu */
    if (0 == (menu->handle = WE_WIDGET_MENU_CREATE(WE_MODID_SIA,
        WeImplicitChoice, 0, 0, showAttributes, parentHandle, 
        (int)parentIndex, 0, 0))) 
    {
        return 0;
    }

    /* Actions */
	if (IS_SET(actions, SIA_MENU_ACTION_OK))
	{
		if (!smaCreateAction(menu->handle, WeSelect, handleOkAction, 0,
			0, SIA_STR_ID_ACTION_OK))
		{
			return 0;
		}
	}
    
	if (IS_SET(actions, SIA_MENU_ACTION_BACK))
	{
		if (!smaCreateAction(menu->handle, WeBack, handleBackAction, 0, 
			0, SIA_STR_ID_ACTION_BACK))
		{
			return 0;
		}
	}
	if (IS_SET(actions, SIA_MENU_ACTION_MENU))
	{
		if (!smaCreateAction(menu->handle, WeMenu, handleMenuAction, 0, 
			0, SIA_STR_ID_ACTION_MENU))
		{
			return 0;
		}
	}

    /* Add title only if titleStrId is valid */
    if (0 != titleStrHandle)
    {
        (void)smaSetTitle(menu->handle, titleStrHandle, 0);
    }

    return handle;
}


static SiaMenu *getMenuInstanceByHandle(SiaMenuHandle handle)
{
    if (handle <= SIA_MAX_MENUS)
    {
        return navMenuInstances[handle].navMenuPtr;
    }
    else
    {
        return NULL;
    }
}

static SiaMenu *getMenuInstanceByWindowHandle(WeWindowHandle handle)
{
    int i;
    for (i=1; i<= SIA_MAX_MENUS; i++)
    {
        if (navMenuInstances[i].navMenuPtr->handle == handle)
        {
            return navMenuInstances[i].navMenuPtr;
        }
    }
    return NULL;
}


static SiaMenuHandle createMenuInstance(SiaMenu **menu)
{
    int i;

    for (i=1; i <= SIA_MAX_MENUS; i++)
    {
        if (0 == navMenuInstances[i].handle)
        {
            /* found an empty spot */
            navMenuInstances[i].handle = i;
            navMenuInstances[i].navMenuPtr = SIA_CALLOC(sizeof(SiaMenu));
            *menu = navMenuInstances[i].navMenuPtr;
            return navMenuInstances[i].handle;
        } /* if */
    }
    return 0;
}


static void removeMenuInstance(SiaMenuHandle handle)
{
    if (handle <= SIA_MAX_MENUS)
    {
        navMenuInstances[handle].handle = 0;
		if (navMenuInstances[handle].navMenuPtr)
		{
			SIA_FREE(navMenuInstances[handle].navMenuPtr);
			navMenuInstances[handle].navMenuPtr = NULL;
		}

    }
}



/*!\brief Adds a item at the end of a previously created menu.
 *
 * \param itemStrHandle The string that are displayed to the user.
 * \param contextValue A value that will be returned when smaGetMenuItem is
 *                    called.
 * \return TRUE if the item is generated successfully, otherwise FALSE.
 *****************************************************************************/
SiaMenuResult smaAddMenuItem(SiaMenuHandle handle, WeStringHandle string1, 
    WeStringHandle string2, WeImageHandle image1, WeImageHandle image2, 
    int contextValue, WE_BOOL hasSubMenu)
{
    SiaMenu *menu;

    menu = getMenuInstanceByHandle(handle);
    
    if (menu == NULL)
    {
        return SIA_MENU_RESULT_ERROR;
    } /* if */

    if (menu->listIndex >= SIA_MAX_CONTEXT_LIST_ITEMS)
    {
        return SIA_MENU_RESULT_TO_MANY_ITEMS;
    }
    if (0 > WE_WIDGET_CHOICE_SET_ELEMENT(menu->handle, 
        menu->listIndex, 0, string1, string2, image1, image2, 0, 0, hasSubMenu))
    {
        return SIA_MENU_RESULT_ERROR;
    }
    menu->type[menu->listIndex] = SIA_MENU_TYPE_ORIGINAL;
    menu->item.listItems[menu->listIndex] = contextValue;
    menu->listIndex++;
    return SIA_MENU_RESULT_OK;
}


/*!\brief Returns the selected item in a menu.
 *
 * \return The item that is currently selected or -1 if no selected item could
 *         be extracted.
 *****************************************************************************/
int smaGetMenuItem(SiaMenuHandle handle)
{
    int index;
    SiaMenu *menu;

    menu = getMenuInstanceByHandle(handle);
    
    if (menu == NULL)
    {
        return FALSE;
    } /* if */

    if (0 == menu->handle)
    {
        return -1;
    }
    index = smaGetSelectedChoiceIndex(menu->handle);
    if ((index < 0) || (index >= SIA_MAX_CONTEXT_LIST_ITEMS))
    {
        return -1;
    }
    /*
     *	Insert code here for checking item type and returning the correct item
     */
    return menu->item.listItems[index];
}


/*!\brief Displayes a previously created menu.
 *
 * \return TRUE if the menu could be displayed. otherwise FALSE.
 *
 *****************************************************************************/
WE_BOOL smaShowMenu(SiaMenuHandle handle)
{
    SiaMenu *menu;

    menu = getMenuInstanceByHandle(handle);
    
    if (menu == NULL || menu->handle == 0)
    {
        return FALSE;
    }
    return smaBindAndShowWindow(menu->handle, smaGetPosLeftTop());
}

/*!\brief Removes a previously created navigation menu.
 *
 *****************************************************************************/
void smaRemoveMenu(SiaMenuHandle handle)
{
    SiaMenu *menu;

    menu = getMenuInstanceByHandle(handle);
    
    if (menu == NULL)
    {
        return;
    } /* if */

    /* Deregister window and remove actions */
    (void)smaDeleteWindow(menu->handle);
	removeMenuInstance(handle);
}

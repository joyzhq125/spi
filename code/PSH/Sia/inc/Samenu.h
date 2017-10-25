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

/*!\file Samenu.h
 * \brief Functions for handling menu
 */

#ifndef SAMENU_H
#define SAMENU_H


#define SIA_MAX_MENUS 5

/**************************************************
 * Macros
 **************************************************/

#define SIA_CREATE_NAV_MENU(strH, callBack) \
    smaCreateMenu ((WeStringHandle)(strH), (SiaCallBack)(callBack), \
    SIA_MENU_ACTION_OK | SIA_MENU_ACTION_BACK, WE_CHOICE_ELEMENT_STRING_1, 0, 0)

#define SIA_CREATE_SI_MENU(strH, callBack) \
    smaCreateMenu ((WeStringHandle)(strH), (SiaCallBack)(callBack), \
    SIA_MENU_ACTION_OK | SIA_MENU_ACTION_BACK | SIA_MENU_ACTION_MENU, \
    WE_CHOICE_ELEMENT_STRING_1 | WE_CHOICE_ELEMENT_IMAGE_1, 0, 0)


#define SIA_ADD_NAV_MENU_ITEM(handle, strH, contextV) \
  smaAddMenuItem ((SiaMenuHandle)(handle),(WeStringHandle)(strH), 0, 0, 0, (int)(contextV), FALSE)


#define SIA_ADD_SI_MENU_ITEM(handle, strH, contextV) \
  smaAddMenuItem ((SiaMenuHandle)(handle),(WeStringHandle)(strH), 0, 0, 0, (int)(contextV), FALSE)

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! \enum SiaAction
 *  Actions related to the context dependent menu */
typedef enum
{
    SIA_MENU_ACTION_OK          = 0x01, /* !< The End User presses ok */
    SIA_MENU_ACTION_BACK        = 0x02, /* !< The End User presses back */
    SIA_MENU_ACTION_MENU        = 0x04
} SiaMenuAction;

typedef enum
{
    SIA_MENU_RESULT_OK,
    SIA_MENU_RESULT_TO_MANY_ITEMS,
    SIA_MENU_RESULT_ERROR
} SiaMenuResult;

typedef int SiaMenuHandle;

/******************************************************************************
 * Data-types
 *****************************************************************************/
typedef void (*SiaCallBack)(SiaMenuAction actionType);
typedef WE_BOOL (*SiaMenuVisibilityCallBack)(int entryId);

/*! \struct SiaMenuItems
 *  \brief Holds all needed data for a navigation menu entry
 */
typedef struct {
    int             entryId;/*!< Unique id for this menu entry */
    WE_UINT32      strId;  /*!< String to display for menu item */
    SiaMenuVisibilityCallBack visibilityCallBack; /*!< Callback function used to
                                                      decide whether to show 
                                                      this menu entry */
}SiaMenuItem;

/******************************************************************************
 * Prototypes
 *****************************************************************************/

void smaInitMenu(void);
void smaTerminateMenu(void);

SiaMenuHandle smaCreateMenu(WeStringHandle titleStrHandle, 
    SiaCallBack signalHandler, int actions, int showAttributes, 
    SiaMenuHandle parent, WE_UINT32 parentIndex);
SiaMenuResult smaAddMenuItem(SiaMenuHandle handle, WeStringHandle string1, 
    WeStringHandle string2, WeImageHandle image1, WeImageHandle image2, 
    int contextValue, WE_BOOL hasSubMenu);
int smaGetMenuItem(SiaMenuHandle handle);
WE_BOOL smaShowMenu(SiaMenuHandle handle);
void smaRemoveMenu(SiaMenuHandle handle);

#endif

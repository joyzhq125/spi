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

#ifndef SASIS_H
#define SASIS_H


/*--- Definitions/Declarations ---*/

/*--- Types ---*/
typedef enum
{
    SIA_SLIDE_MENU_PREV = 1,
    SIA_SLIDE_MENU_NEXT,
    SIA_SLIDE_MENU_REWIND,
    SIA_SLIDE_MENU_BACK,
    SIA_SLIDE_MENU_EXIT,
	SIA_SLIDE_MENU_OBJ_LIST,
    SIA_SLIDE_MENU_NO_OF_ITEMS
} SiaSlideMenuItems;


/*********************************************
 * Exported function
 *********************************************/

WE_BOOL smaCreateSlideMenu(we_act_action_entry_t  *actions, 
    WE_UINT32 actionCount, SiaObjectInfoList *objInfoList, 
    SmtrLinkEntry *selectedLink);
void smaDeleteSlideMenu(void);
#endif /* SASIS_H */

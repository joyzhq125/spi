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

/*--- Include files ---*/
/* WE */
#include "We_Log.h"    /* WE: Signal logging */ 
#include "We_Int.h"
#include "We_Core.h"
#include "We_Lib.h"    /* WE: ANSI Standard libs allowed to use */
#include "We_Wid.h"
#include "We_Mem.h"
#include "We_Pck.h"
#include "We_Act.h"

/* SIS */
#include "Sis_cfg.h"
#include "Sis_if.h"
#include "Sis_def.h"

/* MMS */
#include "Mms_Def.h"
#include "Mms_Cfg.h"
#include "Mms_If.h"

/* SMA */
#include "Sia_rc.h"
#include "Saintsig.h"
#include "Sia_if.h"
#include "Satypes.h"
#include "Smtr.h"
#include "Samem.h"
#include "Samain.h"
#include "Sauiform.h"
#include "Sauiform.h"
#include "Saui.h"
#include "Saui.h"
#include "Samenu.h"
#include "Sasmnu.h"
#include "Saattach.h"
#include "Sacrh.h"


/*--- Definitions/Declarations ---*/

/*--- Types ---*/

/* \struct SlideInstanceData Used to hold information between states */
typedef struct
{
    WE_UINT32				actionCount;
    we_act_action_entry_t  *actions;
	SiaMenuHandle			menuHandle;
	SiaObjectInfoList		*objInfoList;
    SmtrLinkEntry           *selectedLink;
}SlideInstanceData;

/*--- Constants ---*/

/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Global variables ---*/

/*--- Static variables ---*/

static SlideInstanceData instData;

/******************************************************************************
 * Function prototypes
 *****************************************************************************/
static void navMenuSignalConverter(SiaMenuAction actionType);
static WE_BOOL createMenuItems(SiaMenuHandle menuHandle, 
    const we_act_action_entry_t *actions, WE_UINT32 count,
    const SmtrLinkEntry *selectedLink);
static WE_BOOL viewLink(void *linkIn);

/*!
 * \brief Deletes the navigation menu for the multipart/mixed msg viewer.
 *
 *****************************************************************************/
void smaDeleteSlideMenu(void)
{
    if (instData.actions != NULL)
	{
		smaFreeActionList(instData.actions, instData.actionCount);
		instData.actions = NULL;
	}
	smaRemoveMenu(instData.menuHandle);
}

/*!
 * \brief Create and view attachment menu 
 * \return TRUE upon success
 *****************************************************************************/
WE_BOOL smaCreateSlideMenu(we_act_action_entry_t  *actions, 
    WE_UINT32 actionCount, SiaObjectInfoList *objInfoList, 
    SmtrLinkEntry *selectedLink)
{
	instData.actions = actions;
    instData.actionCount = actionCount;
	instData.objInfoList  = objInfoList;
    instData.selectedLink = selectedLink;

    instData.menuHandle = SIA_CREATE_NAV_MENU(0, navMenuSignalConverter);

    if (0 == instData.menuHandle)
    {
        return FALSE;  
    }
    else if (!createMenuItems(instData.menuHandle, actions, actionCount, selectedLink))
    {
        smaDeleteSlideMenu();
        return FALSE;
    }
    else if (!smaShowMenu(instData.menuHandle))
    {
        smaDeleteSlideMenu();
        return FALSE;
    }

    return TRUE;
}

/*!
 * \brief Create and view attachment menu 
 * \brief mo The current media object
 * 
 * \return TRUE upon success
 *****************************************************************************/
static WE_BOOL createMenuItems(SiaMenuHandle menuHandle, 
    const we_act_action_entry_t *actions, WE_UINT32 count,
    const SmtrLinkEntry *selectedLink)
{
    WE_UINT32 i = 0;
    WE_UINT32 strId = 0;
    WE_BOOL knownScheme = FALSE;
    char *tmpStr;
    char *str;


    for (i = 0; i < count; i++)
    {
        if (SIA_MENU_RESULT_OK != SIA_ADD_NAV_MENU_ITEM(menuHandle, 
            SIA_GET_STR_ID((WE_UINT32)actions[i].string_id), i))
        {
            return FALSE;
        }
    }

    /* add selected link items */
    if (selectedLink != NULL)
    {
        
        switch (selectedLink->scheme)
        {
            case SMTR_SCHEME_HTTP:
            case SMTR_SCHEME_HTTPS:
            case SMTR_SCHEME_WWW:
            case SMTR_SCHEME_WAP:
                strId = SIA_GET_STR_ID(SIA_STR_ID_OPEN_LINK);
                tmpStr = smaGetStringBufferFromHandle(strId);
                str = SIA_ALLOC(strlen(tmpStr) + strlen(selectedLink->link) + 1);
                sprintf(str, "%s%s", tmpStr, selectedLink->link);
                strId = smaCreateString(str);
                SIA_FREE(str);
                knownScheme = TRUE;
                break;
            case SMTR_SCHEME_PHONE:
                strId = SIA_GET_STR_ID(SIA_STR_ID_MAKE_CALL);
                tmpStr = smaGetStringBufferFromHandle(strId);
                str = SIA_ALLOC(strlen(tmpStr) + strlen(selectedLink->link) + 1);
                sprintf(str, "%s%s", tmpStr, selectedLink->link);
                strId = smaCreateString(str);
                SIA_FREE(str);
                knownScheme = TRUE;
                break;
            case SMTR_SCHEME_NONE:
            case SMTR_SCHEME_MAIL:
            default:
                knownScheme = FALSE;
                break;
        } /* switch */

        if (knownScheme)
        {
            if (SIA_MENU_RESULT_OK != 
                SIA_ADD_NAV_MENU_ITEM(menuHandle, strId, count))
            {
                return FALSE;
            }
            WE_WIDGET_RELEASE(strId);
        }
        count++;
    }
    
    /* Back */
    if (SIA_MENU_RESULT_OK != SIA_ADD_NAV_MENU_ITEM(menuHandle,
        SIA_GET_STR_ID(SIA_STR_ID_MENU_NEXT), count + SIA_SLIDE_MENU_NEXT))
    {
        return FALSE;
    }      
    if (SIA_MENU_RESULT_OK != SIA_ADD_NAV_MENU_ITEM(menuHandle, 
        SIA_GET_STR_ID(SIA_STR_ID_MENU_PREVIOUS), count + SIA_SLIDE_MENU_PREV))
    {
        return FALSE;
    }      
    if (SIA_MENU_RESULT_OK != SIA_ADD_NAV_MENU_ITEM(menuHandle,
        SIA_GET_STR_ID(SIA_STR_ID_MENU_REWIND), count + SIA_SLIDE_MENU_REWIND))
    {
        return FALSE;
    }      
    if (SIA_MENU_RESULT_OK != SIA_ADD_NAV_MENU_ITEM(menuHandle,
        SIA_GET_STR_ID(SIA_STR_ID_OBJ_LIST), count + SIA_SLIDE_MENU_OBJ_LIST))
    {
        return FALSE;
    }      
    if (SIA_MENU_RESULT_OK != SIA_ADD_NAV_MENU_ITEM(menuHandle,
        SIA_GET_STR_ID(SIA_STR_ID_ACTION_BACK), count + SIA_SLIDE_MENU_BACK))
    {
        return FALSE;
    }      
    return TRUE;
}


/*!
 * \brief Signal converter for the navigation menu.
 * 
 * \param actionType The type of action, MEA_NAV_ACTION_BACK or 
 *  MEA_NAV_ACTION_OK.
 *****************************************************************************/
static void navMenuSignalConverter(SiaMenuAction actionType)
{
    WE_UINT32 count = instData.actionCount;
    WE_UINT32 item = (WE_UINT32)smaGetMenuItem(instData.menuHandle);

    if (actionType == SIA_MENU_ACTION_BACK)
    {
        smaDeleteSlideMenu();
        return;
    }
    if (actionType == SIA_MENU_ACTION_OK)
    {
        if (item < instData.actionCount)
        {
           /* Handle object actions */
			(void)SIA_SIGNAL_SENDTO_UUP(SIA_FSM_MAIN, SIA_SIG_MH_OBJ_ACTION, 
               item, instData.actionCount, instData.actions);
        }
        else
        {
            /* remove "count-offset" */
            item = item - count;

            /* do we have any link options */
            if (instData.selectedLink != NULL)
            {
                if (item == 0)
                {
                    (void)viewLink(instData.selectedLink);
                    item=SIA_SLIDE_MENU_BACK;
                }
                else
                {/* if */
                    item++;
                }
            } /* if */

            switch (item)
            {
            case SIA_SLIDE_MENU_BACK:
                smaDeleteSlideMenu();
                break;
            case SIA_SLIDE_MENU_PREV:
                smaDisplayPreviousSlide();
                smaDeleteSlideMenu();
                break;
            case SIA_SLIDE_MENU_NEXT:
                smaDisplayNextSlide();
                smaDeleteSlideMenu();
                break;
            case SIA_SLIDE_MENU_REWIND:
                smaRewindSmil();
                smaDeleteSlideMenu();
                break;
            case SIA_SLIDE_MENU_OBJ_LIST:
				/* show object list */
				(void)smaCreateAttachmentMenu(instData.objInfoList);
				break;
			}
        }
    }
} 




/*!
 * \brief Starts the BRA with a specific URL.
 *
 * \param url The URL to open in the BRA.
 *****************************************************************************/
static void startBra(char *url)
{
#ifdef WE_MODID_BRA
    we_content_data_t  contentData;

    memset(&contentData, 0, sizeof(we_content_data_t));
    contentData.contentUrl = url;
    contentData.contentDataType = WE_CONTENT_DATA_NONE;
    WE_MODULE_START(WE_MODID_SIA, WE_MODID_BRA, NULL, &contentData, NULL);
#else
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA,
        "(%s) (%d) Failed to start the BRA with the URL: %s, because the BRA"
        "is not defined!\n", __FILE__, __LINE__, url));
#endif
}

/*!
 * \brief Handles the viewing of a link detected inside a text object
 * 
 * \param linkIn  The particular link
 * \param fsm     The fsm to call when viewing is done
 * \param sig     The signal to send when viewing is done
 * \return TRUE upon success
 *****************************************************************************/
static WE_BOOL viewLink(void *linkIn)
{
    SmtrLinkEntry *link = (SmtrLinkEntry *)linkIn;

    if ((NULL == link) || (NULL == link->link))
    {
        return FALSE;
    } /* if */

    switch (link->scheme)
    {
    case SMTR_SCHEME_HTTP:
    case SMTR_SCHEME_HTTPS:
    case SMTR_SCHEME_WWW:
    case SMTR_SCHEME_WAP:
        startBra(link->link);
        return TRUE;
    case SMTR_SCHEME_PHONE:
        smaMakeCall(link->link);
        return TRUE;

    case SMTR_SCHEME_NONE:
    case SMTR_SCHEME_MAIL:
    default:
        return FALSE;
    } /* switch */

} /* viewLink */



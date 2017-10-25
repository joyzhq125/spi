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
#include "We_Lib.h"    /* WE: ANSI Standard libs allowed to use */
#include "We_Cfg.h"
#include "We_Core.h"
#include "We_Wid.h"
#include "We_Log.h"
#include "We_Mem.h"
#include "We_Mime.h"
#include "We_Pck.h"
#include "We_Act.h"

/* SIS */
#include "Sis_cfg.h"
#include "Sis_def.h"
#include "Sis_if.h"

/* MMS */
#include "Mms_Def.h"
#include "Mms_Cfg.h"
#include "Mms_If.h"

#include "Sia_if.h"
#include "Satypes.h"
#include "Samain.h"
#include "Samem.h"
#include "Saintsig.h"
#include "Sauiform.h"
#include "Saui.h"
#include "Samenu.h"
#include "Saattach.h"
#include "Sasls.h"
#include "Sacrh.h"
#include "Sauidia.h"
#include "Sia_rc.h"



/* Tells if a body-part is containing a SMIL presentation */
#define IS_SMIL_PART(cc) (0 == we_cmmn_strcmp_nc((char *)cc.strValue,\
    MMS_MEDIA_TYPE_STRING_SMIL))


/*--- Types ---*/
typedef struct 
{
    SiaObjectInfoList		*objList;
    SiaMenuHandle			menuHandle;

    
	we_act_action_entry_t	*actions;
	WE_UINT32				actionCount;
    SiaMenuHandle			actionMenuHandle;
	int				        currentItem;
}InstanceData;

/******************************************************************************
 * Constants
 ***********************§******************************************************/
static InstanceData instData;

/*--- Definitions/Declarations ---*/
/*--- Prototypes ---*/
static void sigHandler(SiaSignal *sig);
static void attachmentCallback(SiaMenuAction actionType);
static SiaMenuResult createAttachmentMenuItems(SiaMenuHandle handle, 
    const SiaObjectInfoList *objInfoList);
static void actionMenuCallback(SiaMenuAction actionType);
static WE_BOOL createActionMenuItems(SiaMenuHandle menuHandle, 
    const we_act_action_entry_t *actions, WE_UINT32 count);
static void handleObjAction(const SiaObjectInfo *objInfo, WE_UINT32 index);
static WE_BOOL smaCreateActionMenu(const we_act_action_entry_t  *actions, 
    WE_UINT32 actionCount);

/*!
 * \brief Initialize the SIA_FSM_ATTACHMENT.
 *****************************************************************************/
void smaAttachmentInit(void)
{
    smaSignalRegisterDst(SIA_FSM_ATTACHMENT, sigHandler);
    memset(&instData, 0x00, sizeof(InstanceData));
   
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
        "SIA_FSM_ATTACHMENT: initialized\n"));
} /* smaAttachmentInit */

/*!
 * \brief Terminate the M_FSM_CORE_MAIN FSM.
 *****************************************************************************/
void smaAttachmentTerminate(void)
{
	smaSignalDeregister(SIA_FSM_ATTACHMENT);
    smaRemoveMenu(instData.menuHandle);
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
        "SIA_FSM_ATTACHMENT: terminated\n"));
} /* smaAttachmentTerminate */

/*!
 * \brief Signal handler for the SIA_FSM_PIPE.
 *****************************************************************************/
static void sigHandler(SiaSignal *sig)
{
    switch (sig->type)
    {
    case SIA_SIG_ATTACHMENT_GET_ACTIONS_RSP:
        /* have the actions for the object */
		if (sig->u_param1 == SIA_CRH_OK)
        {
            instData.actions = sig->p_param;
            (void)smaCreateActionMenu(sig->p_param, sig->u_param2);
            
        }
        else
        {
            /* do nothing */
			return;
        }
        break;
    default:
		WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA,
            "(%s) (%d) Erroneous signal recieved in SIA_FSM_PIPE:sigHanlder\n", 
            __FILE__, __LINE__));
        break;
    } /* switch */
	smaSignalDelete(sig);
} /* sigHandler */

/*!
 * \brief Creates the context sensitive menu for the message handler.
 *
 * \param  folder The folder that i currently active.
 * \return TRUE if the operation was successful, otherwise FALSE.
  *****************************************************************************/
WE_BOOL smaCreateAttachmentMenu(SiaObjectInfoList *objInfoList)
{
    SiaMenuResult result;
    if (objInfoList == NULL)
    {
        return FALSE;
    }

    /* Create menu */
	instData.objList = objInfoList;
    instData.menuHandle = SIA_CREATE_SI_MENU(0, attachmentCallback);

    if (instData.menuHandle == 0)
    {
        return FALSE;
    } /* if */

    result = createAttachmentMenuItems(instData.menuHandle, objInfoList);
    if (SIA_MENU_RESULT_ERROR == result)
    {
		/* error */
		smaRemoveMenu(instData.menuHandle);
        return FALSE;
    } /* if */
	
	if (!smaShowMenu(instData.menuHandle))
    {
        smaRemoveMenu(instData.menuHandle);
        return FALSE;
    } /* if */
    if (SIA_MENU_RESULT_TO_MANY_ITEMS == result)
    {
        (void)smaShowDialog(SIA_GET_STR_ID(SIA_STR_DIA_TO_MANY_ITEMS), 
            SIA_DIALOG_INFO);
    }
    return TRUE;
} /* createAttachmentMenu */


static void attachmentCallback(SiaMenuAction actionType)
{
    int item;
    SiaObjectInfo *objInfo;
    char *drmCt;

	switch (actionType)
	{
	case SIA_MENU_ACTION_OK:
	case SIA_MENU_ACTION_MENU:
		/* show object action menu */
        item = smaGetMenuItem(instData.menuHandle);
        if (item > 0)
        {
            /* item found */
			instData.currentItem = item;
            objInfo = smaGetObjectInfo((WE_UINT32)item, instData.objList);
            if (objInfo != NULL)
            {
                drmCt = smaMakeDrmContentType(&objInfo->obj.contentType);
                /* object found - get actions */
                smaCrhGetActions(&objInfo->obj.contentType, drmCt,
                    SIA_FSM_ATTACHMENT, SIA_SIG_ATTACHMENT_GET_ACTIONS_RSP);

                if (drmCt != NULL)
                {
                    SIA_FREE(drmCt);
                } /* if */

            }
            else
            {
                /* object not found - so no actions available */
                (void)SIA_SIGNAL_SENDTO_UUP(SIA_FSM_ATTACHMENT, 
                    SIA_SIG_ATTACHMENT_GET_ACTIONS_RSP, SIA_CRH_ERROR, 0, NULL);
            } /* if */
        } /* if */
		break;
	case SIA_MENU_ACTION_BACK:
		/* remove the menu */
        smaRemoveMenu(instData.menuHandle);
		break;
	}
}


static WE_BOOL isMultiPart(const MmsContentType *ct)
{
	if (ct == NULL)
	{
		return FALSE;
	}

	if ((0 == we_cmmn_strcmp_nc((char *)ct->strValue, 
		we_int_to_mime(WE_MIME_TYPE_MULTIPART_ANY))) ||

		(0 == we_cmmn_strcmp_nc((char *)ct->strValue, 
		we_int_to_mime(WE_MIME_TYPE_MULTIPART_MIXED))) ||
		(0 == we_cmmn_strcmp_nc((char *)ct->strValue, 
		we_int_to_mime(WE_MIME_TYPE_MULTIPART_FORM_DATA))) ||
		(0 == we_cmmn_strcmp_nc((char *)ct->strValue, 
		we_int_to_mime(WE_MIME_TYPE_MULTIPART_BYTERANGES))) ||
		(0 == we_cmmn_strcmp_nc((char *)ct->strValue, 
		we_int_to_mime(WE_MIME_TYPE_MULTIPART_ALTERNATIVE))) ||
		(0 == we_cmmn_strcmp_nc((char *)ct->strValue, 
		we_int_to_mime(WE_MIME_TYPE_APPLICATION_VND_WAP_MULTIPART_ANY))) ||
		(0 == we_cmmn_strcmp_nc((char *)ct->strValue, 
		we_int_to_mime(WE_MIME_TYPE_APPLICATION_VND_WAP_MULTIPART_MIXED))) ||
		(0 == we_cmmn_strcmp_nc((char *)ct->strValue, 
		we_int_to_mime(WE_MIME_TYPE_APPLICATION_VND_WAP_MULTIPART_FORM_DATA))) ||
		(0 == we_cmmn_strcmp_nc((char *)ct->strValue, 
		we_int_to_mime(WE_MIME_TYPE_APPLICATION_VND_WAP_MULTIPART_BYTERANGES))) ||
		(0 == we_cmmn_strcmp_nc((char *)ct->strValue, 
		we_int_to_mime(WE_MIME_TYPE_APPLICATION_VND_WAP_MULTIPART_ALTERNATIVE))) ||
		(0 == we_cmmn_strcmp_nc((char *)ct->strValue, 
		we_int_to_mime(WE_MIME_TYPE_APPLICATION_VND_WAP_MULTIPART_RELATED))))
	{
		return TRUE;
	}

	return FALSE;
}


/*!
 * \brief Creates menu items
 *
 * \param  smaMenuHandle The menu to add items to.
 * \param attachmentList The list of items to add.
 * \return TRUE if the operation was successful, otherwise FALSE.
 *****************************************************************************/
static SiaMenuResult createAttachmentMenuItems(SiaMenuHandle handle, 
    const SiaObjectInfoList *objInfoList)
{
    WE_UINT32 strHandle;
    char *fileName;
    const SiaObjectInfo *objInfo;
    SiaMenuResult result;

    /* Add content to the menu*/
	if (objInfoList == NULL) 
    {
        return FALSE;
    } /* if */

    while (objInfoList != NULL)
    {
        objInfo = &objInfoList->current;

        if (!IS_SMIL_PART(objInfo->obj.contentType) && 
		    !isMultiPart(&objInfo->obj.contentType))
		{
            fileName = smaCreateFileName(objInfo);

            if (fileName != NULL)
            {
                strHandle = smaCreateString(fileName);
                result = SIA_ADD_SI_MENU_ITEM(handle, strHandle, objInfo->obj.id);
	            
                SIA_FREE(fileName);
                if (SIA_MENU_RESULT_OK != result)
                {
		            return result;
	            } /* if */
            }
        }
        objInfoList = objInfoList->next;
    } /* while */
    return SIA_MENU_RESULT_OK;
} /* createAttachmentMenuItems */




/*!
 * \brief Create and view attachment menu 
 * \return TRUE upon success
 *****************************************************************************/
static WE_BOOL smaCreateActionMenu(const we_act_action_entry_t  *actions, 
    WE_UINT32 actionCount)
{
    instData.actionCount = actionCount;

    instData.actionMenuHandle = SIA_CREATE_NAV_MENU(0, actionMenuCallback);

    if (0 == instData.actionMenuHandle)
    {
        return FALSE;  
    }
    else if (!createActionMenuItems(instData.actionMenuHandle, actions, actionCount))
    {
        smaRemoveMenu(instData.actionMenuHandle);
        return FALSE;
    }
    else if (!smaShowMenu(instData.actionMenuHandle))
    {
        smaRemoveMenu(instData.actionMenuHandle);
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
static WE_BOOL createActionMenuItems(SiaMenuHandle menuHandle, 
    const we_act_action_entry_t *actions, WE_UINT32 count)
{
    unsigned int i = 0;

    for (i = 0; i < count; i++)
    {
        if (SIA_MENU_RESULT_OK != SIA_ADD_NAV_MENU_ITEM(menuHandle, 
            SIA_GET_STR_ID((WE_UINT32)actions[i].string_id), i))
        {
            return FALSE;
        }
    }
    return TRUE;
}

/*!
 * \brief Signal converter for the navigation menu.
 * 
 * \param actionType The type of action, MEA_NAV_ACTION_BACK or 
 *  MEA_NAV_ACTION_OK.
 *****************************************************************************/
static void actionMenuCallback(SiaMenuAction actionType)
{
    int item = smaGetMenuItem(instData.actionMenuHandle);
	SiaObjectInfo *objInfo;

    if (actionType == SIA_MENU_ACTION_BACK)
    {
        smaRemoveMenu(instData.actionMenuHandle);
    }
    else if (actionType == SIA_MENU_ACTION_OK)
    {
        if (item <= (int)instData.actionCount)
        {
			/* Handle object actions */
			objInfo = smaGetObjectInfo((WE_UINT32)instData.currentItem, 
                instData.objList);
		    smaRemoveMenu(instData.actionMenuHandle);
			handleObjAction(objInfo, (WE_UINT32)item);
		}
	}
    smaFreeActionList(instData.actions,instData.actionCount);
    instData.actions=NULL;
    instData.actionCount=0;
} 


/*!
 * \brief Handle object actions
 *
 * \param instance The current instance.
 * \param index The index in the object action list.
 *****************************************************************************/
static void handleObjAction(const SiaObjectInfo *objInfo, WE_UINT32 index)
{
    we_pck_attr_list_t *fileAttributes;
    char *fileName;

    if (objInfo == NULL)
    {
        return;
    }
    fileName = smaCreateFileName(objInfo);

    /* Set file attributes */
    we_pck_attr_init(&fileAttributes);

    (void)we_pck_add_attr_string_value (WE_MODID_SIA, &fileAttributes, 
		WE_PCK_ATTRIBUTE_MIME, (const char *)objInfo->obj.contentType.strValue);
	
    (void)we_pck_add_attr_int_value (WE_MODID_SIA, &fileAttributes,
		WE_PCK_ATTRIBUTE_SIZE, (WE_INT32)objInfo->obj.size);

    /* Send the content to the specified module */
    smaCrhSendContent(fileAttributes, fileName, 
        SIA_FSM_ATTACHMENT, SIA_SIG_ATTACHMENT_OBJ_ACTION_DONE, 
		&(instData.actions[index]), smaGetMsgId(), (WE_UINT16)objInfo->obj.id);
    we_pck_attr_free(WE_MODID_SIA, fileAttributes);

    if (fileName != NULL)
    {
        SIA_FREE(fileName);
    } /* if */
}

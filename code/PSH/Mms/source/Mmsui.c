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
 





#include "We_Lib.h"    
#include "We_Def.h"     
#include "We_Core.h"    
#include "We_Dcvt.h"    
#include "We_Mem.h"    
#include "We_Cfg.h"    
#include "We_Cfg.h"     

#include "Mms_Cfg.h"    
#include "Mms_Def.h"    
#include "Mms_If.h"     

#ifdef MMS_UBS_IN_USE

#include "Ubs_If.h"     
#include "Mmsui.h"      

#include "Msig.h"       
#include "Fldmgr.h"     


#define MMS_NUM_TITLE_VALUES (11)











 
ubs_title_property_array_element_t mms_ubs_titles[] = UBS_TITLE_PROPERTY_ARRAY;


#define MMS_MAX_UI_SUBSCRIBERS 8
static WE_UINT8 g_regUBSubscribers[MMS_MAX_UI_SUBSCRIBERS];
static WE_UINT8 g_numRegUBSubscribers = 0;


WE_UINT16 fldrMgrGetUbsData(WE_UINT16 filterListSize,
                             ubs_key_value_t *filterList,
                             WE_UINT16 nMsgs,
                             WE_UINT16 firstMsg,
                             ubs_msg_list_item_t *list);

MmsMessageFolderInfo *Mms_FldrMgrGetMsgInfo(WE_UINT32 msgId);

void fldrMgrGetNumberOfMessagesUbs( WE_UINT16 filterListSize,
                                   ubs_key_value_t *filterList,
                                   WE_UINT16 *nMsgs,
                                   WE_UINT16 *nUnreadMsgs);
                                   
const char *fldMgrFolderToString(MmsFolderType folderType);
MmsFolderType fldMgrStringToFolder(const char *folderId);

    


static void free_ubs_msg_handle(WE_UINT8 module, ubs_msg_handle_t *p)
{
    if (NULL != p)
    {
        if (NULL != p->handle)
        {
            WE_MEM_FREE (module, p->handle);
            p->handle = NULL;
        }
    }
} 


static void free_ubs_key_value(WE_UINT8 module, ubs_key_value_t *p)
{
    if (NULL != p)
    {
        if (NULL != p->value)
        {
            WE_MEM_FREE (module, p->value);
            p->value = NULL;
        }
    }
} 


static void free_ubs_msg_list_item(WE_UINT8 module, ubs_msg_list_item_t *p)
{
    int i;

    if (NULL != p)
    {
        free_ubs_msg_handle(module, &(p->handle));

        if (NULL != p->titleList)
        {
            for (i = 0; i < (p->titleListSize); i++)
            {
                free_ubs_key_value(module, &(p->titleList[i]));
            }
            WE_MEM_FREE(module, p->titleList);
            p->titleList = NULL;
        }
    }
} 


static void free_ubs_msg(WE_UINT8 module, ubs_msg_t *p)
{
    int i;

    if (NULL != p)
    {
        free_ubs_msg_handle(module, &(p->handle));

        if (NULL != p->propertyList)
        {
            for (i = 0; i < (p->propertyListSize); i++) {
                free_ubs_key_value(module, &(p->propertyList[i]));
            }
            WE_MEM_FREE(module, p->propertyList);
            p->propertyList = NULL;
        }
    }
} 








































 








static void set_ubs_UTF8_value(WE_UINT8 module,
                            ubs_key_value_t *result,
                            WE_UINT16 key,
                            const unsigned char *value)
{
    WE_UINT16 len;

    result->key = key;
    result->valueType = UBS_VALUE_TYPE_UTF8;
    if (NULL == value)
    {
        result->value = NULL;
        len = 0;
    }
    else
    {
        len = (WE_UINT16) (strlen((const char*)value)+1);
        result->value = WE_MEM_ALLOC(module, len);
        if (NULL == result->value)
        {
            result->valueLen = 0;
            return ;
        }
        strcpy((char *)result->value, (const char *) value);
    }
    result->valueLen = len;
} 








static void set_ubs_uint32_value(WE_UINT8 module,
                            ubs_key_value_t *result,
                            WE_UINT16 key,
                            WE_UINT32 value)
{
    WE_UINT32 *p = NULL;

    result->key = key;
    result->valueType = UBS_VALUE_TYPE_UINT32;
    p = WE_MEM_ALLOC(module, sizeof(value));
    result->value = (unsigned char *) p;
    *p = value;
    result->valueLen = sizeof(value);
} 



















 








static void set_ubs_bool_value(WE_UINT8 module,
                            ubs_key_value_t *result,
                            WE_UINT16 key,
                            WE_BOOL value)
{
    WE_BOOL *p = NULL;

    result->key = key;
    result->valueType = UBS_VALUE_TYPE_BOOL;
    p = WE_MEM_ALLOC(module, sizeof(value));
    result->value = (unsigned char *) p;
    *p = value;
    result->valueLen = sizeof(value);
} 

































 









static WE_UINT32 fromHandleToId(const ubs_msg_handle_t *handle)
{
    WE_UINT32 msgId = 0;
    unsigned char *data = handle->handle;

    if (handle == NULL || handle->handleLen != 4)
    {
        return 0;
    }
    
    msgId = (WE_UINT32) data[0];
    msgId = (WE_UINT32) ((msgId << 8) + data[1]);
    msgId = (WE_UINT32) ((msgId << 8) + data[2]);
    msgId = (WE_UINT32) ((msgId << 8) + data[3]);

    return msgId;
}







static unsigned char *fromIdToHandle(WE_UINT8 module, WE_UINT32 msgId)
{    
    unsigned char *handle = WE_MEM_ALLOC(module, 4);
    if (NULL == handle)
    {
        return NULL;
    }

    handle[3] = (unsigned char) (msgId & 0xff);
    handle[2] = (unsigned char) ((msgId >> 8) & 0xff);
    handle[1] = (unsigned char) ((msgId >> 16) & 0xff);
    handle[0] = (unsigned char) ((msgId >> 24) & 0xff);

    return handle;
}
























 

static void handleUIRegister(WE_UINT8 srcmodule, const ubs_register_t *indata)
{
    WE_UINT8 result = UBS_RESULT_FAILURE;

    if (indata->msgType == UBS_MSG_TYPE_MMS)
    {
        if (g_numRegUBSubscribers < MMS_MAX_UI_SUBSCRIBERS )
        {
            g_regUBSubscribers[g_numRegUBSubscribers] = srcmodule;
            g_numRegUBSubscribers++;
            result = UBS_RESULT_SUCCESS;
        }
    }
    
    UBSif_replyRegister (srcmodule, 
        indata->transactionId,
        indata->msgType,
        result);
} 

static void handleUIDeregister(WE_UINT8 srcmodule, const ubs_deregister_t *indata)
{
    int       i;
    int       j;

    if (indata->msgType == UBS_MSG_TYPE_MMS)
    {
        for(i=0; i<g_numRegUBSubscribers; i++)
        {
            if (g_regUBSubscribers[i] == srcmodule)
            {
                for(j=i+1; j<g_numRegUBSubscribers; j++)
                {
                    g_regUBSubscribers[j-1] = g_regUBSubscribers[j];
                }
                --g_numRegUBSubscribers;

                return ;
            }
        }
    }

} 

static void handleUIGetNbrOfMsgs(WE_UINT8 srcmodule,
                                 const ubs_get_nbr_of_msgs_t *indata)
{
    WE_UINT16                  nbrOfMsgs = 0;
    WE_UINT16                  nbrOfNewMsgs = 0;

    if (indata->msgType == UBS_MSG_TYPE_MMS)
    {
        fldrMgrGetNumberOfMessagesUbs(
            indata->filterListSize,
            indata->filterList,
            &nbrOfMsgs,
            &nbrOfNewMsgs);
    }

    UBSif_replyNbrOfMsgs(
        srcmodule, 
        indata->transactionId,
        indata->msgType,
        nbrOfMsgs
        
        );
} 


WE_BOOL WeMmsUbs_SetMessageData(ubs_msg_list_item_t *outData, const MmsMessageFolderInfo *msgInfo, WE_BOOL getAll)
{
    const unsigned char *folder = NULL;

    if (NULL == outData)
    {
        return FALSE;
    }

    outData->handle.handle = fromIdToHandle(WE_MODID_MMS, msgInfo->msgId);
    if (NULL == outData->handle.handle)
    {
        return FALSE;
    }
    outData->handle.handleLen = 4;
    outData->titleList = WE_MEM_ALLOC(WE_MODID_MMS, sizeof(ubs_key_value_t)*MMS_NUM_TITLE_VALUES);
    if (NULL == outData->titleList)
    {
        return FALSE;
    }
    outData->titleListSize = 0;
  
    

















    


    if (TRUE == mms_ubs_titles[UBS_MSG_KEY_FOLDER].isTitleProperty || getAll == TRUE)
    {
        folder = (const unsigned char*) fldMgrFolderToString(msgInfo->folderId);
        if (folder != NULL)
        {
            set_ubs_UTF8_value(WE_MODID_MMS, &(outData->titleList[outData->titleListSize]), UBS_MSG_KEY_FOLDER, folder);
            ++outData->titleListSize;
        }
    } 

    


    if (TRUE == mms_ubs_titles[UBS_MSG_KEY_SUBTYPE].isTitleProperty || getAll == TRUE)
    {
        set_ubs_uint32_value(WE_MODID_MMS, &(outData->titleList[outData->titleListSize]), UBS_MSG_KEY_SUBTYPE, (WE_UINT32) msgInfo->suffix);
        ++outData->titleListSize;
    }

    


    if (TRUE == mms_ubs_titles[UBS_MSG_KEY_TIMESTAMP].isTitleProperty || getAll == TRUE)
    {
        set_ubs_uint32_value(WE_MODID_MMS, &(outData->titleList[outData->titleListSize]), UBS_MSG_KEY_TIMESTAMP, (WE_UINT32) msgInfo->date);
        ++outData->titleListSize;
    }

    


    if (msgInfo->suffix == 's' )
    {
        if (TRUE == mms_ubs_titles[UBS_MSG_KEY_TO].isTitleProperty || getAll == TRUE)
        {
            set_ubs_UTF8_value(WE_MODID_MMS, &(outData->titleList[outData->titleListSize]), UBS_MSG_KEY_TO, (const unsigned char*) msgInfo->address);
            ++outData->titleListSize;
        } 
    } 
    else
    {
        if (TRUE == mms_ubs_titles[UBS_MSG_KEY_FROM].isTitleProperty || getAll == TRUE)
        {
            set_ubs_UTF8_value(WE_MODID_MMS, &(outData->titleList[outData->titleListSize]), UBS_MSG_KEY_FROM, (const unsigned char*) msgInfo->address);
            ++outData->titleListSize;
        } 
    } 

    


    if (TRUE == mms_ubs_titles[UBS_MSG_KEY_SIZE].isTitleProperty || getAll == TRUE)
    {
        set_ubs_uint32_value(WE_MODID_MMS, &(outData->titleList[outData->titleListSize]), UBS_MSG_KEY_SIZE, msgInfo->size);
        ++outData->titleListSize;
    } 

    


    if (TRUE == mms_ubs_titles[UBS_MSG_KEY_SUBJECT].isTitleProperty || getAll == TRUE)
    {
        set_ubs_UTF8_value(WE_MODID_MMS, &(outData->titleList[outData->titleListSize]), UBS_MSG_KEY_SUBJECT, (const unsigned char*) msgInfo->subject);
        ++outData->titleListSize;
    }

    


    if (TRUE == mms_ubs_titles[UBS_MSG_KEY_FORWARD_LOCK].isTitleProperty || getAll == TRUE)
    {
        set_ubs_bool_value(WE_MODID_MMS, &(outData->titleList[outData->titleListSize]), UBS_MSG_KEY_FORWARD_LOCK, (WE_BOOL) (msgInfo->drmStatus & MMS_DRM_STATUS_FWDL));
        ++outData->titleListSize;
    } 

    


    if (TRUE == mms_ubs_titles[UBS_MSG_KEY_READ].isTitleProperty || getAll == TRUE)
    {
        set_ubs_bool_value(WE_MODID_MMS, 
            &(outData->titleList[outData->titleListSize]), UBS_MSG_KEY_READ,
            (msgInfo->read != 0));
        ++outData->titleListSize;
    } 

    


    if (TRUE == mms_ubs_titles[UBS_MSG_KEY_REPORT_STATUS].isTitleProperty || getAll == TRUE)
    {
        set_ubs_uint32_value(WE_MODID_MMS, &(outData->titleList[outData->titleListSize]), UBS_MSG_KEY_REPORT_STATUS, 
            (((WE_UINT32) (msgInfo->rrReq)) << 0)  |
            (((WE_UINT32) (msgInfo->rrDel)) << 8)  |
            (((WE_UINT32) (msgInfo->drReq)) << 16) |
            (((WE_UINT32) (msgInfo->drRec)) << 24));
        ++outData->titleListSize;
    } 

    


    if (TRUE == mms_ubs_titles[UBS_MSG_KEY_PRIORITY].isTitleProperty || getAll == TRUE)
    {
        if (msgInfo->priority == MMS_PRIORITY_LOW)
        {
            set_ubs_uint32_value(WE_MODID_MMS, &(outData->titleList[outData->titleListSize]), UBS_MSG_KEY_PRIORITY, UBS_MSG_KEY_VALUE_PRIORITY_LOW);
            ++outData->titleListSize;
        } 
        else if (msgInfo->priority == MMS_PRIORITY_NORMAL)
        {
            set_ubs_uint32_value(WE_MODID_MMS, &(outData->titleList[outData->titleListSize]), UBS_MSG_KEY_PRIORITY, UBS_MSG_KEY_VALUE_PRIORITY_NORMAL);
            ++outData->titleListSize;
        }
        else if (msgInfo->priority == MMS_PRIORITY_HIGH)
        {
            set_ubs_uint32_value(WE_MODID_MMS, &(outData->titleList[outData->titleListSize]), UBS_MSG_KEY_PRIORITY, UBS_MSG_KEY_VALUE_PRIORITY_HIGH);
            ++outData->titleListSize;
        }
    } 
	
	



	if (TRUE == mms_ubs_titles[UBS_MSG_KEY_CLASS].isTitleProperty || 
        getAll == TRUE)
    {
        set_ubs_bool_value(WE_MODID_MMS, 
            &(outData->titleList[outData->titleListSize]), 
            UBS_MSG_KEY_CLASS, 
            ((WE_UINT32) (msgInfo->class)));
        ++outData->titleListSize;
    } 

            
    return TRUE;
} 

#define MMS_MAX_UBS_MSG_LIST    (8)




static void handleUIGetMsgList(WE_UINT8 srcmodule,
                               const ubs_get_msg_list_t *indata)
{
    int                         i;
    WE_UINT16                  nbrOfMsgs = 0;
    WE_UINT16                  nbrOfNewMsgs = 0;
    ubs_msg_list_item_t         *outdata = NULL;
    WE_UINT16                  msgListSize = 0;
    WE_UINT16                  numActualMsg = 0;
    WE_UINT16                  nextMsg = 0;

    if (indata->msgType != UBS_MSG_TYPE_MMS)
    {
        UBSif_replyMsgList (srcmodule, 
            indata->transactionId,
            indata->msgType,
            0,
            NULL,
            0);
        return ;
    }

    fldrMgrGetNumberOfMessagesUbs(
        indata->filterListSize,
        indata->filterList,
        &nbrOfMsgs,
        &nbrOfNewMsgs);

    if (nbrOfMsgs <= indata->startFromMsg)
    {
        


        UBSif_replyMsgList (srcmodule, 
            indata->transactionId,
            indata->msgType,
            0,
            NULL,
            0);
        return ;        
    }
    
    msgListSize = (WE_UINT16) (nbrOfMsgs-indata->startFromMsg);
    if (msgListSize > MMS_MAX_UBS_MSG_LIST)
    {
        msgListSize = MMS_MAX_UBS_MSG_LIST;
    }
    
    outdata = WE_MEM_ALLOC(WE_MODID_MMS, sizeof(ubs_msg_list_item_t)*msgListSize);
    if (outdata == NULL)
    {
        UBSif_replyMsgList (srcmodule, 
            indata->transactionId,
            indata->msgType,
            0,
            NULL,
            0);
        return ;
    }
    memset(outdata, 0, sizeof(ubs_msg_list_item_t)*msgListSize);
    
    numActualMsg = fldrMgrGetUbsData(
        indata->filterListSize,
        indata->filterList,
        msgListSize,
        indata->startFromMsg,
        outdata);

    if (numActualMsg == 0)
    {
        UBSif_replyMsgList (srcmodule, 
            indata->transactionId,
            indata->msgType,
            0,
            NULL,
            0);   
    }
    else
    {
        if (indata->startFromMsg+numActualMsg < nbrOfMsgs)
        {
            nextMsg = (WE_UINT16) (indata->startFromMsg+numActualMsg);
        }
        else
        {
            nextMsg = 0;
        }
        
        UBSif_replyMsgList (srcmodule, 
            indata->transactionId,
            indata->msgType,
            numActualMsg,
            outdata,
            nextMsg);        
    }

    for(i=0; i< numActualMsg; i++)
    {
        free_ubs_msg_list_item(WE_MODID_MMS, &(outdata[i]));
    }
    WE_MEM_FREE(WE_MODID_MMS, outdata);        
} 





static void handleUIGetMsg(WE_UINT8 srcmodule,
                           const ubs_get_msg_t *inData)
{
    WE_UINT32 msgId = 0;
    ubs_msg_list_item_t tmpData;
    ubs_msg_t *outData = NULL;
    MmsMessageFolderInfo *msgInfo = NULL;
    
    if (inData->msgType != UBS_MSG_TYPE_MMS)
    {
        UBSif_replyMsg (srcmodule, 
            inData->transactionId,
            inData->msgType,
            UBS_RESULT_FAILURE,
            NULL);
        return ;
    }
  
    msgId = fromHandleToId(&(inData->handle));
    
    msgInfo = Mms_FldrMgrGetMsgInfo(msgId);
      
    if (NULL == msgInfo)
    {
        UBSif_replyMsg (srcmodule, 
            inData->transactionId,
            inData->msgType,
            UBS_RESULT_FAILURE,
            NULL);
        return ;        
    }

    outData = WE_MEM_ALLOC(WE_MODID_MMS, sizeof(ubs_msg_t));
    if (outData == NULL)
    {
        UBSif_replyMsg (srcmodule, 
            inData->transactionId,
            inData->msgType,
            UBS_RESULT_FAILURE,
            NULL);
        return ;
    }
    memset(outData, 0, sizeof(ubs_msg_t));
    
    if (FALSE == WeMmsUbs_SetMessageData(&tmpData, msgInfo, FALSE))
    {
        UBSif_replyMsg (srcmodule, 
            inData->transactionId,
            inData->msgType,
            UBS_RESULT_FAILURE,
            NULL);
        return ;        
    }

    


    outData->handle = tmpData.handle;
    outData->propertyList = tmpData.titleList;
    outData->propertyListSize = tmpData.titleListSize;

    UBSif_replyMsg (srcmodule, 
        inData->transactionId,
        inData->msgType,
        UBS_RESULT_SUCCESS,
        outData);
    

    free_ubs_msg(WE_MODID_MMS, outData);
    WE_MEM_FREE(WE_MODID_MMS, outData);        
} 





static void handleUIDeleteMsg(WE_UINT8 srcmodule,
                              ubs_delete_msg_t *indata)
{
    WE_UINT32      msgId = 0;
    MmsResult       result = MMS_RESULT_OK;	
    WE_UINT8       retVal = UBS_RESULT_FAILURE;

    if (indata->msgType != UBS_MSG_TYPE_MMS ||
        indata->handle.handle == NULL ||
        indata->handle.handleLen != 4)
    {
        UBSif_replyDeleteMsg (srcmodule,
            indata->transactionId,
            indata->msgType,
            &(indata->handle),
            retVal);
        return ;
    }
    
    msgId = fromHandleToId(&(indata->handle));

    result = fldrMgrDeleteMessage(msgId,TRUE);
    if (result == MMS_RESULT_OK)
    {
        retVal = UBS_RESULT_SUCCESS;
    }

    UBSif_replyDeleteMsg(srcmodule,
        indata->transactionId,
        indata->msgType,
        &(indata->handle),
        retVal);
} 





static void handleUBSetMsgProperty(WE_UINT8 srcmodule,
                              ubs_set_msg_property_t *indata)
{
    WE_UINT32      msgId = 0;
    MmsResult       result = MMS_RESULT_OK;	
    WE_UINT8       retVal = UBS_RESULT_FAILURE;
    WE_BOOL        bValue = FALSE;
    MmsFolderType   folderId;

    if (indata->msgType != UBS_MSG_TYPE_MMS ||
        indata->handle.handle == NULL ||
        indata->handle.handleLen != 4)
    {
        UBSif_replySetMsgProperty(srcmodule,
            indata->transactionId,
            indata->msgType,
            &(indata->handle),
            retVal);
        return ;
    }
    
    msgId = fromHandleToId(&(indata->handle));

    if (indata->propertyValue.key == UBS_MSG_KEY_FOLDER &&
        indata->propertyValue.valueType == UBS_VALUE_TYPE_UTF8)
    {
        folderId = fldMgrStringToFolder((char*)indata->propertyValue.value);
        
        if (MMS_ALL_FOLDERS != folderId)
        {
            


            result = moveMessage(folderId, msgId);
            
            if (result == MMS_RESULT_OK)
            {
                retVal = UBS_RESULT_SUCCESS;
            }
        }
    }
    else if (indata->propertyValue.key == UBS_MSG_KEY_READ &&
             indata->propertyValue.valueType == UBS_VALUE_TYPE_BOOL)
    {
        bValue = (WE_BOOL) *(indata->propertyValue.value);
        fldrMgrSetReadMark(msgId, (WE_UINT8) bValue);
        retVal = UBS_RESULT_SUCCESS;
    }

    UBSif_replySetMsgProperty(srcmodule,
        indata->transactionId,
        indata->msgType,
        &(indata->handle),
        retVal);
} 

int mmsHandleExtUBSignal(
         WE_UINT8 srcmodule,      
         WE_UINT16 signal,
         void *sigData)
{
    
    switch (signal)
    {
    

 
    case UBS_SIG_REGISTER:
        handleUIRegister(srcmodule, sigData);
        return TRUE;
    case UBS_SIG_DEREGISTER:
        handleUIDeregister(srcmodule, sigData);
        return TRUE;
    case UBS_SIG_GET_NBR_OF_MSGS:
        handleUIGetNbrOfMsgs(srcmodule, sigData);
        return TRUE;
    case UBS_SIG_GET_MSG_LIST:
        handleUIGetMsgList(srcmodule, sigData);
        return TRUE;
    case UBS_SIG_GET_MSG:
        handleUIGetMsg(srcmodule, sigData);
        return TRUE;
    case UBS_SIG_DELETE_MSG:
        handleUIDeleteMsg(srcmodule, sigData);
        return TRUE;
    case UBS_SIG_SET_MSG_PROPERTY:
        handleUBSetMsgProperty(srcmodule, sigData);
        return TRUE;
    case UBS_SIG_CHANGE_MSG:
    case UBS_SIG_GET_FULL_MSG:
    case UBS_SIG_CREATE_MSG:
        
        return TRUE;
    default:
        
        break; 
    }  
    
    return FALSE;
} 









void mmsNotifyUBSMsgChanged(WE_UINT32 msgId)
{
    int i;
    ubs_msg_list_item_t outData;
    MmsMessageFolderInfo *msgInfo = NULL;

    msgInfo = Mms_FldrMgrGetMsgInfo(msgId);
    if (NULL == msgInfo)
    {
        


        return ;
    }
    
    if (FALSE == WeMmsUbs_SetMessageData(&outData, msgInfo, FALSE))
    {
        


        return ;
    }

    


    for(i=0; i<g_numRegUBSubscribers;i++)
    {
        UBSif_notifyMsgChanged (g_regUBSubscribers[i], 
            UBS_MSG_TYPE_MMS,
            &outData);
    } 

    free_ubs_msg_list_item(WE_MODID_MMS, &outData);
} 





void mmsNotifyUBSMsgNew(WE_UINT32 msgId)
{
    int i;
    ubs_msg_list_item_t outData;
    MmsMessageFolderInfo *msgInfo = NULL;

    msgInfo = Mms_FldrMgrGetMsgInfo(msgId);
    if (NULL == msgInfo)
    {
        


        return ;
    }

    


    if (msgInfo->folderId == MMS_HIDDEN)
    {
        return ;
    }
    
    if (FALSE == WeMmsUbs_SetMessageData(&outData, msgInfo, FALSE))
    {
        


        return ;
    }

    


    for(i=0; i<g_numRegUBSubscribers;i++)
    {        
        UBSif_notifyNewMsg (g_regUBSubscribers[i], 
            UBS_MSG_TYPE_MMS,
            &outData);
    } 

    free_ubs_msg_list_item(WE_MODID_MMS, &outData);
} 




void mmsNotifyUBSMsgDeleted(WE_UINT32 msgId)
{
    int i;
    ubs_msg_handle_t handle;

    handle.handle = fromIdToHandle(WE_MODID_MMS, msgId);
    if (NULL == handle.handle)
    {
        return ;
    }
    handle.handleLen = 4;
    
    for(i=0; i<g_numRegUBSubscribers;i++)
    {
        UBSif_notifyMsgDeleted (g_regUBSubscribers[i], 
            UBS_MSG_TYPE_MMS,
            &handle);
    } 
    
    free_ubs_msg_handle(WE_MODID_MMS, &handle);
} 




void mmsNotifyUBSUnreadMsg(WE_UINT16 nbrOfUnreadMsgs)
{
    UBSif_notifyUnreadMsgCount(WE_MODID_MMS,
        UBS_MSG_TYPE_MMS,
        (unsigned) nbrOfUnreadMsgs);
} 










void mmsHandleUBSStart(void)
{
    UBSif_startInterface();
} 

#endif 


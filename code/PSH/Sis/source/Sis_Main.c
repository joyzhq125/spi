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





#include "We_Core.h"
#include "We_Mem.h"
#include "We_Cfg.h"
#include "we_prsr.h"
#include "We_Log.h"
#include "We_File.h"

#include "Mms_Def.h"
#include "Mms_Cfg.h"
#include "Mms_If.h"

#include "Sis_def.h"
#include "Sis_cfg.h"
#include "Sis_if.h"
#include "Sis_Mem.h"
#include "Sis_main.h"
#include "Sis_Isig.h"
#include "Sis_open.h"
#include "Sis_Asy.h"



















static WE_BOOL smilOpenActive;



static void smilTerminate(WE_UINT16, void **p);

static const char *sigName(WE_UINT16 signal);

static void handleOpenSmil(WE_UINT8 module, const SisOpenSmilReq *req);
static void handleCloseSmil(WE_UINT8 module, const SisCloseSmilReq *req);
static void handleGetSlideInfo(WE_UINT8 module, const SisGetSlideInfoReq *req);
static void handleSigModuleStatus(WE_UINT16 signal, void **p);














void addSlsObjectInfoListElement(SisObjectInfoList **listStart,
                                 WE_UINT32          id,             
                                 MmsContentType      *contentType,
                                 WE_UINT32          size)
{
    SisObjectInfoList *tmpListPtr = NULL;
    
    if (contentType == NULL)
    {
        return;
    } 

    
    if (*listStart == NULL)
    {
        *listStart= SIS_CALLOC(sizeof(SisObjectInfoList));
        tmpListPtr = *listStart;
    }
    else
    {
        tmpListPtr = *listStart;

        while (tmpListPtr->next != NULL)
        {
            tmpListPtr = tmpListPtr->next;
        } 
        
        tmpListPtr->next = SIS_CALLOC(sizeof(SisObjectInfoList));
        tmpListPtr = tmpListPtr->next;
    } 
            
    tmpListPtr->current.id          = id;
    (void)MMSif_copyMmsContentType(WE_MODID_SIS, 
        &tmpListPtr->current.contentType, contentType);
    tmpListPtr->current.size        = size;
   
}











void addSlsParamListElement(SlsParamList **listStart, char *name, char *value)
{

    SlsParamList *tmpListPtr     = NULL;
           
       
    if (*listStart == NULL)
    {
        *listStart= SIS_CALLOC(sizeof(SlsParamList));
        tmpListPtr = *listStart;
    }
    else
    {
        tmpListPtr = *listStart;

        while (tmpListPtr->next != NULL)
        {
            tmpListPtr = tmpListPtr->next;
        } 
        
        tmpListPtr->next = SIS_CALLOC(sizeof(SlsParamList));
        tmpListPtr = tmpListPtr->next;
    } 
            
    tmpListPtr->param.name = name;
    tmpListPtr->param.value = value;
        
}

















void addSlsSlideElementListElement(SisSlideElementList **listStart,
    SlsMediaObjectType  objectType,
    WE_UINT32    regionId,
    WE_INT32    objectId,
    WE_UINT32    fgColor,        
    WE_UINT32    bgColor,            
    WE_UINT32    beginT,         
    WE_UINT32    endT,           
    WE_UINT32    duration,
    char         *alt,           
    SlsParamList *paramList)
{

    SisSlideElementList *tmpListPtr     = NULL;
           
       
    if (*listStart == NULL)
    {
        *listStart= SIS_CALLOC(sizeof(SisSlideElementList));
        tmpListPtr = *listStart;
    }
    else
    {
        tmpListPtr = *listStart;

        while (tmpListPtr->next != NULL)
        {
            tmpListPtr = tmpListPtr->next;
        } 
        
        tmpListPtr->next = SIS_CALLOC(sizeof(SisSlideElementList));
        tmpListPtr = tmpListPtr->next;
    } 
            
    
        
    tmpListPtr->current.objectType = objectType;
    tmpListPtr->current.regionId = regionId;
    tmpListPtr->current.objectId = objectId;
    tmpListPtr->current.fgColor  = fgColor;
    tmpListPtr->current.bgColor  = bgColor;
    tmpListPtr->current.beginT   = beginT;
    tmpListPtr->current.endT     = endT;
    tmpListPtr->current.duration = duration;
    tmpListPtr->current.alt      = alt;
    tmpListPtr->current.paramList= paramList;

  
}










WE_BOOL addSlsSlideInfoListElement(SisSlideInfoList   **listStart,
    WE_UINT32           duration,
    SisSlideElementList *elementList)
{

    SisSlideInfoList *tmpListPtr = NULL;
    
    if (elementList == NULL)
    {
        return FALSE;
    } 

    
    if (*listStart == NULL)
    {
        *listStart= SIS_CALLOC(sizeof(SisSlideInfoList));
        tmpListPtr = *listStart;
    }
    else
    {
        tmpListPtr = *listStart;

        while (tmpListPtr->next != NULL)
        {
            tmpListPtr = tmpListPtr->next;
        } 
        
        tmpListPtr->next = SIS_CALLOC(sizeof(SisSlideInfoList));
        tmpListPtr = tmpListPtr->next;
    } 
            
    tmpListPtr->current.duration    = duration;    
    tmpListPtr->current.elementList = elementList;
    return TRUE;
}









void addSlsRegionListElement(SisRegionList **listStart,
    WE_UINT32     id, 
    const SisRegion     *region)
{
    SisRegionList *tmpListPtr = NULL;
    
    if (region == NULL)
    {
        return;
    } 

    
    if (*listStart == NULL)
    {
        *listStart= SIS_CALLOC(sizeof(SisRegionList));
        tmpListPtr = *listStart;
    }
    else
    {
        tmpListPtr = *listStart;

        while (tmpListPtr->next != NULL)
        {
            tmpListPtr = tmpListPtr->next;
        } 
        
        tmpListPtr->next = SIS_CALLOC(sizeof(SisRegionList));
        tmpListPtr = tmpListPtr->next;
    } 
    
    tmpListPtr->id = id;
    
    tmpListPtr->region.name              = region->name;
    tmpListPtr->region.size              = region->size;
    tmpListPtr->region.position.cx.type  = region->position.cx.type;
    tmpListPtr->region.position.cx.value = region->position.cx.value;
    tmpListPtr->region.position.cy.type  = region->position.cy.type;
    tmpListPtr->region.position.cy.value = region->position.cy.value;

    tmpListPtr->region.fit               = region->fit;
}






void slsInit(void)
{
    slsSignalInit();
    slsOpenInit();
    
    if (!slsAsyncInit()) 
    {
        
    }

    smilOpenActive = FALSE;
 
} 








static void smilTerminate(WE_UINT16 signal, void **p)
{
    
    slsOpenTerminate();
   
    slsAsyncTerminate();

    slsSignalTerminate();

    
    WE_SIGNAL_DESTRUCT(WE_MODID_SIS, signal, *p);
    *p = NULL;

    
    WE_FILE_CLOSE_ALL(WE_MODID_SIS);
    
    WE_MEM_FREE_ALL (WE_MODID_SIS);
    
    WE_MODULE_IS_TERMINATED (WE_MODID_SIS);
    
    WE_SIGNAL_DEREGISTER_QUEUE (WE_MODID_SIS);

    WE_KILL_TASK (WE_MODID_SIS);
    

} 








static void handleOpenSmil(WE_UINT8 module, const SisOpenSmilReq *req)
{
    SisResult  result   = SIS_RESULT_OK;
    WE_UINT32 userData = 0;
    
    if (req) 
    {
        if (smilOpenActive) 
        {
            userData = req->userData;
            result = SIS_RESULT_BUSY;
            
        }
        else
        {
                        
            
            slsOpenSmil(module, req->msgId, req->userData);
            
            smilOpenActive = TRUE;            
        }
    }
    else
    {             
        result = SIS_RESULT_ERROR;
        
    }
    
    if (result != SIS_RESULT_OK) 
    {
        sendCloseSmilRsp(module, result, userData);
    }
    
}







static void handleCloseSmil(WE_UINT8 module, const SisCloseSmilReq *req)
{
    SisResult  result   = SIS_RESULT_OK;
    WE_UINT32 userData = 0;
    if (req) 
    {
         if (smilOpenActive) 
         {
             
             if (slsCloseSmil(module, req->userData))
             {
                 smilOpenActive = FALSE;        
             }
             else
             {
                 userData = req->userData;
                 result = SIS_RESULT_BUSY;
                 
             }
         }
         else
         {
             userData = req->userData;
             result = SIS_RESULT_ERROR;
         }
    }
    else
    {
        result = SIS_RESULT_ERROR;
    }
    

    if (result != SIS_RESULT_OK) 
    {
        sendCloseSmilRsp(module, result, userData);
    }
    
}







static void handleGetSlideInfo(WE_UINT8 module, const SisGetSlideInfoReq *req)
{
    SisResult  result   = SIS_RESULT_OK;
    WE_UINT32 userData = 0;

    if (req) 
    {
        if (smilOpenActive) 
        {
            
            if (!slsGetSlideInfo(module, req->startSlide, 
                req->numberOfSlides, 
                req->userData))
            {
                result   = SIS_RESULT_ERROR;
                userData = req->userData;
            }
            
        }
        else
        {
            result = SIS_RESULT_BUSY;            
        }
    }
    else
    {
        result = SIS_RESULT_ERROR;
    }

    if (result != SIS_RESULT_OK) 
    {
        sendGetSlideInfoRsp(module, result, NULL, userData);
    }
                     
} 







static void handleSigModuleStatus(WE_UINT16 signal, void **p)
{
    we_module_status_t *mStatus = (we_module_status_t*)*p;

    if (mStatus != NULL)
    {
        if (MODULE_STATUS_TERMINATED == mStatus->status)
        {
            if (WE_MODID_MMS == mStatus->modId)
            {
                smilTerminate(signal, p);
            }            
        }
        else if (MODULE_STATUS_ACTIVE == mStatus->status)
        {
            
            if (WE_MODID_MMS == mStatus->modId)
            {
                WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIS, 
                    "The MMS Service is started.\n"));
                
                
                WE_MODULE_IS_ACTIVE (WE_MODID_SIS);              
            } 
            
        }
        else
        {
            WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS,
                "msfSignalHandler erroneous signal module status\n"));
        }        
    }
        
}










void slsSignalDispatcher(WE_UINT8 module, WE_UINT16 signal, void *p)
{    
    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
        "%s(%d): %s received, sender: 0x%x\n",
        __FILE__, __LINE__, sigName(signal), module));

    switch (signal)
    {
    case SIS_OPEN_SMIL_REQ:                        
        handleOpenSmil(module, (SisOpenSmilReq *)p);                
        break;        
    case SIS_CLOSE_SMIL_REQ:               
        handleCloseSmil(module, (SisCloseSmilReq *)p);        
        break;
    case SIS_GET_SLIDE_INFO_REQ:                       
        handleGetSlideInfo(module, (SisGetSlideInfoReq *)p);                        
        break;    
    case WE_SIG_MODULE_EXECUTE_COMMAND:
    case WE_SIG_MODULE_START:
        MMSif_startInterface();
        WE_MODULE_START(WE_MODID_SIS, WE_MODID_MMS, NULL, NULL, NULL);                
        break;
    case WE_SIG_MODULE_STATUS:
        handleSigModuleStatus(signal, &p);
        break;    
    case WE_SIG_MODULE_TERMINATE:        
        WE_MODULE_TERMINATE(WE_MODID_SIS, WE_MODID_MMS);       
        break;
    case WE_SIG_PIPE_NOTIFY:
        (void)SIS_SIGNAL_SENDTO_IU(SIS_FSM_ASYNC, SIS_SIG_ASYNC_PIPE_NOTIFY, 
            ((we_pipe_notify_t *)p)->eventType, 
            (WE_UINT32)(((we_pipe_notify_t *)p)->handle));
        break;
    case MMS_SIG_GET_MSG_SKELETON_REPLY:
        {
            MmsGetSkeletonReply *mmsReplyGetSkeleton = (MmsGetSkeletonReply *)p;

            (void)SIS_SIGNAL_SENDTO_IUP(SIS_FSM_OPEN, SIS_SIG_OPEN_MMS_GET_SKELETON_RSP,
                mmsReplyGetSkeleton->result,mmsReplyGetSkeleton->userData, 
                mmsReplyGetSkeleton->infoList);
            
            
            ((MmsGetSkeletonReply *)p)->infoList = NULL;
        }
        break;  
    case MMS_SIG_GET_BODY_PART_REPLY:
        {
            MmsGetBodyPartReply *mmsReplyGetBp = (MmsGetBodyPartReply *)p;

            (void)SIS_SIGNAL_SENDTO_IUUP(SIS_FSM_OPEN, SIS_SIG_OPEN_MMS_GET_BP_RSP,
                mmsReplyGetBp->result,mmsReplyGetBp->userData, 
                mmsReplyGetBp->bodyPartSize, mmsReplyGetBp->pipeName);
            
            
            ((MmsGetBodyPartReply *)p)->pipeName = NULL;           
        }    
        break;
    default:
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
            "%s(%d): Unexpected signal: 0x%x received, sender: 0x%x\n",
            __FILE__, __LINE__, signal, module));
        break;
    } 
    if (p)
    {
        WE_SIGNAL_DESTRUCT(WE_MODID_SIS, signal, p);
    } 
    
} 





static const char *sigName(WE_UINT16 signal)
{
    switch (signal)
    {
    case SIS_OPEN_SMIL_REQ:                        
        return "SIS_OPEN_SMIL_REQ";                     
    case SIS_CLOSE_SMIL_REQ:               
        return "SIS_CLOSE_SMIL_REQ";       
    case SIS_GET_SLIDE_INFO_REQ:                       
        return "SIS_GET_SLIDE_INFO_REQ";                       
    case WE_SIG_FILE_NOTIFY:
        return "WE_SIG_FILE_NOTIFY";  
    case WE_SIG_MODULE_EXECUTE_COMMAND:
        return "WE_SIG_MODULE_EXECUTE_COMMAND";
    case WE_SIG_MODULE_START:
        return "WE_SIG_MODULE_START"; 
    case WE_SIG_MODULE_STATUS:
        return "WE_SIG_MODULE_STATUS";                   
    case WE_SIG_MODULE_TERMINATE:
        return "WE_SIG_MODULE_TERMINATE";     
    case WE_SIG_PIPE_NOTIFY:
        return "WE_SIG_PIPE_NOTIFY"; 
    case MMS_SIG_GET_MSG_SKELETON_REPLY:
        return "MMS_SIG_GET_MSG_SKELETON_REPLY";  
    case MMS_SIG_GET_BODY_PART_REPLY:
        return "MMS_SIG_GET_BODY_PART_REPLY";         
    default:
        return "UNKNOWN"; 
        
    } 
} 



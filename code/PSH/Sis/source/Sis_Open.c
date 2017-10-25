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




#include "We_Lib.h"
#include "We_Core.h"

#include "We_Def.h"
#include "We_Cfg.h"
#include "We_Mem.h"
#include "We_Log.h"


#include "Mms_Def.h"
#include "Mms_Cfg.h"
#include "Mms_If.h"


#include "Sis_Mem.h"
#include "Sis_Isig.h"

#include "Sis_cfg.h"
#include "Sis_if.h"
#include "Sis_cvt.h"

#include "Sis_open.h"
#include "Sis_prse.h"
#include "Sis_main.h"

#include "Sis_Asy.h"



   


typedef struct 
{
    WE_UINT8           requester;
    WE_UINT32          msgId;

    WE_UINT8           noOfSlides;
    SisRootLayout       rootLayout;
    SisUnrefItemList   *unrefItems;
    SisRegionList      *regionList;
    SisObjectInfoList  *objectList;

    SisSlideInfoList   *slideInfoList;

    WE_UINT32          userData;
    
    MmsBodyInfoList        *bodyInfoList;
    SlsAsyncOperationHandle asyncHandle;

    char               *smilData;

}SlsOpenCtx;









#define IS_SMIL_PART(cc) (0 == we_cmmn_strcmp_nc((char *)cc->strValue,\
    MMS_MEDIA_TYPE_STRING_SMIL))




static SlsOpenCtx openCtx;



static void freeSlsOpenCtx(WE_UINT8 module, SlsOpenCtx *instance);
static void openSigHandler(SlsSignal *sig);

static WE_BOOL handleGetSkeletonRsp(SlsOpenCtx       *ctx,
    MmsResult         result, 
    WE_UINT32        userData, 
    MmsBodyInfoList  *infoList);

static WE_BOOL handleGetBodyPartRsp(SlsOpenCtx    *ctx,
    MmsResult      result, 
    WE_UINT32     userData,
    WE_UINT32     bodyPartSize,
    unsigned char *pipeName);


static WE_BOOL handleBodyPartToBufferRsp(SlsOpenCtx    *ctx,
    WE_INT16      instance,
    SlsAsioResult  result, 
    WE_UINT32     length); 





void slsOpenInit (void)
{
    slsSignalRegisterDst(SIS_FSM_OPEN, openSigHandler);
    memset(&openCtx, 0x00, sizeof(SlsOpenCtx));
    openCtx.rootLayout.bgColor=SIS_COLOR_NOT_USED;
} 






void slsOpenTerminate (void)
{
    freeSlsOpenCtx(WE_MODID_SIS, &openCtx);
    slsSignalDeregister(SIS_FSM_OPEN);
} 








static void freeSlsOpenCtx(WE_UINT8 module, SlsOpenCtx *instance)
{
    

    if(instance->unrefItems)
    {
        SISif_freeSlsUnrefItemList(module, instance->unrefItems);
        if (instance->unrefItems) 
        {
            WE_MEM_FREE(module, instance->unrefItems);
        }  
        instance->unrefItems = NULL;
    }
    if(instance->regionList)
    {
        SISif_freeSlsRegionList(module, instance->regionList);
        if (instance->regionList) 
        {
            if (instance->regionList->region.name) 
                WE_MEM_FREE(module, instance->regionList->region.name);
            WE_MEM_FREE(module, instance->regionList);
        }  
        instance->regionList = NULL;
    }

    if(instance->objectList)
    {
        SISif_freeSlsObjectInfoList(module, instance->objectList);
        if (instance->objectList) 
        {
            WE_MEM_FREE(module, instance->objectList);
        }        
        instance->objectList = NULL;
    }

    if(instance->slideInfoList)
    {
        SISif_freeSlsSlideInfoList(module, instance->slideInfoList);
        if (instance->slideInfoList) 
        {
            WE_MEM_FREE(module, instance->slideInfoList);
        }  
        instance->slideInfoList = NULL;
    }
    if(instance->bodyInfoList)
    {
        MMSif_freeMmsBodyInfoList(WE_MODID_SIS, instance->bodyInfoList);
        instance->bodyInfoList = NULL;
    }

    if (instance->smilData) 
    {
        SIS_FREE(instance->smilData);
    }
    
        
}









void sendCloseSmilRsp(WE_UINT8 destModId, SisResult result, WE_UINT32 userData)
{
    we_dcvt_t       cvt;
    void            *sig;
    void            *sigData;
    WE_UINT16       length;   
    SisCloseSmilRsp  tmpCloseRsp;
    
    
    tmpCloseRsp.result = result; 
	tmpCloseRsp.userData = userData; 

    we_dcvt_init (&cvt, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
    
    if (!Sis_cvt_SlsCloseSmilRsp(&cvt, &tmpCloseRsp)) 
    {
    }
    
    length = (WE_UINT16)cvt.pos;
    
    sig = WE_SIGNAL_CREATE (SIS_CLOSE_SMIL_RSP, WE_MODID_SIS, 
        destModId, length);
    
    sigData = WE_SIGNAL_GET_USER_DATA (sig, &length);
    
    we_dcvt_init (&cvt, WE_DCVT_ENCODE, sigData, length, WE_MODID_SIS);
    
    tmpCloseRsp.result   = result;
    tmpCloseRsp.userData = userData;
    
    if (!Sis_cvt_SlsCloseSmilRsp(&cvt, &tmpCloseRsp)) 
    {
    }

#ifdef WE_LOG_FC  
    WE_LOG_SIG_BEGIN("SIS_CLOSE_SMIL_RSP", LS_SEND, destModId, WE_MODID_SIS);
    WE_LOG_SIG_INT32("result", result);
    WE_LOG_SIG_END();
#endif

    WE_SIGNAL_SEND (sig);
}










void sendGetSlideInfoRsp(WE_UINT8           client,
    SisResult           result, 
    SisSlideInfoList   *slideInfo, 
    WE_UINT32          userData)
{
    we_dcvt_t          cvt;
    void               *sig;
    void               *sigData;
    WE_UINT16          length;
    SisGetSlideInfoRsp  slideInfoRsp;
    

    slideInfoRsp.result     = result;
    slideInfoRsp.slideInfo  = slideInfo;
    slideInfoRsp.userData   = userData;

    we_dcvt_init (&cvt, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
    
    if (!Sis_cvt_SlsGetSlideInfoRsp(&cvt, &slideInfoRsp)) 
    {

    }

    
    length = (WE_UINT16)cvt.pos;

    sig = WE_SIGNAL_CREATE (SIS_GET_SLIDE_INFO_RSP, WE_MODID_SIS, 
        client, length);

    sigData = WE_SIGNAL_GET_USER_DATA (sig, &length);

    we_dcvt_init (&cvt, WE_DCVT_ENCODE, sigData, length, WE_MODID_SIS);


    if (!Sis_cvt_SlsGetSlideInfoRsp(&cvt, &slideInfoRsp)) 
    {

    }


#ifdef WE_LOG_FC  
        WE_LOG_SIG_BEGIN("SIS_GET_SLIDE_INFO_RSP", LS_SEND, client, 
            WE_MODID_SIS);
        WE_LOG_SIG_INT32("result", result);        
        WE_LOG_SIG_END();
#endif   

    WE_SIGNAL_SEND (sig);
} 










void sendOpenSmilFailed(WE_UINT8 client, SisResult result, WE_UINT32 userData)
{
    sendOpenSmilRsp(client,
        result,   
        0,
        NULL,
        NULL,
        NULL,
        NULL,
        userData);
}













void sendOpenSmilRsp(WE_UINT8           client,
    SisResult           result,   
    WE_UINT8           noOfSlides,
    const SisRootLayout *rootLayout,
    SisUnrefItemList    *unrefItems,
    SisRegionList       *regionList,
    SisObjectInfoList   *objectList,
    WE_UINT32          userData)

{
    we_dcvt_t			cvt;
    SisOpenSmilRsp      openSmilRsp;
    void				*sig;
    void				*sigData;
    WE_UINT16		    length;

    openSmilRsp.result     = result;
    openSmilRsp.noOfSlides = noOfSlides;
    
    SISif_copySlsRootLayout(rootLayout, &openSmilRsp.rootLayout);

    if (rootLayout) 
    {
        openSmilRsp.rootLayout = *rootLayout;
    }
    else
    {
        memset(&openSmilRsp.rootLayout, 0, sizeof(SisRootLayout));
    }
    
    openSmilRsp.unrefItems = unrefItems;
    openSmilRsp.regionList = regionList;
    openSmilRsp.objectList = objectList;
    openSmilRsp.userData   = userData;

    we_dcvt_init (&cvt, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);

    if (!Sis_cvt_SlsOpenSmilRsp(&cvt, &openSmilRsp)) 
    {
        return;
    }
    length = (WE_UINT16)cvt.pos;

    sig = WE_SIGNAL_CREATE (SIS_OPEN_SMIL_RSP, WE_MODID_SIS, 
        client, length);

    sigData = WE_SIGNAL_GET_USER_DATA (sig, &length);
    
    we_dcvt_init (&cvt, WE_DCVT_ENCODE, sigData, length, WE_MODID_SIS);

    if (!Sis_cvt_SlsOpenSmilRsp(&cvt, &openSmilRsp)) 
    {
        return;
    }
    
#ifdef WE_LOG_FC  
    WE_LOG_SIG_BEGIN("SIS_OPEN_SMIL_RSP", LS_SEND, client, WE_MODID_SIS);
    WE_LOG_SIG_INT32("result", result);
    WE_LOG_SIG_END();
#endif
    
    WE_SIGNAL_SEND (sig);  

} 










static WE_BOOL getSmilBodypartNumber(MmsBodyInfoList *infoList, 
    WE_UINT32 *bodyPartId)
{
    
    MmsBodyInfoList    *tmpInfoList = NULL;
    unsigned char      *startParam  = NULL;
    MmsAllParams       *params;
    MmsEntryHeader     *eHeader;

    tmpInfoList = infoList;

    
    if (!tmpInfoList->contentType || 
        (MMS_VND_WAP_MULTIPART_RELATED != infoList->contentType->knownValue))
    {
        return FALSE;
    }

    
    params = tmpInfoList->contentType->params;
    while (params)
    {
        if ((params->param == MMS_START_REL || params->param == MMS_START_REL_14)&&
            params->type == MMS_PARAM_STRING &&
            (params->value.string != NULL))
        {
            
            startParam = params->value.string;
            while (tmpInfoList)
            {
                if (!tmpInfoList->contentType)
                {
                    
                    return FALSE;
                } 
                
                eHeader = tmpInfoList->entryHeader;
                while (eHeader)
                {
                    if (eHeader->headerType == MMS_WELL_KNOWN_CONTENT_ID)
                    {
                        
                        if (0 == we_cmmn_strcmp_nc(
                            (const char*)eHeader->value.wellKnownFieldName, 
                            (const char*)startParam))
                        {
                            if ((tmpInfoList->contentType->knownValue == 
                                MMS_VALUE_AS_STRING) &&
                                (!we_cmmn_strcmp_nc((char*)tmpInfoList->contentType->strValue, 
                                MMS_MEDIA_TYPE_STRING_SMIL)))
                            {

                                
                                *bodyPartId = (WE_UINT32)tmpInfoList->number;
                                
                                return TRUE;
                            } 
                        } 
                    } 
                     else if (eHeader->headerType == MMS_WELL_KNOWN_CONTENT_LOCATION)
                    {
                    



                        if (0 == we_cmmn_strcmp_nc(
                            (const char*)eHeader->value.wellKnownFieldName, 
                            (const char*)startParam))
                        {
                            
                            if ((tmpInfoList->contentType->knownValue == 
                                MMS_VALUE_AS_STRING) &&
                                (!we_cmmn_strcmp_nc((char*)tmpInfoList->contentType->strValue, 
                                MMS_MEDIA_TYPE_STRING_SMIL)))
                            {
                                
                                *bodyPartId = (WE_UINT32)tmpInfoList->number;
                                
                                return TRUE;
                            }
                        }
                    }
                    eHeader = eHeader->next;
                } 
                tmpInfoList= tmpInfoList->next;
            } 
        } 
        params = params->next;
    } 
    
    
    tmpInfoList = infoList;
    if (tmpInfoList)
    {
        
        tmpInfoList = tmpInfoList->next;
        while (tmpInfoList)
        {
            if (!tmpInfoList->contentType)
            {
                
                return FALSE;
            } 
            
            if ((tmpInfoList->contentType->knownValue == 
                MMS_VALUE_AS_STRING) &&
                (!we_cmmn_strcmp_nc((char*)tmpInfoList->contentType->strValue, 
                MMS_MEDIA_TYPE_STRING_SMIL)))
            {
                
                *bodyPartId = (WE_UINT32)tmpInfoList->number;
                
                return TRUE;
            } 
            tmpInfoList = tmpInfoList->next;
        } 
    } 
    
    return FALSE;
    
} 








static void addSlsUnrefItemListElement(SisUnrefItemList   **listStart,
    WE_UINT32 id)
{

    SisUnrefItemList *tmpListPtr = NULL;
    
    if (listStart == NULL)
    {
        return;
    } 

    
    if (*listStart == NULL)
    {
        *listStart= SIS_CALLOC(sizeof(SisUnrefItemList));
        tmpListPtr = *listStart;
    }
    else
    {
        tmpListPtr = *listStart;

        while (tmpListPtr->next != NULL)
        {
            tmpListPtr = tmpListPtr->next;
        } 
        
        tmpListPtr->next = SIS_CALLOC(sizeof(SisUnrefItemList));
        tmpListPtr = tmpListPtr->next;
    } 
            
    tmpListPtr->objectId = id;      
}








static WE_BOOL isUnrefItem(WE_UINT32 bodyPart, const SisSlideInfoList *slideInfoList)
{   
    SisSlideElementList    *tmpSlideElement = NULL;
   
    
    while (slideInfoList) 
    {
        tmpSlideElement = slideInfoList->current.elementList;
        
        while (tmpSlideElement) 
        {
            if (tmpSlideElement->current.objectId == (WE_INT32)bodyPart) 
            {
                return FALSE;
            }

            tmpSlideElement = tmpSlideElement->next;
        }

        slideInfoList = slideInfoList->next;
        
    }

    return TRUE;
    
}










static WE_BOOL createUnrefItemList(SisUnrefItemList **list , 
    const MmsBodyInfoList  *bodyInfoList,
    const SisSlideInfoList *slideInfoList)
{
    WE_UINT32 bodyPart;
    WE_BOOL   isSmilBodyPart = FALSE;           
       
    if (list == NULL) 
    {
        return FALSE;
    }
    
    if (bodyInfoList == NULL) 
    {
        return FALSE;
    }

    if (slideInfoList == NULL) 
    {
        return FALSE;
    }

    while(bodyInfoList)
    {
        
        if (bodyInfoList->contentType) 
        {
            isSmilBodyPart = IS_SMIL_PART(bodyInfoList->contentType);
        }

        if (!isSmilBodyPart && (bodyInfoList->numOfEntries == 0)) 
        {
            bodyPart= bodyInfoList->number;

            if(isUnrefItem(bodyPart, slideInfoList))
            {
                addSlsUnrefItemListElement(list, bodyPart);
            }
        }    
        
        isSmilBodyPart = FALSE;

        bodyInfoList = bodyInfoList->next;
    }  
    
    return TRUE;
}







WE_BOOL slsCloseSmil(WE_UINT8 module, WE_UINT32 userData)
 {
     
     
     if (module != openCtx.requester) 
     {
         return FALSE;
     }

     freeSlsOpenCtx(WE_MODID_SIS, &openCtx);

     sendCloseSmilRsp(module, SIS_RESULT_OK, userData);

     return TRUE;
 }











WE_BOOL slsGetSlideInfo(WE_UINT8  module,
    WE_UINT32 startSlide,
    WE_UINT32 numberOfSlides, 
    WE_UINT32 userData)
{

    SisSlideInfoList *tmpSlideInfoList  = NULL;
    SisSlideInfoList *endSlideInfoList  = NULL;
    SisSlideInfoList *tailSlideInfoList = NULL;
    
    WE_UINT32        i;

    if (module != openCtx.requester) 
    {
        return FALSE;
    }
    
    openCtx.userData  = userData;

    
    tmpSlideInfoList = openCtx.slideInfoList;
    for(i = 0; i < startSlide; i++)
    {
        tmpSlideInfoList = tmpSlideInfoList->next;

        if (!tmpSlideInfoList) 
        {
            sendGetSlideInfoRsp(module,
                SIS_RESULT_MISSING_MEDIA_OBJECTS, 
                NULL, 
                userData);
           return TRUE;
        }

    }

    endSlideInfoList = tmpSlideInfoList;

    for(i = 0; i < numberOfSlides && endSlideInfoList; i++)
    {
       endSlideInfoList = endSlideInfoList->next;           
    }
    

    













    if (endSlideInfoList)
    {   
    
        tailSlideInfoList = endSlideInfoList->next;
        if (endSlideInfoList->next) 
        {
        


            endSlideInfoList->next = NULL;
        }    
    }

    sendGetSlideInfoRsp(module,
        SIS_RESULT_OK, 
        tmpSlideInfoList, 
        userData);

    



    if (endSlideInfoList)
        endSlideInfoList->next = tailSlideInfoList;

    return TRUE;

}










void slsOpenSmil(WE_UINT8 module, WE_UINT32 msgId, WE_UINT32 userData)
{
    
    openCtx.requester  = module;
    openCtx.msgId      = msgId;
    openCtx.userData   = userData;

    
    MMSif_getMsgSkeleton(WE_MODID_SIS, openCtx.msgId, 0);            

} 










static WE_BOOL handleGetSkeletonRsp(SlsOpenCtx       *ctx,
    MmsResult         result, 
    WE_UINT32        userData, 
    MmsBodyInfoList  *infoList)
{
    SisResult        slsResult = SIS_RESULT_OK;
    WE_UINT32       bodyPartId; 
    MmsBodyInfoList *tmpInfoList = NULL;

      
    if(result == MMS_RESULT_OK)
    {   

        ctx->bodyInfoList = infoList;
                        
        if (!getSmilBodypartNumber(infoList, &bodyPartId)) 
        {
           slsResult = SIS_RESULT_ERROR;
        }
        else
        {
            tmpInfoList = infoList;

            while (tmpInfoList) 
            {
                                                                                                                 
                addSlsObjectInfoListElement(&ctx->objectList,
                    tmpInfoList->number, 
                    tmpInfoList->contentType, 
                    tmpInfoList->size);  

                if (!ctx->objectList) 
                {
                    slsResult = SIS_RESULT_ERROR;
                }

                tmpInfoList = tmpInfoList->next;
            }
                                  
            MMSif_getBodyPart(WE_MODID_SIS, 
                ctx->msgId, 
                (WE_UINT16)bodyPartId, 
                0);
        }
        
    }
    else
    {
       slsResult = SIS_RESULT_ERROR;
        
    }

    if (slsResult != SIS_RESULT_OK) 
    {

        sendOpenSmilFailed(ctx->requester, slsResult, userData);
        return FALSE;
    }

    return TRUE;
    
}











static WE_BOOL handleGetBodyPartRsp(SlsOpenCtx    *ctx,
    MmsResult      result, 
    WE_UINT32     userData,
    WE_UINT32     bodyPartSize,
    unsigned char *pipeName)
{

    SisResult   slsResult = SIS_RESULT_OK;

    if (result != MMS_RESULT_OK) 
    {
        slsResult = SIS_RESULT_ERROR;        
    }
    else
    {
                        
        openCtx.asyncHandle = 
            slsAsyncPipeOperationStart((const char *)pipeName, 
            SIS_FSM_OPEN, 
            0, 
            SIS_SIG_OPEN_LOAD_BP_TO_BUFFER_RSP, 
            SIS_ASYNC_MODE_READ);
        
        if (!openCtx.asyncHandle) 
        {
            slsResult = SIS_RESULT_ERROR;
                        
        }
        else
        {    
            
            ctx->smilData = SIS_CALLOC(bodyPartSize + 1);
            
            if (!ctx->smilData) 
            {
                slsResult = SIS_RESULT_ERROR;
                
                slsAsyncOperationStop(ctx->asyncHandle);                
            }
            else
            {    
                slsAsyncPipeRead(ctx->asyncHandle, 
                    ctx->smilData, 
                    bodyPartSize);
            }
        }
    }

    if (pipeName) 
    {
        WE_MEM_FREE(WE_MODID_SIS, pipeName);
    }
    
    if (slsResult != SIS_RESULT_OK) 
    {
        sendGetSlideInfoRsp(ctx->requester, slsResult, NULL, userData);
        return FALSE;
    }

    return TRUE;
    
}










static WE_BOOL handleBodyPartToBufferRsp(SlsOpenCtx    *ctx,
    WE_INT16      instance,
    SlsAsioResult  result, 
    WE_UINT32     length)    
{   
    SisResult         slsResult     = SIS_RESULT_OK;
    SlsParseResult    parseResult   = SIS_PARSE_OK;    
    

    
    instance = instance;

    if (result != SIS_ASIO_RESULT_OK) 
    {
        slsResult = SIS_RESULT_ERROR;
    }
    else
    {
        
        if (ctx->smilData[length] != '\0')
            ctx->smilData[length] = '\0';

       
        parseResult = parseSMIL(ctx->smilData, 
                        ctx->bodyInfoList, 
                        &ctx->slideInfoList, 
                        &ctx->regionList,
                        &ctx->rootLayout,
                        &ctx->noOfSlides);
        
        ctx->smilData = NULL;
        
        if (parseResult != SIS_PARSE_OK) 
        {      
            
            if (parseResult == SIS_PARSE_ERROR_TO_MANY_SLIDES) 
            {
                slsResult = SIS_RESULT_TO_MANY_SLIDES;
            }
            else
            {
                slsResult = SIS_RESULT_SMIL_INVALID;
            }
        }
        else
        {
            if (!createUnrefItemList(&ctx->unrefItems, ctx->bodyInfoList, ctx->slideInfoList)) 
            {
                ctx->unrefItems = NULL;
            }
        }

                              
    }

    if (slsResult != SIS_RESULT_OK)        
    {

        sendOpenSmilFailed(ctx->requester, slsResult, ctx->userData);
    }
    else
    {
        sendOpenSmilRsp(ctx->requester, 
            SIS_RESULT_OK, 
            ctx->noOfSlides, 
            &ctx->rootLayout, 
            ctx->unrefItems,            
            ctx->regionList, 
            ctx->objectList, 
            openCtx.userData);             
    }
   
    slsAsyncOperationStop(&ctx->asyncHandle);

    return TRUE;
    
}







static void openSigHandler(SlsSignal *sig)
{		

    
    WE_BOOL  result = TRUE;
    
    if (NULL == sig)
    {
        
        return;
    } 

    switch (sig->type)
    {    
    case SIS_SIG_OPEN_MMS_GET_SKELETON_RSP:
                     
            
        result = handleGetSkeletonRsp(&openCtx,
                    (MmsResult)sig->i_param, 
                    sig->u_param1, 
                    (MmsBodyInfoList *)sig->p_param);              
        break;
    case SIS_SIG_OPEN_MMS_GET_BP_RSP:

        result = handleGetBodyPartRsp(&openCtx,
                    (MmsResult)sig->i_param, 
                    sig->u_param1, 
                    sig->u_param2, 
                    (unsigned char *)sig->p_param);
        
        break;
    case SIS_SIG_OPEN_LOAD_BP_TO_BUFFER_RSP:

        result = handleBodyPartToBufferRsp(&openCtx,
                    (WE_INT16)sig->i_param, 
                    (SlsAsioResult)sig->u_param1, 
                    (WE_UINT32)sig->u_param2);          
     
        break;                   
    default:
        
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIS,
            "(%s) (%d) Unknown signal!\n", __FILE__, __LINE__));
        break;
    } 

    if (!result) 
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIS,
            "(%s) (%d) Failed handling (%d)!\n", __FILE__, __LINE__, sig->type));
    }

    
    slsSignalDelete(sig);

} 


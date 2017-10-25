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
#include "We_Lib.h"

#include "We_Log.h"     

#include "Mms_Def.h"
#include "Mms_Cfg.h"
#include "Mms_If.h"

#include "Sis_cfg.h"
#include "Sis_if.h"
#include "Sis_cvt.h"

#ifdef WE_LOG_FC
#include "Sis_main.h"
#include "Sis_Mem.h"
#endif































WE_BOOL Sis_cvt_SlsOpenSmilReq(we_dcvt_t *obj, SisOpenSmilReq *data)
{
    
    WE_BOOL  returnValue = TRUE;

    if (!we_dcvt_uint32(obj, &data->msgId)    || 
        !we_dcvt_uint32(obj, &data->userData))
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));

        returnValue = FALSE;
    } 

    return returnValue;
}










WE_BOOL Sis_cvt_SlsCloseSmilRsp(we_dcvt_t *obj, SisCloseSmilRsp *data)
{
    
    WE_INT32 slsResult;
    WE_BOOL  returnValue = TRUE;

    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        slsResult = (WE_INT32)data->result;
    } 

    if (!we_dcvt_int32(obj, &slsResult)                  ||
        !we_dcvt_uint32(obj, &data->userData))
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));

        returnValue = FALSE;
    } 

    data->result = (SisResult)slsResult; 

    return returnValue;
}











WE_BOOL Sis_cvt_SlsGetSlideInfoReq(we_dcvt_t *obj, SisGetSlideInfoReq *data)
{
    WE_BOOL returnValue = TRUE;

    if (!we_dcvt_uint32(obj, &data->startSlide)     ||
        !we_dcvt_uint32(obj, &data->numberOfSlides) ||
        !we_dcvt_uint32(obj, &data->userData))
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));

        returnValue = FALSE;
    } 

    return returnValue;
}











WE_BOOL Sis_cvt_SlsObjectInfo(we_dcvt_t *obj, SisObjectInfo *data)
{
    
    WE_BOOL  returnValue        = TRUE;

   
    if (!we_dcvt_uint32(obj, &data->id)                ||
        !mms_cvt_MmsContentType(obj, &data->contentType) ||
        !we_dcvt_uint32(obj, &data->size))        
     
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));

        returnValue = FALSE;
    } 

    
    return returnValue;
}










WE_BOOL Sis_cvt_SlsObjectInfoListSt(we_dcvt_t *obj, SisObjectInfoList *data)
{
    WE_BOOL returnValue = TRUE;

    if (!Sis_cvt_SlsObjectInfo(obj, &data->current))        
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));

        returnValue = FALSE;
    } 

    return returnValue;
}










WE_BOOL Sis_cvt_SlsObjectInfoList(we_dcvt_t *obj, SisObjectInfoList **data)
{
    WE_UINT16              listLength;
    WE_UINT16              i;
    SisObjectInfoList      *listPtr     = NULL; 
    SisObjectInfoList      *freePtr     = NULL;
   
   
    if (data == NULL) 
    {
        return FALSE;
    }
    else
    {
    

        if (obj->operation == WE_DCVT_ENCODE       ||
            obj->operation == WE_DCVT_FREE         ||
            obj->operation == WE_DCVT_ENCODE_SIZE) 
        {
            if(*data !=  NULL)
            {
                listPtr = *data;
                for(listLength = 1; listPtr->next != NULL; listLength++)
                {
                    listPtr = listPtr->next;
                }
            }
            else
            {
                listLength = 0;
            }             
        }
        
        
        if (!we_dcvt_uint16(obj, &listLength)) 
        {
            return FALSE;
        }
        else
        {
            if (obj->operation == WE_DCVT_DECODE) 
            {
                if (listLength > 0) 
                {
                    *data = WE_MEM_ALLOC(obj->module, sizeof(SisObjectInfoList));
                    memset(*data, 0x00, sizeof(SisObjectInfoList));
                    if ( !(*data) ) 
                    {
                        return FALSE;
                    }
                    (*data)->next = NULL;
                }
                else
                {
                    *data = NULL;
                }
                
            }
            listPtr = *data;
            
            for(i = 1; i <= listLength; i++)
            {            
                if (!Sis_cvt_SlsObjectInfoListSt(obj, listPtr)) 
                {
                    return FALSE;
                }          
                
                if ( (i + 1) <= (listLength)) 
                {
                    if (obj->operation == WE_DCVT_DECODE) 
                    {                
                        
                        listPtr->next = WE_MEM_ALLOC(obj->module, sizeof(SisObjectInfoList));
                        memset(listPtr->next, 0x00, sizeof(SisObjectInfoList));
                        
                        if (listPtr->next == NULL)
                        {
                            return FALSE;          
                        }      
                        
                        listPtr->next->next = NULL;
                    }
                }
                
                listPtr = listPtr->next;
            }
            
            if (obj->operation == WE_DCVT_FREE) 
            {
                listPtr = *data;
                for(i = 1; i <= listLength; i++)
                {
                    
                    freePtr = listPtr;                               
                    
                    
                    listPtr = listPtr->next;
                    
                    if(freePtr)
                        WE_MEM_FREE(obj->module, freePtr);
                }
            }
            
        }
    }

    return TRUE;
}










WE_BOOL Sis_cvt_SlsSize(we_dcvt_t *obj, SisSize *data)
{
    WE_BOOL returnValue = TRUE;

    if (!Sis_cvt_SlsSizeValue(obj, &data->cx) ||
        !Sis_cvt_SlsSizeValue(obj, &data->cy))
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));

        returnValue = FALSE;
    } 

    return returnValue;
}










WE_BOOL Sis_cvt_SlsRegion(we_dcvt_t *obj, SisRegion *data)
{
    WE_INT32 slsFit     = 0;
    WE_BOOL returnValue = TRUE;

    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        slsFit = (WE_INT32)data->fit;
    } 

    

    if (!we_dcvt_string(obj, &data->name)     ||
        !Sis_cvt_SlsSize(obj, &data->position) ||
        !Sis_cvt_SlsSize(obj, &data->size)     ||
        !we_dcvt_int32(obj, &slsFit))        
     
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));

        returnValue = FALSE;
    } 

    data->fit = (SisFit)slsFit; 

    return returnValue;
}










WE_BOOL Sis_cvt_SlsRegionListSt(we_dcvt_t *obj, SisRegionList *data)
{
    WE_BOOL returnValue = TRUE;

    if (!we_dcvt_uint32(obj, &data->id) ||
        !Sis_cvt_SlsRegion(obj, &data->region))
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));

        returnValue = FALSE;
    } 

    return returnValue;
}










WE_BOOL Sis_cvt_SlsRegionList(we_dcvt_t *obj, SisRegionList **data)
{
    
    WE_UINT16              listLength;
    WE_UINT16              i;
    SisRegionList          *listPtr     = NULL; 
    SisRegionList          *freePtr     = NULL;
   
   
    if (data == NULL) 
    {
        return FALSE;
    }
    else
    {
    
        if (obj->operation == WE_DCVT_ENCODE       ||
            obj->operation == WE_DCVT_FREE         ||
            obj->operation == WE_DCVT_ENCODE_SIZE) 
        {
            if(*data !=  NULL)
            {
                listPtr = *data;
                for(listLength = 1; listPtr->next != NULL; listLength++)
                {
                    listPtr = listPtr->next;
                }
            }
            else
            {
                listLength = 0;
            }             
        }
        
        
        if (!we_dcvt_uint16(obj, &listLength)) 
        {
            return FALSE;
        }
        else
        {
            if (obj->operation == WE_DCVT_DECODE) 
            {
                if (listLength > 0) 
                {
                    *data = WE_MEM_ALLOC(obj->module, sizeof(SisRegionList));
                    if ( !(*data) ) 
                    {
                        return FALSE;
                    }
                    (*data)->next = NULL;
                }
                else
                {
                    *data = NULL;
                }
                
            }
            
            listPtr = *data;
            for(i = 1; i <= listLength; i++)
            {
                
                if (!Sis_cvt_SlsRegionListSt(obj, listPtr)) 
                {
                    return FALSE;
                }    
                
                if ( (i + 1) <= (listLength)) 
                {
                    if (obj->operation == WE_DCVT_DECODE) 
                    {                
                        
                        listPtr->next = WE_MEM_ALLOC(obj->module, sizeof(SisRegionList));
                        
                        if (listPtr->next == NULL)
                        {
                            return FALSE;          
                        }                                   
                        
                        listPtr->next->next = NULL;
                    }
                }
                
                listPtr = listPtr->next;
                
            }
            
            if (obj->operation == WE_DCVT_FREE) 
            {
                listPtr = *data;
                for(i = 1; i <= listLength; i++)
                {
                    
                    freePtr = listPtr;                               
                    
                    
                    listPtr = listPtr->next;
                    
                    if(freePtr)
                        WE_MEM_FREE(obj->module, freePtr);
                }
            }
            
        }
    }

    return TRUE;
}











WE_BOOL Sis_cvt_SlsUnrefItemSt(we_dcvt_t *obj, SisUnrefItemList *data)
{
    
    if (!we_dcvt_uint32(obj, &data->objectId))
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));

        return FALSE;
    } 

    return TRUE;
}











WE_BOOL Sis_cvt_SlsUnrefItemList(we_dcvt_t *obj, SisUnrefItemList **data)
{
    WE_UINT16              listLength;
    WE_UINT16              i;
    SisUnrefItemList       *listPtr     = NULL; 
    SisUnrefItemList       *freePtr     = NULL;
   
   
    if (data == NULL) 
    {
        return FALSE;
    }
    else
    {
    
        if (obj->operation == WE_DCVT_ENCODE       ||
            obj->operation == WE_DCVT_FREE         ||
            obj->operation == WE_DCVT_ENCODE_SIZE) 
        {
            if(*data !=  NULL)
            {
                listPtr = *data;
                for(listLength = 1; listPtr->next != NULL; listLength++)
                {
                    listPtr = listPtr->next;
                }
            }
            else
            {
                listLength = 0;
            }             
        }
        
        
        if (!we_dcvt_uint16(obj, &listLength)) 
        {
            return FALSE;
        }
        else
        {
            if (obj->operation == WE_DCVT_DECODE) 
            {
                if (listLength > 0) 
                {
                    *data = WE_MEM_ALLOC(obj->module, sizeof(SisUnrefItemList));
                    if ( !(*data) ) 
                    {
                        return FALSE;
                    }
                    (*data)->next = NULL;
                }
                else
                {
                    *data = NULL;
                }
                
            }
            
            listPtr = *data;
            for(i = 1; i <= listLength; i++)
            {
                
                if (!Sis_cvt_SlsUnrefItemSt(obj, listPtr)) 
                {
                    return FALSE;
                }    
                
                if ( (i + 1) <= (listLength)) 
                {
                    if (obj->operation == WE_DCVT_DECODE) 
                    {                
                        
                        listPtr->next = 
                            WE_MEM_ALLOC(obj->module, sizeof(SisUnrefItemList));
                        
                        if (listPtr->next == NULL)
                        {
                            return FALSE;          
                        }                                   
                        
                        listPtr->next->next = NULL;
                    }
                }
                
                listPtr = listPtr->next;
                
            }
            
            if (obj->operation == WE_DCVT_FREE) 
            {
                listPtr = *data;
                for(i = 1; i <= listLength; i++)
                {
                    
                    freePtr = listPtr;                               
                    
                    
                    listPtr = listPtr->next;
                    
                    if(freePtr)
                        WE_MEM_FREE(obj->module, freePtr);
                }
            }
            
        }
    }

    return TRUE;
}












WE_BOOL Sis_cvt_SlsSizeValue(we_dcvt_t *obj, SisSizeValue *data)
{
    WE_BOOL returnValue = TRUE;

    if (!we_dcvt_uint16(obj, &data->value) ||
        !we_dcvt_uint16(obj, &data->type))
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));

        returnValue = FALSE;
    } 

    return returnValue;
}










WE_BOOL Sis_cvt_SlsRootLayout(we_dcvt_t *obj, SisRootLayout *data)
{
    WE_BOOL returnValue = TRUE;
   

    if (!we_dcvt_uint32(obj, &data->bgColor) ||
        !Sis_cvt_SlsSizeValue(obj, &data->cx) ||
        !Sis_cvt_SlsSizeValue(obj, &data->cy))
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));

        returnValue = FALSE;
    } 



    return returnValue;
}










WE_BOOL Sis_cvt_SlsOpenSmilRsp(we_dcvt_t *obj, SisOpenSmilRsp *data)
{
    WE_BOOL  returnValue = TRUE;
    WE_INT32 slsResult;

 
    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        slsResult = (WE_INT32)data->result;
    } 

    if (!we_dcvt_int32(obj, &slsResult)                  ||
        !we_dcvt_uint8(obj, &data->noOfSlides)           ||
        !Sis_cvt_SlsRootLayout(obj, &data->rootLayout)    ||
        !Sis_cvt_SlsUnrefItemList(obj, &data->unrefItems)  ||
        !Sis_cvt_SlsRegionList(obj, &data->regionList)     ||
        !Sis_cvt_SlsObjectInfoList(obj, &data->objectList) ||
        !we_dcvt_uint32(obj, &data->userData))
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));

        returnValue = FALSE;
    } 

    
    data->result = (SisResult)slsResult; 
    
    return returnValue;
} 










WE_BOOL Sis_cvt_SlsParam(we_dcvt_t *obj, SlsParam *data)
{    
    WE_BOOL    returnValue = TRUE;

    if(!we_dcvt_string(obj, &data->name))
    {
        returnValue = FALSE;
    }
    else
    {
        if (!we_dcvt_string(obj, &data->value)) 
        {
            returnValue = FALSE;
        }
    }

    if (!returnValue) 
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
    }  

    return returnValue;
} 









WE_BOOL Sis_cvt_SlsParamListSt(we_dcvt_t *obj, SlsParamList *data)
{
    WE_BOOL  returnValue = TRUE;

    if (!Sis_cvt_SlsParam(obj, &data->param))
        
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));

        returnValue = FALSE;
    } 
    
    
    return returnValue;
} 








WE_BOOL Sis_cvt_SlsParamList(we_dcvt_t *obj, SlsParamList **data)
{
    WE_UINT16              listLength;
    WE_UINT16              i;
    SlsParamList           *listPtr     = NULL; 
    SlsParamList           *freePtr     = NULL;
   
    if (data == NULL) 
    {
        return FALSE;
    }
    else
    {

        if (obj->operation == WE_DCVT_ENCODE       ||
            obj->operation == WE_DCVT_FREE         ||
            obj->operation == WE_DCVT_ENCODE_SIZE) 
        {
            if(*data !=  NULL)
            {
                listPtr = *data;            
                for(listLength = 1; listPtr->next != NULL; listLength++)
                {
                    listPtr = listPtr->next;
                }
            }
            else
            {
                listLength = 0;
            }             
        }
        
        
        if (!we_dcvt_uint16(obj, &listLength)) 
        {
            return FALSE;
        }
        else
        {
            if (obj->operation == WE_DCVT_DECODE) 
            {
                if (listLength > 0) 
                {
                    *data = WE_MEM_ALLOC(obj->module, sizeof(SlsParamList));
                    if ( !(*data) ) 
                    {
                        return FALSE;
                    }
                    (*data)->next = NULL;
                }
                else
                {
                    *data = NULL;
                }
                
            }
            listPtr = *data;
            
            for(i = 1; i <= listLength; i++)
            {           
                if (!Sis_cvt_SlsParamListSt(obj, listPtr)) 
                {
                    return FALSE;
                }          
                
                if ( (i + 1) <= (listLength)) 
                {
                    if (obj->operation == WE_DCVT_DECODE) 
                    {                
                        
                        listPtr->next = 
                            WE_MEM_ALLOC(obj->module, sizeof(SlsParamList));
                        
                        if (listPtr->next == NULL)
                        {
                            return FALSE;          
                        }      
                        
                        listPtr->next->next = NULL;
                    }
                }
                
                listPtr = listPtr->next;
            }
            
            if (obj->operation == WE_DCVT_FREE) 
            {
                listPtr = *data;
                for(i = 1; i <= listLength; i++)
                {
                    
                    freePtr = listPtr;                               
                    
                    
                    listPtr = listPtr->next;
                    
                    if(freePtr)
                        WE_MEM_FREE(obj->module, freePtr);
                }
            }
            
        }

    }
    return TRUE;
} 










WE_BOOL Sis_cvt_SlsSlideElement(we_dcvt_t *obj, SisSlideElement *data)
{
    WE_INT32   slsMediaObjectType  = 0;
    WE_BOOL    returnValue         = TRUE;

    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        slsMediaObjectType = (WE_INT32)data->objectType;
    } 

    if (!we_dcvt_int32(obj, &slsMediaObjectType)   ||
        !we_dcvt_uint32(obj, &data->regionId)      ||
        !we_dcvt_int32(obj, &data->objectId)      ||
        !we_dcvt_uint32(obj, &data->fgColor)       ||
        !we_dcvt_uint32(obj, &data->bgColor)       ||
        !we_dcvt_uint32(obj, &data->beginT)        ||
        !we_dcvt_uint32(obj, &data->endT)          ||
        !we_dcvt_uint32(obj, &data->duration))
        
    {                
        returnValue = FALSE;
    }
    else
    {

        if (!we_dcvt_string(obj, &data->alt)) 
        {
            returnValue = FALSE;        
        }  
        else
        {
            if (!Sis_cvt_SlsParamList(obj, &data->paramList)) 
            {
                returnValue = FALSE;
            }
        }
    }

    if (!returnValue) 
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
    }

    data->objectType = (SlsMediaObjectType)slsMediaObjectType;

    return returnValue;

} 











WE_BOOL Sis_cvt_SlsSlideElementSt(we_dcvt_t *obj, SisSlideElementList *data)
{
    WE_BOOL  returnValue = TRUE;

    if (!Sis_cvt_SlsSlideElement(obj, &data->current))
        
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));

        returnValue = FALSE;
    } 
    
    
    return returnValue;
} 










WE_BOOL Sis_cvt_SlsSlideElementList(we_dcvt_t *obj, SisSlideElementList **data)
{
    WE_UINT16              listLength;
    WE_UINT16              i;
    SisSlideElementList    *listPtr     = NULL; 
    SisSlideElementList    *freePtr     = NULL;
   
    if (data == NULL) 
    {
        return FALSE;
    }
    else
    {
        if (obj->operation == WE_DCVT_ENCODE       ||
            obj->operation == WE_DCVT_FREE         ||
            obj->operation == WE_DCVT_ENCODE_SIZE) 
        {
            if(*data !=  NULL)
            {
                listPtr = *data;
                for(listLength = 1; listPtr->next != NULL; listLength++)
                {
                    listPtr = listPtr->next;
                }
            }
            else
            {
                listLength = 0;
            }             
        }
        
        
        if (!we_dcvt_uint16(obj, &listLength)) 
        {
            return FALSE;
        }
        else
        {
            if (obj->operation == WE_DCVT_DECODE) 
            {
                if (listLength > 0) 
                {
                    *data = WE_MEM_ALLOC(obj->module, sizeof(SisSlideElementList));
                    if ( !(*data) ) 
                    {
                        return FALSE;
                    }
                    (*data)->next = NULL;
                }
                else
                {
                    *data = NULL;
                }
                
            }
            listPtr = *data;
            for(i = 1; i <= listLength; i++)
            {
                
                if (!Sis_cvt_SlsSlideElementSt(obj, listPtr)) 
                {
                    return FALSE;
                }          
                
                if ( (i + 1) <= (listLength)) 
                {
                    if (obj->operation == WE_DCVT_DECODE) 
                    {                
                        
                        listPtr->next = 
                            WE_MEM_ALLOC(obj->module, sizeof(SisSlideElementList));
                        
                        if (listPtr->next == NULL)
                        {
                            return FALSE;          
                        }      
                        
                        listPtr->next->next = NULL;
                    }
                }
                
                listPtr = listPtr->next;
            }
            
            if (obj->operation == WE_DCVT_FREE) 
            {
                listPtr = *data;
                for(i = 1; i <= listLength; i++)
                {
                    
                    freePtr = listPtr;                               
                    
                    
                    listPtr = listPtr->next;
                    
                    if(freePtr)
                        WE_MEM_FREE(obj->module, freePtr);
                }
            }
            
        }
    }

    return TRUE;

} 










WE_BOOL Sis_cvt_SlsSlideInfo(we_dcvt_t *obj, SisSlideInfo *data)
{
    WE_BOOL  returnValue = TRUE;
         
    if (!we_dcvt_uint32(obj, &data->duration)                  ||
        !Sis_cvt_SlsSlideElementList(obj, &data->elementList))
        
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));

        returnValue = FALSE;
    } 

    
    return returnValue;
} 











WE_BOOL Sis_cvt_SlsSlideInfoListSt(we_dcvt_t *obj, SisSlideInfoList *data)
{
    WE_BOOL  returnValue = TRUE;

    if (!Sis_cvt_SlsSlideInfo(obj, &data->current))
        
    {   
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));

        returnValue = FALSE;
    } 
    
    
    return returnValue;
} 










WE_BOOL Sis_cvt_SlsSlideInfoList(we_dcvt_t *obj, SisSlideInfoList **data)
{
    WE_UINT16              listLength;
    WE_UINT16              i;
    SisSlideInfoList       *listPtr     = NULL; 
    SisSlideInfoList       *freePtr     = NULL;
   
    if (data == NULL) 
    {
        return FALSE;
    }
    else
    {
        if (obj->operation == WE_DCVT_ENCODE       ||
            obj->operation == WE_DCVT_FREE         ||
            obj->operation == WE_DCVT_ENCODE_SIZE) 
        {
            if(*data !=  NULL)
            {
                listPtr = *data;
                for(listLength = 1; listPtr->next != NULL; listLength++)
                {
                    listPtr = listPtr->next;
                }
            }
            else
            {
                listLength = 0;
            }             
        }
        
        
        if (!we_dcvt_uint16(obj, &listLength)) 
        {
            return FALSE;
        }
        else
        {
            if (obj->operation == WE_DCVT_DECODE) 
            {
                if (listLength > 0) 
                {
                    *data = WE_MEM_ALLOC(obj->module, sizeof(SisSlideInfoList));
                    if ( !(*data) ) 
                    {
                        return FALSE;
                    }
                    (*data)->next = NULL;
                }
                else
                {
                    *data = NULL;
                }
                
            }
            listPtr = *data;
            for(i = 1; i <= listLength; i++)
            {            
                if (!Sis_cvt_SlsSlideInfoListSt(obj, listPtr)) 
                {
                    return FALSE;
                }          
                
                if ( (i + 1) <= (listLength)) 
                {
                    if (obj->operation == WE_DCVT_DECODE) 
                    {                
                        
                        listPtr->next = 
                            WE_MEM_ALLOC(obj->module, sizeof(SisSlideInfoList));
                        
                        if (listPtr->next == NULL)
                        {
                            return FALSE;          
                        }      
                        
                        listPtr->next->next = NULL;
                    }
                }
                
                listPtr = listPtr->next;
            }
            
            if (obj->operation == WE_DCVT_FREE) 
            {
                listPtr = *data;
                for(i = 1; i <= listLength; i++)
                {
                    
                    freePtr = listPtr;                               
                    
                    
                    listPtr = listPtr->next;
                    
                    if(freePtr)
                        WE_MEM_FREE(obj->module, freePtr);
                }
            }
            
        }
    }

    return TRUE;

} 










WE_BOOL Sis_cvt_SlsGetSlideInfoRsp(we_dcvt_t *obj, SisGetSlideInfoRsp *data)
{
    
    WE_INT32 slsResult;
    WE_BOOL  returnValue = TRUE;

    if ( obj != NULL && obj->operation != WE_DCVT_DECODE)
    {
        slsResult = (WE_INT32)data->result;
    } 

    if (!we_dcvt_int32(obj, &slsResult)                  ||
        !Sis_cvt_SlsSlideInfoList(obj, &data->slideInfo)  ||
        !we_dcvt_uint32(obj, &data->userData))
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));

        returnValue = FALSE;
    } 

    data->result = (SisResult)slsResult; 
    
    return returnValue;
} 








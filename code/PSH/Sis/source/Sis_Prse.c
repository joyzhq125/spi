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
#include "We_Chrs.h"
#include "We_Chrt.h"
#include "We_Lib.h"

#include "Mms_Def.h"

#include "Sis_def.h"
#include "Sis_cfg.h"
#include "Sis_if.h"
#include "Sis_Mem.h"
#include "Sis_main.h"
#include "Sis_prse.h"







typedef struct  
{
    
    int                 *tagStackPtr;
    int                  elementStack[MAX_ELEM_DEPTH];

    WE_UINT8           slides;
    
    
    MmsBodyInfoList     *bodyInfoList;
    
    
    SisSlideInfo        tmpSlideInfo;
    SisSlideElement     tmpSlideElement;
    SlsParamList       *tmpParamList;

    SlsRegionAdmList    *regionAdmList;

    
    SisSlideInfoList    **slideInfoList;
    SisRegionList       **regionList;
    SisRootLayout       *rootLayout;

} SlsPrseInfo;


typedef struct 
{
	const char *colorName;
	const WE_UINT32 colorValue;
}SlsColor;

const SlsColor slsColorSet[] = SIS_COLOR_SET;





#define SMIL_ATTRVAL_FILL       0x0A00
#define SMIL_ATTRVAL_HIDDEN     0x0A01
#define SMIL_ATTRVAL_MEET       0x0A02
#define SMIL_ATTRVAL_SCROLL     0x0A03
#define SMIL_ATTRVAL_SLICE      0x0A04

#define SMIL_ATTR_PARAM_FG_COLOR_1	"foregroundcolor"
#define SMIL_ATTR_PARAM_FG_COLOR_2	"foreground-color"
#define SMIL_ATTR_PARAM_FG_COLOR_3  "TextColor"



static const we_strtable_entry_t smil_Elements_stringTable[] = {
  {"smil",            SMIL_ELEMENT_SMIL},
  {"head",            SMIL_ELEMENT_HEAD},
  {"meta",            SMIL_ELEMENT_META},
  {"layout",          SMIL_ELEMENT_LAYOUT},
  {"root-layout",     SMIL_ELEMENT_ROOT_LAYOUT},
  {"region",          SMIL_ELEMENT_REGION},
  {"body",            SMIL_ELEMENT_BODY},
  {"par",             SMIL_ELEMENT_PAR},
  {"text",            SMIL_ELEMENT_TEXT},
  {"img",             SMIL_ELEMENT_IMG},
  {"audio",           SMIL_ELEMENT_AUDIO},  
  {"ref",             SMIL_ELEMENT_REF},
  {"video",           SMIL_ELEMENT_VIDEO},
  {"param",           SMIL_ELEMENT_PARAM}
 };






static const unsigned char smil_Elements_hashTable[] = 
{
  4, 0, 13, 255, 3, 255, 255, 7, 5, 8,
  11, 10, 255, 255, 1, 9, 255, 255, 6, 2,
  12, 255, 255
};

#define SMIL_ELEMENTS_HASH_TABLE_SIZE                 23
#define SMIL_ELEMENTS_STRING_TABLE_SIZE               14




const we_strtable_info_t smil_Elements = 
{
  smil_Elements_hashTable,
  SMIL_ELEMENTS_HASH_TABLE_SIZE,
  WE_TABLE_SEARCH_LINEAR,
  smil_Elements_stringTable,
  SMIL_ELEMENTS_STRING_TABLE_SIZE
};





static const we_strtable_entry_t smil_Attributes_stringTable[] = {
  {"content",			SMIL_ATTRIBUTE_CONTENT},
  {"name",				SMIL_ATTRIBUTE_NAME},
  {"width",				SMIL_ATTRIBUTE_WIDTH},
  {"height",			SMIL_ATTRIBUTE_HEIGHT},
  {"fit",				SMIL_ATTRIBUTE_FIT},
  {"id",				SMIL_ATTRIBUTE_ID},
  {"left",				SMIL_ATTRIBUTE_LEFT},
  {"top",				SMIL_ATTRIBUTE_TOP},
  {"dur",				SMIL_ATTRIBUTE_DUR},
  {"src",				SMIL_ATTRIBUTE_SRC},
  {"region",			SMIL_ATTRIBUTE_REGION},
  {"alt",				SMIL_ATTRIBUTE_ALT},
  {"begin",				SMIL_ATTRIBUTE_BEGIN},
  {"end",				SMIL_ATTRIBUTE_END},
  {"background-color",	SMIL_ATTRIBUTE_BG_COLOR},
  {"value",				SMIL_ATTRIBUTE_VALUE}
};




static const unsigned char smil_Attributes_hashTable[] = 
{
  11, 255, 255, 3, 15, 1, 12, 13, 255, 9,
  10, 7, 255, 8, 255, 0, 255, 255, 5, 2,
  14, 255, 255, 255, 255, 4, 255, 255, 6
};

#define SMIL_ATTR_HASH_TABLE_SIZE                 29
#define SMIL_ATTR_STRING_TABLE_SIZE               16



const we_strtable_info_t smil_Attributes = {
  smil_Attributes_hashTable,
  SMIL_ATTR_HASH_TABLE_SIZE,
  WE_TABLE_SEARCH_LINEAR,
  smil_Attributes_stringTable,
  SMIL_ATTR_STRING_TABLE_SIZE
};




static const we_strtable_entry_t smil_Attrvals_stringTable[] = {
  {"fill",            SMIL_ATTRVAL_FILL},
  {"hidden",          SMIL_ATTRVAL_HIDDEN},
  {"meet",            SMIL_ATTRVAL_MEET},
  {"scroll",          SMIL_ATTRVAL_SCROLL},
  {"slice",           SMIL_ATTRVAL_SLICE}
};




static const unsigned char smil_Attrvals_hashTable[] = {
  3, 2, 1, 255, 255, 0, 4
};

#define SMIL_ATTR_VAL_HASH_TABLE_SIZE                 7
#define SMIL_ATTR_VAL_STRING_TABLE_SIZE               5



const we_strtable_info_t smil_Attrvals = {
  smil_Attrvals_hashTable,
  SMIL_ATTR_VAL_HASH_TABLE_SIZE,
  WE_TABLE_SEARCH_LINEAR,
  smil_Attrvals_stringTable,
  SMIL_ATTR_VAL_STRING_TABLE_SIZE
};



const unsigned char smil_Element_table[] = {
  WE_PRSR_CANCELSPACE | WE_PRSR_TEXT, 
  WE_PRSR_CANCELSPACE | WE_PRSR_TEXT, 
  WE_PRSR_CANCELSPACE | WE_PRSR_TEXT, 
  WE_PRSR_CANCELSPACE | WE_PRSR_TEXT, 
  WE_PRSR_CANCELSPACE | WE_PRSR_TEXT, 
  WE_PRSR_CANCELSPACE | WE_PRSR_TEXT, 
  WE_PRSR_CANCELSPACE | WE_PRSR_TEXT, 
  WE_PRSR_CANCELSPACE | WE_PRSR_TEXT, 
  WE_PRSR_CANCELSPACE | WE_PRSR_TEXT, 
  WE_PRSR_CANCELSPACE | WE_PRSR_TEXT, 
  WE_PRSR_CANCELSPACE | WE_PRSR_TEXT, 
  WE_PRSR_CANCELSPACE | WE_PRSR_TEXT, 
  WE_PRSR_CANCELSPACE | WE_PRSR_TEXT, 
  WE_PRSR_CANCELSPACE | WE_PRSR_TEXT  
};


const WE_UINT16 smil_AttributeTypes[] = {
  WE_PRSR_TYPE_CDATA,  
  WE_PRSR_TYPE_CDATA,  
  WE_PRSR_TYPE_CDATA,  
  WE_PRSR_TYPE_CDATA,  
  WE_PRSR_TYPE_CDATA,  
  WE_PRSR_TYPE_ENUM_A, 
  WE_PRSR_TYPE_CDATA,  
  WE_PRSR_TYPE_ID,     
  WE_PRSR_TYPE_CDATA,  
  WE_PRSR_TYPE_CDATA,  
  WE_PRSR_TYPE_IDREF,  
  WE_PRSR_TYPE_CDATA,  
  WE_PRSR_TYPE_CDATA,  
  WE_PRSR_TYPE_CDATA,  
  WE_PRSR_TYPE_CDATA,  
  WE_PRSR_TYPE_CDATA   
};










#define DEFAULT_DURATION 0
#define STR_SIZE 32





static SlsPrseInfo     slsParser;


static int startElementCallback(void*, int, we_prsr_attribute_t*, int);
static int endElementCallback(void*);
static int charDataCallback(void* ,const char*, int, int);
SisFit getFitAttr(WE_UINT32 val);









void freeSlsRegionAdmList(SlsRegionAdmList * list)
{
    SlsRegionAdmList * prev;
    SlsRegionAdmList * curr;

    curr = list;

    if (curr) 
    {
        while (curr) 
        {
            prev = curr;
            curr = curr->next;

            SIS_FREE(prev->region);
            SIS_FREE(prev);
            
        }

       
    }
}








void addSlsRegionAdmListElement(SlsRegionAdmList **listStart, 
    WE_UINT32 regionId, 
    const char *region)
{
    
    SlsRegionAdmList *tmpListPtr     = NULL;
           
    if (region == NULL)
    {
        return;
    } 

    
    if (*listStart == NULL)
    {
        *listStart= SIS_CALLOC(sizeof(SlsRegionAdmList));
        tmpListPtr = *listStart;
    }
    else
    {
        tmpListPtr = *listStart;

        while (tmpListPtr->next != NULL)
        {
            tmpListPtr = tmpListPtr->next;
        } 
        
        tmpListPtr->next = SIS_CALLOC(sizeof(SlsRegionAdmList));
        tmpListPtr = tmpListPtr->next;
    } 
            
    tmpListPtr->regionId = regionId;
    tmpListPtr->region   = (char *)we_cmmn_strdup(WE_MODID_SIS, region);           
}








WE_UINT32 getRegionIdFromRegion(const char *region)
{
    WE_UINT32          id         = 0;
    SlsRegionAdmList   *tmpListPtr = slsParser.regionAdmList; 

    if (region) 
    {
        while(tmpListPtr)
        {
            
            if (tmpListPtr->region) 
            {
                if(we_cmmn_strcmp_nc(tmpListPtr->region, region) == 0)
                {
                


                    return tmpListPtr->regionId;
                }
            }

            tmpListPtr = tmpListPtr->next;            
            id++;
        }
       
        addSlsRegionAdmListElement(&slsParser.regionAdmList, id, region);
        
    }

    return id;
}








 
WE_INT32 getObjectIdFromSrc(MmsBodyInfoList *info, const char *src)
{
    MmsBodyInfoList *tmpInfo    = NULL;
    MmsEntryHeader  *tmpHdr     = NULL;	
    const char      *tmpSrc     = NULL;
    MmsAllParams    *tmpParam   = NULL;
   
    
    WE_UNUSED_PARAMETER(tmpSrc);
    WE_UNUSED_PARAMETER(tmpParam);


    tmpInfo = info;

    if (!tmpInfo)	
    {
        return SIS_BODYPART_NOT_FOUND;
    }

    
    if (we_cmmn_strncmp_nc(src, "cid:", 4) == 0)
    {
        
        if (src[4] == 0)
        {
            return SIS_BODYPART_NOT_FOUND;
        } 

        while (tmpInfo)
        {
            tmpHdr = tmpInfo->entryHeader;
            while (tmpHdr)
            {
                if (MMS_WELL_KNOWN_CONTENT_ID == tmpHdr->headerType)
                {
                    if (tmpHdr->value.wellKnownFieldName)
                    {
                        if (0 == we_cmmn_strcmp_nc(
                            (char*)tmpHdr->value.wellKnownFieldName, 
                            (char*)src + 4))
                        {                            
                           
                            return (WE_INT32)tmpInfo->number;
                            
                        } 
                            						                                                                               
                    } 
                } 			
                tmpHdr = tmpHdr->next;
            } 
            tmpInfo = tmpInfo->next;
        } 
    } 
    
    else
    {
        
        if (src[0] == 0)
        {
            return SIS_BODYPART_NOT_FOUND;
        } 

        while (tmpInfo)
        {
            tmpHdr = tmpInfo->entryHeader;
            while (tmpHdr)
            {
                if (MMS_WELL_KNOWN_CONTENT_LOCATION == tmpHdr->headerType ||
                    MMS_WELL_KNOWN_CONTENT_ID == tmpHdr->headerType)
                {
                    if (tmpHdr->value.wellKnownFieldName)
                    {
                        if (0 == we_cmmn_strcmp_nc(
							(char*)tmpHdr->value.wellKnownFieldName,
                            (char*)src))
                        {
                                                        
                            return (WE_INT32)tmpInfo->number;
                        } 
                    } 		
                } 
                tmpHdr = tmpHdr->next;
            } 
            tmpInfo = tmpInfo->next;
        } 
    }
    
#ifdef SIS_FIND_BODY_PART_BY_CT
      tmpInfo = info;

    


    if (we_cmmn_strncmp_nc(src, "cid:", 4) == 0)
    {
        tmpSrc =src+4;
    }            
    else
    {
        tmpSrc = src;
    }

    if (*tmpSrc == 0)
    {
        return SIS_BODYPART_NOT_FOUND;
    } 
    
    


    while (tmpInfo)
    {
        tmpParam = tmpInfo->contentType->params;
        while (tmpParam)
        {
            if (MMS_NAME == tmpParam->param || MMS_NAME_14 == tmpParam->param)
            {
                if (tmpParam->type == MMS_PARAM_STRING &&
                    tmpParam->value.string != NULL)
                {
                    if (0 == we_cmmn_strcmp_nc(
                        (char*)tmpParam->value.string,
                        (char*)tmpSrc))
                    {
                        
                        return (WE_INT32)tmpInfo->number;
                    } 
                } 		
            } 
            tmpParam = tmpParam->next;
        } 
        tmpInfo = tmpInfo->next;
    } 
       
    


    tmpInfo = info;

    while (tmpInfo)
    {
        tmpParam = tmpInfo->contentType->params;
        while (tmpParam)
        {
            if (MMS_FILENAME == tmpParam->param || MMS_FILENAME_14 == tmpParam->param)
            {
                if (tmpParam->type == MMS_PARAM_STRING &&
                    tmpParam->value.string != NULL)
                {
                    if (0 == we_cmmn_strcmp_nc(
                        (char*)tmpParam->value.string,
                        (char*)tmpSrc))
                    {
                        
                        return (WE_INT32)tmpInfo->number;
                    } 
                } 		
            } 
            tmpParam = tmpParam->next;
        } 
        tmpInfo = tmpInfo->next;
    } 

#endif
    return SIS_BODYPART_NOT_FOUND;
} 









static void pushElement(int type)
{
    if ((slsParser.tagStackPtr + 1) == (slsParser.elementStack + MAX_ELEM_DEPTH))
    {
        
    }
    else if (SMIL_ELEMENT_NOT_VALID == *slsParser.tagStackPtr)
    {
        *slsParser.tagStackPtr = type;
    }
    else
    {
        *++slsParser.tagStackPtr = type;
    }
} 






static int popElement(void)
{
    if (slsParser.tagStackPtr == slsParser.elementStack)
    {
        int i = *slsParser.tagStackPtr;
        *slsParser.tagStackPtr = SMIL_ELEMENT_NOT_VALID;
        return i;
    }
    else
    {
        return *slsParser.tagStackPtr--;
    }
} 






static void elementStackInit(void)
{
    slsParser.tagStackPtr = slsParser.elementStack;
    *slsParser.tagStackPtr = SMIL_ELEMENT_NOT_VALID;
} 










static char *getStrAttr(const we_prsr_attribute_t *attr, int numAttrs, int type)
{
    while (numAttrs--)
    {
        if (attr->type == type)
        {
            if (WE_PRSR_ATTRVAL_STR == attr->flag)
            {
                if (attr->u.s)
                {
                    return we_cmmn_strdup(WE_MODID_SIS, attr->u.s);
                }
                else
                {
                    return NULL;
                }
            }
        }
        attr++;
    }
    return NULL;
} 












static int getIntAttr(const we_prsr_attribute_t *attr, int numAttrs, int type,
	WE_UINT16 *sResult, WE_UINT32  *lResult, char **extra)
{
    char value[STR_SIZE];
    int i;
    *extra = NULL;      
    while (numAttrs--)
    {
        if (attr->type == type)
        {
            if (WE_PRSR_ATTRVAL_STR == attr->flag)
            {
                if (attr->u.s)
                {
                    for (i = 0; i < STR_SIZE && 
                        attr->u.s[i] && ct_isdigit(attr->u.s[i]); i++)
                    {
                        value[i] = attr->u.s[i];
                    }
                    if (i == STR_SIZE)
                    {
                        value[STR_SIZE - 1] = 0;
                    }
                    else
                    {
                        value[i] = 0;
                    }
                    if (sResult) 
                    {
                        *sResult = (WE_UINT16)atoi(value);
                    }
                    if (lResult) 
                    {
                        *lResult = (WE_UINT32)atol(value);
                    }
                    if (attr->u.s[i])
                    {
                        *extra = attr->u.s + i;
                    }
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
			if (WE_PRSR_ATTRVAL_INT == attr->flag)
            {
				if (attr->u.i)
                {
					*lResult = (WE_UINT32)attr->u.i;
					if (NULL != sResult)
					{
						sResult = 0;
					}
					return 1;
				}
			}
        }
        attr++;
    }
    return 0;
} 






SisFit getFitAttr(WE_UINT32 val)
{
	switch (val)
	{
	case 0x0A00:
		return SIS_ATTR_FILL;
	case 0x0A01:
		return SIS_ATTR_HIDDEN;
	case 0x0A02:
		return SIS_ATTR_MEET;
	case 0x0A03:
		return SIS_ATTR_SCROLL;
	case 0x0A04:
		return SIS_ATTR_SLICE;
	default:
		return SIS_ATTR_NOT_USED;
	}
} 






static int initCallback (we_prsr_parserData_t*  p, we_prsr_decoderData_t* d,
	const char* doctype_str, int len, int  wbxml_code, int charset)
{
    p->startHandler     = startElementCallback;
    p->endHandler       = endElementCallback;
    p->charHandler      = charDataCallback;
    p->attributes       = &smil_Attributes;
    p->attributeType    = smil_AttributeTypes;
    p->elements         = &smil_Elements;
    p->elementTable     = smil_Element_table;
    p->attrvals         = &smil_Attrvals;

    (void)d;
    (void)doctype_str;
    (void)wbxml_code;
    (void)charset;
    (void)len;
    return WE_PRSR_APPLICATION_OK;
} 





static int piCallback(void* userData, we_prsr_parser_t* parser, 
	const char* target,  const char* data, 
	 const char* end)
{
    (void)userData;
    (void)parser;
    (void)target;
    (void)data;
    (void)end;
    return WE_PRSR_APPLICATION_OK;
} 








static void saveRegions(SisRegion *region, const we_prsr_attribute_t* attrs, 
                        int numAttrs)
{
      
    char        *e        = NULL;
	WE_UINT32   tmpFit   = 0;
        

    
    region->name = getStrAttr(attrs, numAttrs, SMIL_ATTRIBUTE_ID);

    
    
    if (!getIntAttr(attrs, numAttrs, SMIL_ATTRIBUTE_LEFT,
        &region->position.cx.value, 0,  &e))
    {      
        region->position.cx.value = 0;
    }
    else
    {
        if (e && *e == '%')
        {
            region->position.cx.type = SIS_REL;
        }
        else
        {
            region->position.cx.type = SIS_ABS;
        }
    }
    e = NULL;
    if (!getIntAttr(attrs, numAttrs, SMIL_ATTRIBUTE_TOP,
        &region->position.cy.value, 0, &e))
    {
        region->position.cy.value = 0;
    }            
    else
    {
        if (e && *e == '%')
        {
            region->position.cy.type = SIS_REL;
        }
        else
        {
            region->position.cy.type = SIS_ABS;
        }
    }
    
    e = NULL;
    if (!getIntAttr(attrs, numAttrs, SMIL_ATTRIBUTE_WIDTH,
        &region->size.cx.value, 0, &e))
	{

		region->size.cx.value =100;
		region->size.cx.type = SIS_REL;
    }
    else
    {
        if (e && *e == '%')
        {
            region->size.cx.type = SIS_REL;
        }
        else
        {
            region->size.cx.type = SIS_ABS;
        }
    }
    e = NULL;
    if (!getIntAttr(attrs, numAttrs, SMIL_ATTRIBUTE_HEIGHT,
        &region->size.cy.value, 0, &e))
	{
		

		region->size.cy.value =100;
		region->size.cy.type = SIS_REL;
    }            
    else
    {
        if (e && *e == '%')
        {
            region->size.cy.type = SIS_REL;
        }
        else
        {
            region->size.cy.type = SIS_ABS;
        }
    }
		
	if (!getIntAttr(attrs, numAttrs, SMIL_ATTRIBUTE_FIT, 0,
        &tmpFit, &e))
    {
        region->fit = SIS_ATTR_NOT_USED;
    }
	else
	{
		region->fit = getFitAttr(tmpFit);
	}
       

} 









static WE_UINT32 getMs(WE_UINT32 value, const char *e)
{
    if (e)
    {
        switch (*e)
        {
            
        case 'h':
            return (value * 3600);
        case 'm':
            
            if (*(e + 1) == 's')
            {
                return value;
            }
            
            else
            {
                return (value * 60);
            }
            
        case 's':
        default:
            break;
        }
    }
    return (value * 1000);
}








static void storeSlideElementInfo(SisSlideElement *element, int type, 
    const we_prsr_attribute_t* attrs, int numAttrs)
{    
    WE_UINT16  tmpShort;
    WE_UINT32  tmpInt;
    char       *extra  = NULL;
    char       *src    = NULL;
    char       *region = NULL;

    switch(type) 
    {
    case SMIL_ELEMENT_IMG:
        element->objectType = SIS_OBJECT_TYPE_IMAGE;
    	break;
    case SMIL_ELEMENT_VIDEO:
        element->objectType = SIS_OBJECT_TYPE_VIDEO;
        break;
    case SMIL_ELEMENT_TEXT:
        element->objectType = SIS_OBJECT_TYPE_TEXT;
        break;
    case SMIL_ELEMENT_AUDIO:
        element->objectType = SIS_OBJECT_TYPE_AUDIO;
        break;
    case SMIL_ELEMENT_REF:
        element->objectType = SIS_OBJECT_TYPE_REF;
        break;
    default:
        element->objectType  = SIS_OBJECT_TYPE_UNKNOWN;
        break;
    }   
    
    
    if (type != SMIL_ELEMENT_AUDIO) 
    {
        region = getStrAttr(attrs, numAttrs, SMIL_ATTRIBUTE_REGION);

        element->regionId = getRegionIdFromRegion(region);

        if (region) 
        {
            WE_MEM_FREE(WE_MODID_SIS, region);
        }
    }

    src = getStrAttr(attrs, numAttrs, SMIL_ATTRIBUTE_SRC);
    
    element->objectId = getObjectIdFromSrc(slsParser.bodyInfoList, src);

    if (src) 
    {
        WE_MEM_FREE(WE_MODID_SIS, src);
    }
     
    element->alt = getStrAttr(attrs, numAttrs, SMIL_ATTRIBUTE_ALT);
      
    if (getIntAttr(attrs, numAttrs, SMIL_ATTRIBUTE_BEGIN, 
        &tmpShort, &tmpInt, &extra))
    {
        element->beginT = 
            getMs((WE_UINT32)(tmpShort & 0xFFFF), extra);
    }
    else
    {
        element->beginT = 0;
    }
    
    if (getIntAttr(attrs, numAttrs, SMIL_ATTRIBUTE_END, 
        &tmpShort, &tmpInt, &extra))
    {
        element->endT = 
            getMs((WE_UINT32)(tmpShort & 0xFFFF), extra);
    }
    else
    {
        element->endT = 0;
    }

    if (getIntAttr(attrs, numAttrs, SMIL_ATTRIBUTE_DUR, 
        &tmpShort, &tmpInt, &extra))
    {
        element->duration = 
            getMs((WE_UINT32)(tmpShort & 0xFFFF), extra);
    }
    else
    {
        element->duration = 0;
    }

}










static int startElementCallback(void* userData, int type, 
	we_prsr_attribute_t* attrs, int numAttrs)
{
        
    static WE_BOOL     noOfSlidesExeedsMax = FALSE;

    SisRootLayout       tmpLayout;

    SisRegion           tmpRegion;
    WE_UINT32           regionId;
       
    char *e = NULL;
	
	char *tmpStr;
	char *paramName;
	char *paramValue;
   
   


    (void)userData;

    switch (*slsParser.tagStackPtr)
    {
    case SMIL_ELEMENT_SMIL:
        noOfSlidesExeedsMax = FALSE;
        break;
    
    case SMIL_ELEMENT_BODY:

        
        memset(&slsParser.tmpSlideInfo, 0, sizeof(SisSlideInfo));
        memset(&slsParser.tmpSlideElement, 0, sizeof(SisSlideElement));
        slsParser.tmpSlideElement.bgColor = SIS_COLOR_NOT_USED;
        slsParser.tmpSlideElement.fgColor = SIS_COLOR_NOT_USED;

        if (SMIL_ELEMENT_PAR == type)
        {
            
            if (MAX_NO_OF_SLIDES <= slsParser.slides)
            {
                
				noOfSlidesExeedsMax = TRUE;				          
            }
            else
            {                                        
                
                e = NULL;
                if (!getIntAttr(attrs, numAttrs, SMIL_ATTRIBUTE_DUR,
                    0, &slsParser.tmpSlideInfo.duration, &e))
                {
                    
                    slsParser.tmpSlideInfo.duration = DEFAULT_DURATION;
                }
                
                slsParser.tmpSlideInfo.duration = 
                    getMs(slsParser.tmpSlideInfo.duration, e);
            }
        }
        break;
    case SMIL_ELEMENT_PAR:
		if (!noOfSlidesExeedsMax)
		{				
            storeSlideElementInfo(&slsParser.tmpSlideElement, type, attrs, numAttrs);                                      
        }
		
        break;

    case SMIL_ELEMENT_AUDIO:
    case SMIL_ELEMENT_IMG:
    case SMIL_ELEMENT_VIDEO:
    case SMIL_ELEMENT_REF:

        if (type == SMIL_ELEMENT_PARAM) 
        {
            paramName   = getStrAttr(attrs, numAttrs, SMIL_ATTRIBUTE_NAME);
			paramValue = getStrAttr(attrs, numAttrs, SMIL_ATTRIBUTE_VALUE);

            addSlsParamListElement(&slsParser.tmpParamList, 
                paramName, paramValue);

        }

        break;

	case SMIL_ELEMENT_TEXT:
		{
			
			if (SMIL_ELEMENT_PARAM == type)
			{
				
				paramName   = getStrAttr(attrs, numAttrs, SMIL_ATTRIBUTE_NAME);
				paramValue  = getStrAttr(attrs, numAttrs, 
						SMIL_ATTRIBUTE_VALUE);
                
                slsParser.tmpSlideElement.bgColor = SIS_COLOR_NOT_USED;
                slsParser.tmpSlideElement.fgColor = SIS_COLOR_NOT_USED;

				if ((we_cmmn_strcmp_nc(paramName, SMIL_ATTR_PARAM_FG_COLOR_1) == 0) ||
                    (we_cmmn_strcmp_nc(paramName, SMIL_ATTR_PARAM_FG_COLOR_2) == 0) ||
					(we_cmmn_strcmp_nc(paramName, SMIL_ATTR_PARAM_FG_COLOR_3) == 0))

				{
					slsParser.tmpSlideElement.fgColor = colorSTOI((const char *)paramValue);
				}
					addSlsParamListElement(&slsParser.tmpParamList, 
                paramName, paramValue);
			       
            }
		}
		break;
    case SMIL_ELEMENT_LAYOUT:
		if (!noOfSlidesExeedsMax)
		{
            if (SMIL_ELEMENT_ROOT_LAYOUT == type)
            {
                
                memset(&tmpLayout, 0, sizeof(SisRootLayout));

                e = NULL;
                if (!getIntAttr(attrs, numAttrs, SMIL_ATTRIBUTE_WIDTH,
                    &tmpLayout.cx.value, 0, &e))
                {
                    tmpLayout.cx.value = 0;
                }
                else
                {				
                    if (e && *e == '%')
                    {
                        tmpLayout.cx.type = SIS_REL;
                    }
                    else
                    {
                        tmpLayout.cx.type = SIS_ABS;
                    }
                }
                e = NULL;
                if (!getIntAttr(attrs, numAttrs, SMIL_ATTRIBUTE_HEIGHT,
                    &tmpLayout.cy.value, 0, &e))
                {
                    tmpLayout.cy.value = 0;
                }
                else
                {					
                    if (e && *e == '%')
                    {
                        tmpLayout.cy.type = SIS_REL;
                    }
                    else
                    {
                        tmpLayout.cy.type = SIS_ABS;
                    }
                }              
                tmpStr = getStrAttr(attrs, numAttrs, SMIL_ATTRIBUTE_BG_COLOR);
                tmpLayout.bgColor = colorSTOI((const char *)tmpStr);
                SIS_FREE(tmpStr);

                
                slsParser.rootLayout->cx.value = tmpLayout.cx.value;
                slsParser.rootLayout->cx.type = tmpLayout.cx.type;

                slsParser.rootLayout->cy.value = tmpLayout.cy.value;
                slsParser.rootLayout->cy.type = tmpLayout.cy.type;

                slsParser.rootLayout->bgColor = tmpLayout.bgColor;
                


            }
            if (SMIL_ELEMENT_REGION == type)
            {
                saveRegions(&tmpRegion, attrs, numAttrs);
                
                
                regionId = getRegionIdFromRegion(tmpRegion.name);

                addSlsRegionListElement(slsParser.regionList, regionId, &tmpRegion);
            }
			
		}		        
        break;
    default:
        break;
    }

    pushElement(type);

    if (numAttrs)
    {
        we_prsr_free_attributes(WE_MODID_SIS, attrs, numAttrs);
    }
    return WE_PRSR_APPLICATION_OK;
} 







static int endElementCallback(void* userData)
{
    (void)userData;
    
    if (SMIL_ELEMENT_PAR == *slsParser.tagStackPtr)
    {
        
        

        if(addSlsSlideInfoListElement(slsParser.slideInfoList,
                                   slsParser.tmpSlideInfo.duration, 
                                   slsParser.tmpSlideInfo.elementList)==TRUE)
        {
            slsParser.slides++;
        }

        
        slsParser.tmpSlideInfo.elementList = NULL;
    } 
	else if(SMIL_ELEMENT_BODY == *slsParser.tagStackPtr)
	{
		
		if(slsParser.slides == 0 && 
			addSlsSlideInfoListElement(slsParser.slideInfoList,
									slsParser.tmpSlideInfo.duration,
									slsParser.tmpSlideInfo.elementList) == TRUE)
		{
			slsParser.slides ++;
		}
	}
    else if (*slsParser.tagStackPtr == SMIL_ELEMENT_TEXT    ||
             *slsParser.tagStackPtr == SMIL_ELEMENT_AUDIO   ||
             *slsParser.tagStackPtr == SMIL_ELEMENT_IMG     ||
             *slsParser.tagStackPtr == SMIL_ELEMENT_VIDEO   ||
             *slsParser.tagStackPtr == SMIL_ELEMENT_REF)
    {
               
        addSlsSlideElementListElement(&slsParser.tmpSlideInfo.elementList,
            slsParser.tmpSlideElement.objectType,
            slsParser.tmpSlideElement.regionId, 
            slsParser.tmpSlideElement.objectId, 
            slsParser.tmpSlideElement.fgColor, 
            slsParser.tmpSlideElement.bgColor, 
            slsParser.tmpSlideElement.beginT, 
            slsParser.tmpSlideElement.endT,  
            slsParser.tmpSlideElement.duration, 
            slsParser.tmpSlideElement.alt, 
            slsParser.tmpParamList);

        
        slsParser.tmpParamList = NULL;

    }
	if (slsParser.slides > MAX_NO_OF_SLIDES)
	{
		
		return WE_PRSR_APPLICATION_ERROR;
	}
	else
	{
		(void)popElement();
		return WE_PRSR_APPLICATION_OK;
	} 
} 






static int charDataCallback(void* userData, const char* s, int len, int mode)
{
    (void)userData;
    (void)s;
    (void)len;
    (void)mode;
    return WE_PRSR_APPLICATION_OK;
} 






SlsParseResult parseSMIL( char *smilData, 
     MmsBodyInfoList *bodyInfoList,
    SisSlideInfoList **slideInfoList, 
    SisRegionList **regionList,
    SisRootLayout *rootLayout,
    WE_UINT8     *slides)
{

    we_prsr_parser_t  *msfParser;   
    int                 result;
    int                 handleType;
    int                 handle;
    void               *userData;
	SlsParseResult		ret;

    memset(&slsParser, 0, sizeof(SlsPrseInfo));
    
    
    slsParser.bodyInfoList   = bodyInfoList;
    
    
    slsParser.slideInfoList = slideInfoList;    
    slsParser.regionList    = regionList;
    slsParser.rootLayout    = rootLayout;
    
	
    msfParser = 
        we_prsr_create (WE_MODID_SIS,
	    NULL,
        initCallback,
        piCallback,
        WE_PRSR_BUFFER,
        (char *)smilData,
        (int)strlen((const char*)smilData) + 1,
        0,
        SIA_PARSE_BUFFER_INITIAL_SIZE, 
        WE_CHARSET_UTF_8,
        WE_CHARSET_ASCII,
        TRUE);

	
	smilData = NULL;
    
    
    elementStackInit();
    
	
    do
    {
        result = we_prsr_parse(msfParser);
    } while (WE_PRSR_STATUS_MORE_TO_DO == result);
    
	
    
    
    elementStackInit();
    
	
    (void)we_prsr_delete(msfParser, &handle, &handleType, &userData);
     
	if (result == WE_PRSR_STATUS_PARSING_COMPLETED)
	{
		
		ret = SIS_PARSE_OK;

        
        *slides = slsParser.slides;
	}
	else if (result == WE_PRSR_ERROR_APPLICATION)
	{
		
		ret = SIS_PARSE_ERROR_TO_MANY_SLIDES;
	} 
	else
	{
		
		ret = SIS_PARSE_ERROR_UNKNOWN;
	}
	
    freeSlsRegionAdmList(slsParser.regionAdmList);
   
	return ret;

    
}








WE_UINT32 colorSTOI(const char *colorStrVal)
{
	WE_UINT32 color = 0;
	int index = 0;
	char colorVal[7];
    unsigned char tmp1;
	unsigned char tmp2;
	unsigned char tmp3;
    
	if (colorStrVal == NULL)
	{
		return SIS_COLOR_NOT_USED;
	} 

    
	if (colorStrVal[0] == SIS_HASH_MARK)
	{
		memset(colorVal, 0x00, 7);
        
		if (strlen(colorStrVal) == 4)
        {
            
            colorVal[0] = colorVal[1] = colorStrVal[1];
            colorVal[2] = colorVal[3] = colorStrVal[2];
            colorVal[4] = colorVal[5] = colorStrVal[3];
        }
        else
        {
            strcpy(colorVal, (const char *)&colorStrVal[1]);
        }

        if (!we_cmmn_hex2byte(&colorVal[0], &tmp1) || 
			!we_cmmn_hex2byte(&colorVal[2], &tmp2) ||
			!we_cmmn_hex2byte(&colorVal[4], &tmp3))
		{
			return SIS_COLOR_NOT_USED;
		}
		else
		{
			color+= tmp1 << 16;
			color+= tmp2 << 8;
			color+= tmp3;
		} 

	}
	else
	{
		

		while (slsColorSet[index].colorName != NULL)
		{
			if (!we_cmmn_strcmp_nc(slsColorSet[index].colorName, colorStrVal))
			{
				return slsColorSet[index].colorValue;
			} 
			index++;
		} 

		
		color = SIS_COLOR_NOT_USED;
	} 
	return color;
} 

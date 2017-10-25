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




#ifndef SIS_MAIN_H
#define SIS_MAIN_H

#ifndef SIS_IF_H
#error Sis_if.h must be included berfore Sis_main.h!
#endif

#ifndef MMS_DEF_H
#error Mms_Def.h must be included berfore Sis_main.h!
#endif





typedef struct   
{
    const SisMediaType       mediaType;         
    const char               *mimeType;         
    const MmsKnownMediaType  mmsMediaType;      
    const SlsMediaObjectType objectType;        
}SlsMediaTypeItem;





typedef enum 
{
    
    SMIL_MEDIA_OBJECT_AUDIO = 0,
    
    SMIL_MEDIA_OBJECT_IMG,
    



    SMIL_MEDIA_OBJECT_REF,
    
    SMIL_MEDIA_OBJECT_TEXT,
	
    SMIL_MEDIA_OBJECT_VIDEO
} SmilMediaObjectElement;







typedef struct SmilMediaObjectSt
{
    
    SmilMediaObjectElement mediaObjectElement;

    



    char *src;

    
    char *alt;

    
    char *region;

    
    WE_UINT32 begin; 

    
    WE_UINT32 end;

    
    WE_UINT32 duration;

    
    WE_UINT32 color;

    
    struct SmilMediaObjectSt *next;
}SmilMediaObject;








typedef struct
{
    
    SmilMediaObject *mobj;

    
    WE_UINT32 dur;
    WE_UINT8  used;
} SmilPar;





typedef struct Regions
{
    char    *id;         
    SisSize  position;   
    SisSize  size;       
	SisFit	 fit;		 
    struct Regions *next; 
} Regions;

















void addSlsObjectInfoListElement(SisObjectInfoList **listStart,
    WE_UINT32          id,             
    MmsContentType     *contentType,
    WE_UINT32          size);









void addSlsParamListElement(SlsParamList **listStart, char *name, char *value);









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
    SlsParamList *paramList);









WE_BOOL addSlsSlideInfoListElement(SisSlideInfoList   **listStart,
    WE_UINT32           duration,
    SisSlideElementList *elementList);









void addSlsRegionListElement(SisRegionList **listStart,
    WE_UINT32     id, 
    const SisRegion     *region);








void slsInit(void);







void slsSignalDispatcher(WE_UINT8 module, WE_UINT16 signal, void *p);






const SlsMediaTypeItem *slsGetSupportedMoList(void);

#endif 

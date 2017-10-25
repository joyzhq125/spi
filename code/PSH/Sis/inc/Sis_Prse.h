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




#ifndef SIS_PRSE_H
#define SIS_PRSE_H




#define SMIL_ELEMENT_AUDIO         0x00
#define SMIL_ELEMENT_BODY          0x01
#define SMIL_ELEMENT_HEAD          0x02
#define SMIL_ELEMENT_IMG           0x03
#define SMIL_ELEMENT_LAYOUT        0x04
#define SMIL_ELEMENT_META          0x05
#define SMIL_ELEMENT_PAR           0x06
#define SMIL_ELEMENT_REF           0x07
#define SMIL_ELEMENT_REGION        0x08
#define SMIL_ELEMENT_ROOT_LAYOUT   0x09
#define SMIL_ELEMENT_SMIL          0x0A
#define SMIL_ELEMENT_TEXT          0x0B
#define SMIL_ELEMENT_VIDEO         0x0C
#define SMIL_ELEMENT_PARAM         0x0D
#define SMIL_ELEMENT_NOT_VALID     0xFF



#define SMIL_ATTRIBUTE_BEGIN        0x00
#define SMIL_ATTRIBUTE_ALT          0x01
#define SMIL_ATTRIBUTE_CONTENT      0x02
#define SMIL_ATTRIBUTE_DUR          0x03
#define SMIL_ATTRIBUTE_END          0x04
#define SMIL_ATTRIBUTE_FIT          0x05
#define SMIL_ATTRIBUTE_HEIGHT       0x06
#define SMIL_ATTRIBUTE_ID           0x07
#define SMIL_ATTRIBUTE_LEFT         0x08
#define SMIL_ATTRIBUTE_NAME         0x09
#define SMIL_ATTRIBUTE_REGION       0x0A
#define SMIL_ATTRIBUTE_SRC          0x0B
#define SMIL_ATTRIBUTE_TOP          0x0C
#define SMIL_ATTRIBUTE_WIDTH        0x0D
#define SMIL_ATTRIBUTE_BG_COLOR		0x0E
#define SMIL_ATTRIBUTE_VALUE		0x0F


typedef enum
{
	SIS_PARSE_OK,
	SIS_PARSE_ERROR_TO_MANY_SLIDES,
	SIS_PARSE_ERROR_UNKNOWN
} SlsParseResult;





typedef struct SlsRegionAdmSt  
{
    WE_UINT32  regionId;
    char       *region;
    struct SlsRegionAdmSt *next;

}SlsRegionAdmList;



















SlsParseResult parseSMIL( char *smilData, 
     MmsBodyInfoList *bodyInfoList,
    SisSlideInfoList **slideInfoList, 
    SisRegionList **regionList,
    SisRootLayout * rootLayout,
    WE_UINT8     *slides);








WE_UINT32 colorSTOI(const char *colorStrVal);

#endif 

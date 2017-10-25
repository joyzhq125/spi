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

/* !\file maslh.h
 * Logic for SMIL slide handling.
 */

#ifndef _MASLH_H_
#define _MASLH_H_

/******************************************************************************
 * Constants
 *****************************************************************************/
#define MSA_OBJ_DURATION_MIN      1000    /*!< One second for the min object 
                                                 duration time in milliseconds */

/******************************************************************************
 * Prototypes
 *****************************************************************************/

WE_BOOL msaCreateSmilInstance(MsaSmilInfo **smilInfo);
void msaCleanupSmilInstance(MsaSmilInfo **smilInfo);
void msaDeleteAllSlides(MsaSmilInfo *smilInfo);
WE_BOOL msaDeleteSlide(MsaSmilInfo *smilInfo);
WE_BOOL msaDeleteSlideMo(MsaSmilInfo *smilInfo, MsaMediaGroupType objType);
MsaSmilSlide *msaGetCurrentSlide(const MsaSmilInfo *smilInfo);
unsigned int msaGetDuration(const MsaSmilInfo *smilInfo);
MsaSmilSlide *msaGetFirstSlide(const MsaSmilInfo *smilInfo);
MsaSmilSlide *msaGetNextSlide(MsaSmilInfo *smilInfo);
MsaSmilSlide *msaGetPreviousSlide(MsaSmilInfo *smilInfo);
unsigned int msaGetSlideCount(const MsaSmilInfo *smilInfo);
unsigned int msaGetSlideIndex(const MsaSmilInfo *smilInfo, 
    const MsaSmilSlide *slide);
MsaSmilSlide *msaInsertSlide(MsaSmilInfo *smilInfo);
void msaSetDuration(const MsaSmilInfo *smilInfo, unsigned int);
MsaMoDataItem *msaSetSlideMo(MsaSmilInfo *smilInfo, MmsContentType *mmsType, 
    unsigned char *buffer, unsigned int size, char *name);
MsaObjInfo *msaSetSlideMoByRef(MsaSmilInfo *smilInfo, MsaMoDataItem *item);
MsaMoDataItem *msaGetMo(const MsaSmilInfo *smilInfo, MsaMediaGroupType objType);
MsaMoDataItem *msaFindObjectByIndex(const MsaSmilInfo *smilInfo, WE_UINT32 index);
WE_BOOL msaGetObjInfo(const MsaSmilInfo *sInfo, MsaMediaGroupType type, 
    MsaObjInfo **objInfo);
WE_UINT32 msaCalculateMsgSize(const MsaSmilInfo *smilInfo, 
    const MsaPropItem *propItem);
void msaOptimizeMoList(MsaSmilInfo *smilInfo);
WE_BOOL msaCreateSmil(MsaSmilInfo *smilInfo, char **buffer, unsigned int *size);
WE_BOOL msaIsSmilValid(const MsaSmilInfo *sInfo);
MsaMediaGroupType msaGetSlideObjectDef(const MsaSmilInfo *sInfo);

WE_BOOL msaSetObjStart(const MsaSmilInfo *sInfo, MsaMediaGroupType gType, 
    WE_UINT32 start);
WE_BOOL msaGetObjStart(const MsaSmilInfo *sInfo, MsaMediaGroupType gType, 
    WE_UINT32 *start);
WE_BOOL msaSetObjDuration(const MsaSmilInfo *sInfo, MsaMediaGroupType gType, 
    WE_UINT32 duration);
WE_BOOL msaGetObjDuration(const MsaSmilInfo *sInfo, MsaMediaGroupType gType, 
    WE_UINT32 *duration);
WE_BOOL msaSetObjColor(const MsaSmilInfo *sInfo, MsaMediaGroupType gType, 
    WE_UINT32 fgColor, WE_UINT32 bgColor);
WE_BOOL msaGetObjColor(const MsaSmilInfo *sInfo, MsaMediaGroupType gType, 
    WE_UINT32 *fgColor, WE_UINT32 *bgColor);


#endif /* _MASLH_H_ */

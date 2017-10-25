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

#ifndef SASIS_H
#define SASIS_H

#ifndef SAINTSIG_H
#error Saintsig.h must be included berfore Sasls.h!
#endif

/*--- Definitions/Declarations ---*/

/*--- Types ---*/
typedef struct 
{
    WE_UINT8           noOfSlides;
    SisRootLayout       rootLayout;
    SisRegionList       *regionList;
    SisSlideInfoList    *slideInfoList;
    SiaObjectInfoList   *objectInfoList;
}SiaSmilInfo;
/*********************************************
 * Exported function
 *********************************************/
void smaSlsTerminate(void);
void smaSisInit(void);
void smaLoadSlideInfo(SiaStateMachine retFsm, int retSig, WE_UINT32 msgId);
void smaHandleOpenSmilRsp(const SisOpenSmilRsp *openSmilRsp);
void smaHandleGetSlideInfoRsp(const SisGetSlideInfoRsp *getSlideInfoRsp);


WE_INT16 smaCvtSlsSizeValue(const SisSizeValue *sizeVal, WE_INT16 screenSize);
void smaCvtSlsSizeToSize(const SisSize *src, WeSize *dst, const WeSize *dispSize);
void smaCvtSlsSizeToPos(const SisSize *src, WePosition *dst, const WeSize *dispSize);

SiaObjectInfo *smaGetObjectInfo(WE_UINT32 bpId, SiaObjectInfoList *objList);
const SisRegion *smaGetRegion(WE_UINT32 regionId, const SisRegionList *regionList);
SisSlideInfo *smaGetSlideInfo(WE_UINT32 slideNo, 
    SisSlideInfoList *slideInfoList);

void smaCopySlsObjectInfo(const SisObjectInfo *source, SiaObjectInfo *dest);
void smaCopySlsObjectInfoList(const SisObjectInfoList *source, 
    SiaObjectInfoList *dest);
void smaFreeSiaObjectInfo(SiaObjectInfo *objInfo);
void smaFreeSiaObjectInfoList(SiaObjectInfoList *objList);

#endif /* SAMAIN_H */

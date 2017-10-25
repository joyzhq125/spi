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









#ifndef SIS_CVT_H
#define SIS_CVT_H




















WE_BOOL Sis_cvt_SlsOpenSmilReq(we_dcvt_t *obj, SisOpenSmilReq *data);










WE_BOOL Sis_cvt_SlsCloseSmilRsp(we_dcvt_t *obj, SisCloseSmilRsp *data);










WE_BOOL Sis_cvt_SlsGetSlideInfoReq(we_dcvt_t *obj, SisGetSlideInfoReq *data);










WE_BOOL Sis_cvt_SlsObjectInfo(we_dcvt_t *obj, SisObjectInfo *data);










WE_BOOL Sis_cvt_SlsObjectInfoListSt(we_dcvt_t *obj, SisObjectInfoList *data);










WE_BOOL Sis_cvt_SlsObjectInfoList(we_dcvt_t *obj, SisObjectInfoList **data);










WE_BOOL Sis_cvt_SlsSize(we_dcvt_t *obj, SisSize *data);










WE_BOOL Sis_cvt_SlsRegion(we_dcvt_t *obj, SisRegion *data);










WE_BOOL Sis_cvt_SlsRegionListSt(we_dcvt_t *obj, SisRegionList *data);










WE_BOOL Sis_cvt_SlsRegionList(we_dcvt_t *obj, SisRegionList **data);










WE_BOOL Sis_cvt_SlsSizeValue(we_dcvt_t *obj, SisSizeValue *data);











WE_BOOL Sis_cvt_SlsRootLayout(we_dcvt_t *obj, SisRootLayout *data);










WE_BOOL Sis_cvt_SlsOpenSmilRsp(we_dcvt_t *obj, SisOpenSmilRsp *data);










WE_BOOL Sis_cvt_SlsParam(we_dcvt_t *obj, SlsParam *data);









WE_BOOL Sis_cvt_SlsParamListSt(we_dcvt_t *obj, SlsParamList *data);









WE_BOOL Sis_cvt_SlsParamList(we_dcvt_t *obj, SlsParamList **data);










WE_BOOL Sis_cvt_SlsSlideElement(we_dcvt_t *obj, SisSlideElement *data);










WE_BOOL Sis_cvt_SlsSlideElementSt(we_dcvt_t *obj, SisSlideElementList *data);










WE_BOOL Sis_cvt_SlsSlideElementList(we_dcvt_t *obj, SisSlideElementList **data);










WE_BOOL Sis_cvt_SlsSlideInfo(we_dcvt_t *obj, SisSlideInfo *data);










WE_BOOL Sis_cvt_SlsSlideInfoListSt(we_dcvt_t *obj, SisSlideInfoList *data);










WE_BOOL Sis_cvt_SlsSlideInfoList(we_dcvt_t *obj, SisSlideInfoList **data);










WE_BOOL Sis_cvt_SlsGetSlideInfoRsp(we_dcvt_t *obj, SisGetSlideInfoRsp *data);

#endif 

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

#ifndef SIS_OPEN_H
#define SIS_OPEN_H

#ifndef SIS_IF_H
#error Sis_if.h must be included berfore Sis_main.h!
#endif






typedef enum
{
    





    SIS_SIG_OPEN_MMS_GET_HEADER_RSP,

    





    SIS_SIG_OPEN_MMS_GET_SKELETON_RSP,

    





    SIS_SIG_OPEN_MMS_GET_BP_RSP,
    
    





    SIS_SIG_OPEN_LOAD_BP_TO_BUFFER_RSP

}SlsOpenSignalId;

























void sendCloseSmilRsp(WE_UINT8 destModId, SisResult result, WE_UINT32 userData);









void sendGetSlideInfoRsp(WE_UINT8           client,
                        SisResult           result, 
                        SisSlideInfoList   *slideInfo, 
                        WE_UINT32          userData);













void sendOpenSmilRsp(WE_UINT8           client,
                     SisResult           result,   
                     WE_UINT8           noOfSlides,
                     const SisRootLayout *rootLayout,
                     SisUnrefItemList    *unrefItems,
                     SisRegionList       *regionList,
                     SisObjectInfoList   *objectList,
                     WE_UINT32          userData);







WE_BOOL slsCloseSmil(WE_UINT8 module, WE_UINT32 userData);











WE_BOOL slsGetSlideInfo(WE_UINT8  module,
                     WE_UINT32 startSlide,
                     WE_UINT32 numberOfSlides, 
                     WE_UINT32 userData);









void slsOpenSmil(WE_UINT8 destModId, WE_UINT32 msgId, WE_UINT32 userData);





void slsOpenInit(void);





void slsOpenTerminate(void);

#endif 

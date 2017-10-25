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

#ifndef SAMAIN_H
#define SAMAIN_H


#ifndef SIS_IF_H
#error Sis_if.h must be included berfore Samain.h!
#endif

#ifndef SATYPES_H
#error Satypes.h must be included berfore Samain.h!
#endif

/*--- Definitions/Declarations ---*/
#define SIA_FIRST_SLIDE 0


/*--- Types ---*/

typedef enum
{
    /*! The slide timer has expired
     *  - Param I : -  
     *  - Param U1: objectId of selected objec
     *  - Param U2: -
     *  - Param P : the selected link #SmtrLinkEntry or NULL
     */
    SIA_SIG_MH_SHOW_SLIDE_MENU,
    
     /*! The list of possible actions are returned through this signal
     *  - Param I : -  
     *  - Param U1: result
     *  - Param U2: count
     *  - Param P : the list of actions
     */
    SIA_SIG_MH_HANDLE_ACTION_RSP,
    /*! Handle object action request
     *  - Param I : -  
     *  - Param U1: object id
     *  - Param U2: -
     *  - Param P : -
     */
    SIA_SIG_MH_OBJ_ACTION,
    /*! Object action operation is complete 
     *  - Param I : -  
     *  - Param U1: -
     *  - Param U2: -
     *  - Param P : -
     */
    SIA_SIG_MH_OBJ_ACTION_DONE,
    /*! Set play timer
     *  - Param I : -  
     *  - Param U1: -
     *  - Param U2: -
     *  - Param P : -
     */
    SIA_SIG_MH_SET_TIMER,
    /*! Set play timer
     *  - Param I : -  
     *  - Param U1: -
     *  - Param U2: -
     *  - Param P : -
     */
    SIA_SIG_MH_NO_TIMER,

    /*! Load slide info is complete 
     *  - Param I : result of the operation  
     *  - Param U1: -
     *  - Param U2: -
     *  - Param P : A pointer to #SiaSmilInfo.
     */
    SIA_SIG_MH_LOAD_SLIDE_INFO_RSP,

    /*! Load slide info is complete 
     *  - Param I : -
     *  - Param U1: -
     *  - Param U2: -
     *  - Param P : -
     */
    SIA_SIG_MH_LOAD_SLIDE_RSP,

    /*! The slide timer has expired
     *  - Param I : -  
     *  - Param U1: -
     *  - Param U2: -
     *  - Param P : -
     */
    SIA_SIG_MH_SLIDE_EXPIRE,

     /*! Handling of the boundary part
     *  - Param I : - 
     *  - Param U1: - 
     *  - Param U2: -
     */
    SIA_SIG_GADGET_GOT_FOCUS,

    /*! Handling of the boundary part
     *  - Param I : - 
     *  - Param U1: - 
     *  - Param U2: -
     */
    SIA_SIG_GADGET_LOST_FOCUS,

    /*! Handling of the boundary part
     *  - Param I : - 
     *  - Param U1: - 
     *  - Param U2: -
     */
    SIA_SIG_EVENT_KEY_DOWN,

    /*! Handling of the boundary part
     *  - Param I : - 
     *  - Param U1: - 
     *  - Param U2: -
     */
    SIA_SIG_EVENT_KEY_UP,


    /*! Handling of the boundary part
     *  - Param I : - 
     *  - Param U1: Event type #WeEventType 
     *  - Param U2: -
     */
    SIA_SIG_MH_STEP_EVENT,
    

       /*! Exit 
     *  - Param I : - 
     *  - Param U1: 
     *  - Param U2: -
     */
    SIA_SIG_MH_EXIT
    
} SiaStates;

/*********************************************
 * Exported function
 *********************************************/

WE_BOOL smaMainInit(void);
void smaMainTerminate(void);
char *convertToUTF8(WE_UINT32 charset, const char *srcData, 
    WE_UINT32 *srcSize, WE_UINT32 *dstSize);
void smaPlaySmil(void);
void smaStopSmil(void);
void smaRewindSmil(void);
void smaDisplayPreviousSlide(void);
void smaDisplayNextSlide(void);
void smaSendPlaySmilRsp(SiaResult result);
WE_BOOL checkObjectType(SlsMediaObjectType objType, SisSlideElement *slide);
WE_BOOL isKnownType(SisMediaType mediaType, SlsMediaObjectType objType);
WE_BOOL handleSiaErr( SiaResult result);
SiaResult getSiaExitResult(void);
void smaMakeCall(const char *number);
void smaMakeCallRsp(const we_tel_make_call_resp_t *callRsp);

SlsMediaObjectType smaGetSlsMediaObjectType(const char *mimeType);
WE_BOOL smaIsSupportedMediaObject(const char *mimeType);

const SisRegionList *smaGetRegionList(void);
const SisRootLayout smaGetRootLayout(void);
SiaObjectInfoList *smaGetObjectInfoList(void);
WE_BOOL smaIsPlaying(void);
void smaHandleUtf8Bom(char *str);
void smaHandlePlaySmilReq(WE_UINT8 module, WE_UINT32 msgId, 
    WE_UINT32 userData);
WE_UINT32 smaGetMsgId(void);
char *smaCreateFileName(const SiaObjectInfo *objInfo);
char *smaMakeDrmContentType(const MmsContentType *contentType);

#endif /* SAMAIN_H */

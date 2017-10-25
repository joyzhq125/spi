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
 
#ifndef SAUIFORM_H
#define SAUIFORM_H

#ifndef SATYPES_H
#error Satypes.h must be included berfore Sauiform.h!
#endif
#ifndef SIA_DEF_H
#error Sia_def.h must be included berfore Sauiform.h!
#endif
#ifndef SAINTSIG_H
#error Saintsig.h must be included berfore Sauiform.h!
#endif


/*--- Definitions/Declarations ---*/

/*--- Types ---*/
typedef int SiaSlideFormHandle;
/*********************************************
 * Exported function
 *********************************************/
WE_UINT32 smaAddImageObjectToForm(WeImageHandle handle,
                                   const SisRegion *region,
                                   const WeColor *bgColor,
                                   WE_UINT32 objId,
                                   SiaSlideFormHandle formHandle);

WE_UINT32 smaAddTextObjectToForm(WeStringHandle handle,
                                  const SisRegion *region,
                                  const WeColor *fgColor,
                                  const WeColor *bgColor,
                                  WE_UINT32 objId,
                                  SiaSlideFormHandle formHandle);

WE_UINT32 smaAddAudioObjectToForm(const WeSoundHandle handle,
                                   WE_UINT32 objId,
                                   SiaSlideFormHandle formHandle);


void smaUpdateSlideFormActions(WE_BOOL isPlaying, 
    SiaSlideFormHandle formHandle);

SiaSlideFormHandle smaCreateSlideForm(const WeColor *backgroundColor);
void smaDeleteSlideForm(SiaSlideFormHandle formHandle);
WE_BOOL smaHideFormObject(SiaSlideFormHandle formHandle, WE_UINT32 objId);
WE_BOOL smaDisplayFormObject(SiaSlideFormHandle formHandle, WE_UINT32 objId);

void smaHandleGadgetNotif(WeNotificationType notif, WE_UINT32 handle, 
    SiaSlideFormHandle formHandle);
void smaHandleEvent(WeEventType event, WE_UINT32 handle,
    SiaSlideFormHandle formHandle);
void smaSlideFormInit(void);
void smaSlideFormTerminate(void);
SiaResult smaShowSlideForm(SiaSlideFormHandle formHandle);
void smaHideSlideForm(SiaSlideFormHandle formHandle);
void smaRegisterStepEvent(SiaSlideFormHandle formHandle, 
    SiaStateMachine fsm, int sig);
void smaUnregisterStepEvent(SiaSlideFormHandle formHandle);
void smaSlideFormStopSound(SiaSlideFormHandle formHandle);
#endif /* SAUIFORM_H */



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

#ifndef SASLIDE_H
#define SASLIDE_H

#ifndef SAUIFORM_H
#error Sauiform.h must be included berfore Saslide.h!
#endif


/*--- Definitions/Declarations ---*/

/*--- Types ---*/

/* Signal destinations.*/
typedef enum
{
    /*! Response from read operation
     *  - Param I : result
     *  - Param U1: handle
     *  - Param U2: Bytes read
     *  - Param P : -
     */        
    SIA_SIG_SLIDE_LOAD_OBJ_RSP,

	/*! Response from get DRM header operation
     *  - Param I : - 
     *  - Param U1: Result of the operation 
     *  - Param U2: Bytes read
     *  - Param P : The DRM header.
     */        
    SIA_SIG_SLIDE_DRE_MSG_RSP,

	/*! Response from get DRM object operation
     *  - Param I : - 
     *  - Param U1: Result of the operation 
     *  - Param U2: Buffer size if buffer is used.
     *  - Param P : NULL or the decoded data if buffer is used.
     */        
    SIA_SIG_SLIDE_DRE_OBJ_RSP,

    /*! Handling of the boundary part
     *  - Param I : - 
     *  - Param U1: Result of the operation 
     *  - Param U2: Buffer size if buffer is used.
     */
    SIA_SIG_READ_BOUNDARY_RSP

}SiaSlideSignalId;

/*********************************************
 * Exported function
 *********************************************/

void smaSlideInit(void);
void smaSlideTerminate(void);
void smaLoadSlide(WE_UINT32 msgId, WE_UINT8 number, 
    SisSlideInfoList *slideInfoList, const SiaSlideFormHandle formHandle, 
    SiaStateMachine callingFsm, int returnSig);

#endif /* SASLIDE_H */

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

#ifndef SALOAD_H
#define SALOAD_H


#ifndef MMS_IF_H
#error Mms_If.h must be included berfore Saload.h!
#endif

/*--- Definitions/Declarations ---*/

/*--- Types ---*/
typedef enum 
{
    SIA_LOAD_RESULT_OK,
    SIA_LOAD_RESULT_ERROR
} SiaLoadResult;


/* Signal destinations.*/
typedef enum
{
    /*! Handling get mms body part (pipe)
     *  - Param I : MmsResult, result of the operation
     *  - Param U1: - 
     *  - Param U2: -
     *  - Param P : -
     */
    SIA_SIG_LOAD_GET_MMS_BP_RSP,

    /*! Handling pipe to buffer response
     *  - Param I : MmsResult, result of the operation
     *  - Param U1: - 
     *  - Param U2: -
     *  - Param P : the buffer holding the text data
     */
    SIA_SIG_LOAD_GET_DATA_FROM_PIPE_RSP,
    /*! Notification signal for resource loaded
     *  - Param I : - 
     *  - Param U1: - 
     *  - Param U2: -
     */
    SIA_SIG_LOAD_RESOURCE_READY,
    
    /*! Notification signal for resource failed
     *  - Param I : - 
     *  - Param U1: - 
     *  - Param U2: -
     */
    SIA_SIG_LOAD_RESOURCE_FAILED

}SiaLoadSignalId;

/*********************************************
 * Exported function
 *********************************************/

void smaLoadInit(void);
void smaLoadTerminate(void);
void smaLoadBpToHandle(SiaStateMachine retFsm, int retSig, WE_UINT32 msgId, 
     const SiaObjectInfo *objInfo);
const char *smaGetNameParam(const MmsAllParams *param);
const char *smaGetFileNameParam(const MmsAllParams *param);
void smaLoadHandleGetBpRsp(const MmsGetBodyPartReply *bodyPart);

#endif /* SALOAD_H */

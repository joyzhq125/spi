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

/*! \file mamr.h
 *  \brief Opens an existing message and reads its content into memory for 
 *         further processing by the ME and SE FSM.
 */

#ifndef _MAMR_H_
#define _MAMR_H_

/* Prerequbsites */
#ifndef MSA_INTSIG_H
#error maintsig.h needs to be included before mame.h!
#endif

/******************************************************************************
 * Constants
 *****************************************************************************/

/*!\enum MsaMrSignalId
 * \brief Signals for the MR FSM 
 */
typedef enum 
{
    /*! Starts the MR FSM
     * u_param1 = FSM, the FSM to send a callback signal to.
     * i_param  = signal, the callback signal.
     * p_param  = Start-up data, see #MsaMrStartData.
     */
    MSA_SIG_MR_START,

    /*! The response from the MMSif_getMsgHeader request
     * 
     * u_param1 = The result see #MmsResult.
     * p_param  = The message header.
     */
    MSA_SIG_MR_GET_HEADER_RSP,

    /*! The response from the MMSif_getBodyPart request
     * 
     * u_param1 = The result see #MmsResult.
     * p_param  = The pipe-name where to data is retrieved
     */
    MSA_SIG_MR_GET_BODYPART_RSP,

    /*! The response from the MMSif_getMsgSkeleton request
     *	
     * u_param1 = The result, see #MmsResult.
     * p_param  = The body information list.
     */
    MSA_SIG_MR_GET_SKELETON_RSP,

    /* Pipe read response.
     *	
     */
    MSA_SIG_MR_PIPE_RSP,










    

    /*! The response when starting the SIS.
     */
    MSA_SIG_MR_SIS_RSP,

    /*! 
     * The response from a SISif_openSmil operation.
     * i_param  = result, see #SisResult.
     * u_param1 = number of slides.
     * p_param  = the file name.
     */
    MSA_SIG_MR_OPEN_SMIL_RSP,

    /*! The response from a SISif_closeSmil operation.
     * u_param1 = The result, see #SisResult.
     */
    MSA_SIG_MR_CLOSE_SMIL_RSP,

    /*! The response from a SSISif_getSlideInfo operation.
     * u_param1 = 
     * u_param2 = 
     * p_param  = A #SisSlideInfo structure.
     */
    MSA_SIG_MR_GET_SLIDE_INFO_RSP

}MsaMrSignalId;

/*!\enum MsaMrStartupMode 
 * \brief Startup message types for the MR
 */
typedef enum
{
    MSA_MR_STARTUP_NORMAL    = 0x01, /*!< Edit message  */
    MSA_MR_STARTUP_FORWARD   = 0x02, /*!< Forward message     */
    MSA_MR_STARTUP_REPLY     = 0x04, /*!< Reply */
    MSA_MR_STARTUP_REPLY_ALL = 0x08, /*!< Reply to all */
    MSA_MR_STARTUP_VIEW      = 0x16  /*!< View message */
}MsaMrStartupMode;

/*! \enum MsaMcrResult 
 *  \brief Result codes from the MR FSM. 
 */
typedef enum
{
    MSA_MR_FAILED,          /*!< The operation failed */
    MSA_MR_OK,              /*!< The operation was successful */
    MSA_MR_BUSY,            /*!< The FSM is busy */
    MSA_MR_FORWARD_LOCK,    /*!< The PDU contains forward locked information
                                 and cannot be changed*/
    MSA_MR_UNSUPPORTED_HEADER_FIELDS /*!< The header cannot be handled */
}MsaMrResult;

/******************************************************************************
 * Data-types
 *****************************************************************************/

/*!\struct MsaMrStartData
 * \brief Used to transport MR startup information through a signal
 */
typedef struct
{
    MsaMrStartupMode    mode;       /*!< Startup mode */
    MmsMsgId            msgId;      /*!< Current message WID */
    MmsFileType         fileType;   /*!< Current file type */
    MsaStateMachine     retFsm;     /*!< Who to tell when done */
    int                 retSig;     /*!< What to say */    
}MsaMrStartData;

/******************************************************************************
 * Prototype declarations 
 *****************************************************************************/

void msaMrInit(void); 
void msaMrTerminate(void);
void msaStartMr(MsaMrStartupMode mode, MmsMsgId msgId, MmsFileType fileType, 
    MsaStateMachine retFsm, unsigned int retSig);
void msaFreeMessage(MsaMessage **msg);
                      
#endif /* _MAMR_H_ */


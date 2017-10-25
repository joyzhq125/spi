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

/*! \file macore.h
 *  Core functions for MSA.
 */

#ifndef _MACORE_H_
#define _MACORE_H_

#ifndef MSA_INTSIG_H
#error You must include maintsig.h before macore.h!
#endif

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! \enum MsaCoreSignalId
 *  Signals for the CORE FSM */
typedef enum 
{
    /*! Terminates the MSA application by calling the MSA_TERMINATE macro
     */
    MSA_SIG_CORE_TERMINATE_APP,

    /*! The SIA module is activated
     * u_param1 = module wid
     */
    MSA_SIG_CORE_MODULE_ACTIVE,

    /*! A module is deactivated
     * u_param1 = module wid
     */
    MSA_SIG_CORE_MODULE_TERMINATED,

    /*! The SIA module is started.
     *
     */
    MSA_SIG_CORE_SIA_STARTED_PLY,

    /*! The SIA has stopped playing.
     * u_param = the result, see #SiaResult
     */
    MSA_SIG_CORE_SIA_STOP
     
}MsaCoreSignalId;

/*! \enum MsaStartupMode
 *  \brief The startup modes for MSA. 
 *  MSA can be started in two different ways - context started from another 
 *  application as a "functionality extension" for that application, or by 
 * itself from a program manager 
 */
typedef enum
{
    MSA_STARTUP_MODE_NEW,      /*!< Content is attached to the start-up of the
                                     MSA */
    MSA_STARTUP_MODE_NEW_TEMPLATE, /*!< Create a new template  */
    MSA_STARTUP_MODE_EDIT,     /*!< Edit a Send.req message */
    MSA_STARTUP_MODE_REPLY,    /*!< Replay to a message */  
    MSA_STARTUP_MODE_REPLY_ALL,/*!< Replay to a message */  
    MSA_STARTUP_MODE_FORWARD,  /*!< Forward a message */ 
    MSA_STARTUP_MODE_DELETE,   /*!< Delete a message */
    MSA_STARTUP_MODE_VIEW,     /*!< View a message */
    MSA_STARTUP_MODE_SCHEME,   /*!< A MMSTO:, MMS: or MAILTO: scheme is 
                                    attached to the start-up of the MSA */
    MSA_STARTUP_MODE_CONFIG,   /*!< Configuration */
    MSA_STARTUP_MODE_SEND,     /*!< Send a message  */
    MSA_STARTUP_MODE_DOWNLOAD, /*!< Download a message  */
    MSA_STARTUP_MODE_PROVISIONING,/*!< Wait for provisioning */
    MSA_STARTUP_MODE_NONE      /*!< The MSA has not been started */
}MsaStartupMode;

/*! Close down with, but show a dialog first */
#define MSA_PANIC_IMPLICIT          0x01
/*! Close down without any dialog */
#define MSA_PANIC_EXPLICIT          0x02
/*! Define for telling MSA to shut down */
#define MSA_PANIC                   0x01

/*! Terminate the MSA */
#define MSA_TERMINATE               msaPanic(FALSE)

extern jmp_buf msaJmpBuf;
extern int msaPanicMode;

/******************************************************************************
 * Data-types
 *****************************************************************************/

/******************************************************************************
 * Prototypes 
 *****************************************************************************/

void msaInit(WE_BOOL internalOnly);
void msaTerminate(WE_BOOL internalOnly);

void msaCoreInit(WE_BOOL internalOnly);
void msaCoreTerminate(WE_BOOL internalOnly);

WE_BOOL msaIsExceptionAllowed(void);
void msaAllowException(WE_BOOL state);

WE_BOOL msaIsInsideRun(void);
void msaSetInsideRun(WE_BOOL state);

void msaModuleStart(WE_UINT32 modId, WE_BOOL useCallback, 
    MsaStateMachine fsm, unsigned int signal);
void msaModuleStarted(WE_UINT32 modId);
void msaModuleStop(WE_UINT32 modId, WE_BOOL terminateApp);
WE_BOOL msaIsModuleStarted(WE_UINT32 modId);
void msaModuleStopped(WE_UINT32 modId);

void msaSetStartupParameters(const we_module_execute_cmd_t *command);
MsaStartupMode msaGetStartupMode(void);

void msaSetStartupFsm(MsaStateMachine stateM);
MsaStateMachine msaGetStartupFsm(void);

void msaPanic(int showDialog);
void msaHandleMmsErrors(int result);

void msaActivateStartupFsm(void);
void msaPlaySmil(MmsMsgId msgId, MmsFileType fileType, MsaStateMachine fsm, 
    unsigned int sig);

#endif

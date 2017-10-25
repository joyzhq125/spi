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

#ifndef SACORE_H
#define SACORE_H


#ifndef SIS_IF_H
#error Sis_if.h must be included berfore Sacore.h!
#endif

#ifndef SATYPES_H
#error Satypes.h must be included berfore Sacore.h!
#endif

#ifndef SIA_DEF_H
#error Sia_def.h must be included berfore Sacore.h!
#endif

/*--- Definitions/Declarations ---*/

/*! Panic code for longjmp */
#define SIA_PANIC 1
/*! Should only be used internally. SMA is out of memory */
#define SIA_PANIC_OUT_OF_MEMORY 1
/*! Should only be used internally. Just exit (dont terminate application)*/
#define SIA_PANIC_NO_DIALOG 3
/*! Should only be used internally. Terminate SMA application */
#define SIA_PANIC_TERMINATE 4

/* Shut down states */
/*! Should only be used internally. Terminate SIS */
#define SIA_TERM_MODULES 0
/*! Should only be used internally. Terminate SMA application */
#define SIA_TERM_DIE    1
/*! Should only be used internally. Exit SMA application and return signal
 *  to caller, but no shutdown */
#define SIA_TERM_EXIT   2

/*! Exit the smil player */
#define EXIT_SMIL_PLAYER(result) (void)SIA_SIGNAL_SENDTO_IU(SIA_FSM_CORE, \
    SIA_SIG_CORE_TERMINATE_APP, (result), SIA_TERM_EXIT)

/*! Terminate the smil player */
#define TERMINATE_SMIL_PLAYER(result, state) (void)SIA_SIGNAL_SENDTO_IU(SIA_FSM_CORE, \
    SIA_SIG_CORE_TERMINATE_APP, (result), (state))


/*--- Types ---*/

typedef enum
{
    /*! Terminate SMA 
     */
    SIA_SIG_CORE_TERMINATE_APP
} SiaCoreStates;

extern int smaPanicMode;

/*********************************************
 * Exported function
 *********************************************/
void initSia(void);
void terminateSia(void);

void smaCoreInit(void);
void smaCoreTerminate(void);

void exitSmilPlayer(SiaResult result, WE_BOOL sendRsp);
WE_BOOL smaIsRunning(void);
void setSiaIsRunning( WE_BOOL state);
SiaResult smaGetSiaExitResult(void);
void smaSetJumpBuffer(jmp_buf *jmpbuf);
WE_BOOL smaIsInitialized(void);
void setSiaIsInitialized( WE_BOOL state);


/*!
 * \brief This function should be used internally (i.e. be called by the SMA)
 *
 *****************************************************************************/
void smaPanic(int mode, SiaResult result);

#endif /* SACORE_H */

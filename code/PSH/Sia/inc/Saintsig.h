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

#ifndef SAINTSIG_H
#define SAINTSIG_H

/*--- Definitions/Declarations ---*/

/*--- Types ---*/

/*********************************************
 * Signal destinations.
 * The constants here are chosen to allow
 * each destination a large enough interval
 * in which to define its signal values.
 *********************************************/


typedef enum
{
    SIA_NOTUSED_FSM,    /*!< N/A ! Must be the first one. */
    SIA_FSM_CORE,       /* FSM used for core functions */
    SIA_FSM_CRH,        /* FSM used for content routing functions */
    SIA_FSM_SMENU,
    SIA_FSM_MAIN,       /* FSM used for message handling */
    SIA_FSM_ASYNC,      /* FSM used for async operations */
    SIA_FSM_SIS,        /* FSM used when accessing SIS */
    SIA_FSM_PH,         /* FSM package handling */
    SIA_FSM_SBP,        /* FSM used to save bodypart to file */
    SIA_FSM_SLIDE,      /* FSM used to generate a slide */	
    SIA_FSM_LOAD,       /* FSM used to read bodyparts */	
	SIA_FSM_PIPE,		/* FSM for handling pipe operations */	
    SIA_FSM_ATTACHMENT,	/* FSM for handling attachment operations */	
    SIA_FSM_MAX_REGS	/* Number of State Machines */	
} SiaStateMachine;


/**************************************************
 * Macros
 **************************************************/

#define SIA_SIGNAL_SENDTO(d, sig) \
  smaSignalSendTo ((SiaStateMachine)(d), (sig), 0, 0, 0, NULL)

#define SIA_SIGNAL_SENDTO_I(d, sig, i) \
  smaSignalSendTo ((SiaStateMachine)(d), (sig), i, 0, 0, NULL)

#define SIA_SIGNAL_SENDTO_U(d, sig, u) \
  smaSignalSendTo ((SiaStateMachine)(d), (sig), 0, (u), 0, NULL)

#define SIA_SIGNAL_SENDTO_IU(d, sig, i, u) \
  smaSignalSendTo ((SiaStateMachine)(d), (sig), (i), (u), 0, NULL)

#define SIA_SIGNAL_SENDTO_UU(d, sig, u1, u2) \
  smaSignalSendTo ((SiaStateMachine)(d), (sig), 0, (u1), (u2), NULL)

#define SIA_SIGNAL_SENDTO_IUU(d, sig, i, u1, u2) \
  smaSignalSendTo ((SiaStateMachine)(d), (sig), (i), (u1), (u2), NULL)

#define SIA_SIGNAL_SENDTO_P(d, sig, p) \
  smaSignalSendTo ((SiaStateMachine)(d), (sig), 0, 0, 0, (p))

#define SIA_SIGNAL_SENDTO_IP(d, sig, i, p) \
  smaSignalSendTo ((SiaStateMachine)(d), (sig), (i), 0, 0, (p))

#define SIA_SIGNAL_SENDTO_UP(d, sig, u, p) \
  smaSignalSendTo ((SiaStateMachine)(d), (sig), 0, (u), 0, (p))

#define SIA_SIGNAL_SENDTO_IUP(d, sig, i, u, p) \
  smaSignalSendTo ((SiaStateMachine)(d), (sig), (i), (u), 0, (p))

#define SIA_SIGNAL_SENDTO_UUP(d, sig, u1, u2, p) \
  smaSignalSendTo ((SiaStateMachine)(d), (sig), 0, (u1), (u2), (p))

#define SIA_SIGNAL_SENDTO_IUUP(d, sig, i, u1, u2, p) \
  smaSignalSendTo ((SiaStateMachine)(d), (sig), (i), (u1), (u2), (p))



/****************************************
 * Type definitions:
 ****************************************/

typedef struct SiaSignalStruct {
  struct SiaSignalStruct *next;
  int                    dst;      /* The destination for this signal */
  int                    type;     /* Which signal it is */
  long int               i_param;  /* Parameters: */
  unsigned long int      u_param1;
  unsigned long int      u_param2;
  void                  *p_param;
} SiaSignal;

/*
 * This kind of function is called when a signal is delivered.
 */
typedef void SiaSignalFunction (SiaSignal *);


/****************************************
 * Exported functions:
 ****************************************/

/*
 * Initialize the signal module.
 */
void smaSignalInit (void);

/*
 * Terminate the signal module.
 */
void smaSignalTerminate (void);


/*
 * Process the first signal in the signal queue.
 */
void smaSignalProcess(void);

/*
 * Return TRUE if the global signal queue is empty,
 * FALSE otherwise.
 */
int smaSignalQueueIsEmpty(void);

/*
 * Delete a signal and its parameters.
 */
void smaSignalDelete (SiaSignal *sig);

/*
 * Register the function "f" to be called when a signal
 * is delivered to destination "dst".
 */
void smaSignalRegisterDst (SiaStateMachine dst, SiaSignalFunction *f);

/*
 * Remove the registrated function for destination "dst".
 */
void smaSignalDeregister ( SiaStateMachine dst);

/*
 * Add a new signal to the end of the global queue.
 */
SiaSignal *smaSignalSendTo( SiaStateMachine dst, 
                           int signum,
                           long i_param,
                           unsigned long u_param1,
                           unsigned long u_param2,
                           void *p_param);


/*
 * Add an old signal to the beginning of the queue.
 */
void smaSignalPrepend(SiaSignal *sig);

#endif /* SAINTSIG_H */

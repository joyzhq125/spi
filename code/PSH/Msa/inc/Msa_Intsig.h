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

/*! \file maintsig.h
 *  \brief  Signals used in the internal state machines.
 */

#ifndef MSA_INTSIG_H
#define MSA_INTSIG_H

/*! \enum MsaStateMachine State machines for the MSA module */
typedef enum
{
    MSA_NOTUSED_FSM,/*!< N/A ! Must be the first one. */
    MSA_CONFIG_FSM, /*!< Configuration*/
    MSA_DEL_FSM,    /*!< Message deletion*/
    MSA_MV_FSM,     /*!< Message Viewer*/
    MSA_MMV_FSM,    /*!< Multipart/mixed Viewer */
    MSA_MME_FSM,    /*!< Multipart/mixed editor */
    MSA_MOB_FSM,    /*!< Media object browser */
    MSA_SE_FSM,     /*!< SMIL Editor*/
    MSA_ME_FSM,     /*!< Message Editor */
    MSA_ASYNC_FSM,  /*!< Asyncronous operations */
    MSA_MCR_FSM,    /*!< Message Creation and Retrieval FSM */
    MSA_CRH_FSM,    /*!< Content routing handling */
    MSA_COMM_FSM,   /*!< Communication handling */
    MSA_CORE_FSM,   /*!< Core */
    MSA_MR_FSM,     /*!< Message reader */
    MSA_PBH_FSM,    /*!< The phone book handler */
    MSA_CTH_FSM,    /*!< Cross triggering handler */
    MSA_PH_FSM,     /*!< Packet Handler */
    MSA_PIPE_FSM,   /*!< Pipe-handler */
    MSA_FSM_MAX_REGS            /* Number of State Machines. */
} MsaStateMachine;

#define MSA_SIGNAL_SENDTO(d, sig) \
    msaSignalSendTo((d), (sig), (unsigned long)0L, (unsigned long)0L, \
    (unsigned long)0L, NULL)

#define MSA_SIGNAL_SENDTO_I(d, sig, i) \
    msaSignalSendTo((d), (sig), i, (unsigned long)0L, (unsigned long)0L, NULL)

#define MSA_SIGNAL_SENDTO_IU(d, sig, i, u) \
    msaSignalSendTo((d), (sig), (i), (u), 0L, NULL)

#define MSA_SIGNAL_SENDTO_IUU(d, sig, i, u1, u2) \
    msaSignalSendTo((d), (sig), (i), (u1), (u2), NULL)

#define MSA_SIGNAL_SENDTO_IUP(d, sig, i, u, p) \
    msaSignalSendTo((d), (sig), (i), (u), (unsigned long)0L, (p))

#define MSA_SIGNAL_SENDTO_IUUP(d, sig, i, u1, u2, p) \
    msaSignalSendTo((d), (sig), (i), (u1), (u2), (p))

#define MSA_SIGNAL_SENDTO_IP(d, sig, i, p) \
    msaSignalSendTo((d), (sig), (i), (unsigned long)0L, (unsigned long)0L, \
        (p))

#define MSA_SIGNAL_SENDTO_U(d, sig, u) \
    msaSignalSendTo((d), (sig), (unsigned long)0L, (u), (unsigned long)0L, \
        NULL)

#define MSA_SIGNAL_SENDTO_UU(d, sig, u1, u2) \
    msaSignalSendTo((d), (sig), (unsigned long)0L, (u1), (u2), NULL)

#define MSA_SIGNAL_SENDTO_UUP(d, sig, u1, u2, p) \
    msaSignalSendTo((d), (sig), (unsigned long)0L, (u1), (u2), (p))

#define MSA_SIGNAL_SENDTO_UP(d, sig, u, p) \
    msaSignalSendTo((d), (sig), (unsigned long)0L, (u), 0L, (p))

#define MSA_SIGNAL_SENDTO_P(d, sig, p) \
    msaSignalSendTo((d), (sig), (unsigned long)0L, (unsigned long)0L, \
    (unsigned long)0L, (p))

/****************************************
 * Type definitions:
 ****************************************/
/*! \struct MsaSignal
 *  The signal data structure */
typedef struct MsaSignalStruct 
{
    struct MsaSignalStruct *next;       /*!< Next signal in the queue */
    MsaStateMachine         dst;        /*!< The destination FSM for this 
                                             signal */
    int                     type;       /*!< Which signal it is */
    long                    i_param;    /*!< Parameter */
    unsigned long           u_param1;   /*!< Parameter: Sometimes used for 
                                             'Sending FSM' */
    unsigned long           u_param2;   /*!< Parameter: Sometimes used for 
                                             'Expected Return Signal' */
    void                   *p_param;    /*!< Pointer to additional parameters*/
} MsaSignal;

/*
 * This kind of function is called when a signal is delivered.
 */
typedef void MsaSignalFunction (MsaSignal *);

/****************************************
 * Exported functions:
 ****************************************/

/*
 * Remove all signals with the specified wid and data for a state machine
 */
void msaSignalFlush(MsaStateMachine dst, int wid, long i_param,
    unsigned long u_param1, unsigned long u_param2, const void *p_param);

/*
 * Initialize the signal module.
 */
void msaSignalInit(void);

/*
 * Terminate the signal module.
 */
void msaSignalTerminate(void);

/*
 * Process the first signal in the signal queue.
 */
void msaSignalProcess(void);

/* Check if signal queue is empty
 *
 * Returns TRUE if the global signal queue is empty,
 *      FALSE otherwise.
 */
int msaSignalQueueIsEmpty(void);

/*
 * Delete a signal and its parameters.
 */
void msaSignalDelete(MsaSignal *sig);

/*
 * Remove the registered function for destination "dst".
 */
void msaSignalDeregister(MsaStateMachine dst);

/*
 * Register the function "f" to be called when a signal
 * is delivered to destination "dst".
 */
void msaSignalRegisterDst(MsaStateMachine dst, MsaSignalFunction *f);

/*
 * Add a new signal to the end of the global queue.
 */
MsaSignal *msaSignalSendTo(MsaStateMachine dst, int sig, long i_param,
    unsigned long u_param1, unsigned long u_param2, void *p_param);

/*
 * Add an old signal to the beginning of the queue.
 */
void msaSignalPrepend(MsaSignal *sig);

#endif

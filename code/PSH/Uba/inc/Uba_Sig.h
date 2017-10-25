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
/*
 * uba_sig.h
 *
 * Created by Mats Tedenvall, Wed Jul 10 10:20 2002.
 *
 * Revision history:
 *
 */
/************************************************************
 *
 * Sending and receiving signals in the protocol stack:
 *
 ************************************************************/
#ifndef UBA_SIG_H
#define UBA_SIG_H

/*********************************************
 * Signal destinations.
 * The constants here are chosen to allow
 * each destination a large enough interval
 * in which to define its signal values.
 *********************************************/

#define UBA_MODULE_MAIN          0x000


/**************************************************
 * Macros
 **************************************************/

#define UBA_SIGNAL_SENDTO(d, sig) \
  uba_signal_sendto ((d), (sig), 0, 0, 0, NULL)

#define UBA_SIGNAL_SENDTO_I(d, sig, i) \
  uba_signal_sendto ((d), (sig), i, 0, 0, NULL)

#define UBA_SIGNAL_SENDTO_U(d, sig, u) \
  uba_signal_sendto ((d), (sig), 0, (u), 0, NULL)

#define UBA_SIGNAL_SENDTO_IU(d, sig, i, u) \
  uba_signal_sendto ((d), (sig), (i), (u), 0, NULL)

#define UBA_SIGNAL_SENDTO_UU(d, sig, u1, u2) \
  uba_signal_sendto ((d), (sig), 0, (u1), (u2), NULL)

#define UBA_SIGNAL_SENDTO_IUU(d, sig, i, u1, u2) \
  uba_signal_sendto ((d), (sig), (i), (u1), (u2), NULL)

#define UBA_SIGNAL_SENDTO_P(d, sig, p) \
  uba_signal_sendto ((d), (sig), 0, 0, 0, (p))

#define UBA_SIGNAL_SENDTO_IP(d, sig, i, p) \
  uba_signal_sendto ((d), (sig), (i), 0, 0, (p))

#define UBA_SIGNAL_SENDTO_IUP(d, sig, i, u, p) \
  uba_signal_sendto ((d), (sig), (i), (u), 0, (p))

#define UBA_SIGNAL_SENDTO_IUUP(d, sig, i, u1, u2, p) \
  uba_signal_sendto ((d), (sig), (i), (u1), (u2), (p))


/****************************************
 * Type definitions:
 ****************************************/

typedef struct uba_signal_st {
  struct uba_signal_st *next;
  int                 dst;      /* The destination for this signal */
  int                 type;     /* Which signal it is */
  long int            i_param;  /* Parameters: */
  unsigned long int   u_param1;
  unsigned long int   u_param2;
  void               *p_param;
} uba_signal_t;

/*
 * This kind of function is called when a signal is delivered.
 */
typedef void uba_signal_function_t (uba_signal_t *);


/****************************************
 * Exported functions:
 ****************************************/

/*
 * Initialize the signal module.
 */
void
uba_signal_init (void);

/*
 * Terminate the signal module.
 */
void
uba_signal_terminate (void);


/*
 * Process the first signal in the signal queue.
 */
void
uba_signal_process (void);

/*
 * Return TRUE if the global signal queue is empty,
 * FALSE otherwise.
 */
int
uba_signal_queue_is_empty (void);

/*
 * Delete a signal and its parameters.
 */
void
uba_signal_delete (uba_signal_t *sig);

/*
 * Register the function "f" to be called when a signal
 * is delivered to destination "dst".
 */
void
uba_signal_register_dst (int dst, uba_signal_function_t *f);

/*
 * Remove the registrated function for destination "dst".
 */
void
uba_signal_deregister (int dst);

/*
 * Add a new signal to the end of the global queue.
 */
void
uba_signal_sendto (int dst,
                 int sig,
                 long int i_param,
                 unsigned long int u_param1,
                 unsigned long int u_param2,
                 void *p_param);

/*
 * Add an old signal to the beginning of the queue.
 */
void
uba_signal_prepend (uba_signal_t *sig);

#endif      /*UBA_SIG_H*/

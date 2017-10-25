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
 * uba_sig.c
 *
 * Created by Anders Evertsson, Thu Jun 5 2003.
 *
 * Revision history:
 *
 */
/************************************************************
 *
 * Sending and receiving signals in the protocol stack:
 *
 ************************************************************/
#include "Uba_Sig.h"
#include "We_Def.h"
#include "We_Mem.h"

/****************************************
 * Constants:
 ****************************************/

/* Max number of registered signal destinations: */
#define UBA_SIG_MAX_REGS               4

/* Max number of signal structs stored in the free list: */
#define UBA_SIG_MAX_FREE_LIST_LENGTH   10


/****************************************
 * Global variables:
 ****************************************/

/* The global queue of signals to be delivered: */
static uba_signal_t *uba_signal_queue_first;
static uba_signal_t *uba_signal_queue_last;

/* List of available signal structs. This is used
 * to reduce the number of allocations. */
static uba_signal_t *uba_signal_free_list;
static int         uba_signal_free_list_length;

/* The table of known destinations */
static uba_signal_function_t *uba_signal_func[UBA_SIG_MAX_REGS];


/****************************************
 * External function:
 ****************************************/

extern void
uba_signal_process_external (uba_signal_t *sig);


/*********************************************
 * Function definitions:
 *********************************************/

/*
 * Initialize the signal module.
 */
void
uba_signal_init (void)
{
  int i;

  uba_signal_queue_first = uba_signal_queue_last = NULL;
  uba_signal_free_list = NULL;
  uba_signal_free_list_length = 0;

  for (i = 0; i < UBA_SIG_MAX_REGS; i++)
    uba_signal_func[i] = NULL;
}

/*
 * Terminate the signal module.
 */
void
uba_signal_terminate (void)
{
  uba_signal_t *sig;
  int           i;

  while (!uba_signal_queue_is_empty ()) {
    sig = uba_signal_queue_first;
    uba_signal_queue_first = sig->next;
    uba_signal_delete (sig);
  }

  while (uba_signal_free_list != NULL) {
    sig = uba_signal_free_list;
    uba_signal_free_list = sig->next;
    WE_MEM_FREE (WE_MODID_UBA, sig);
  }

  for (i = 0; i < UBA_SIG_MAX_REGS; i++)
    uba_signal_func[i] = NULL;
}


/*
 * Process the first signal in the signal queue.
 */
void
uba_signal_process (void)
{
  uba_signal_t          *sig;
  uba_signal_function_t *func;
  int                    idx;

  sig = uba_signal_queue_first;
  if (sig != NULL) {
    uba_signal_queue_first = sig->next;
    if (uba_signal_queue_first == NULL)
      uba_signal_queue_last = NULL;
    idx = sig->dst >> 4;
    if ((idx >= 0) && (idx < UBA_SIG_MAX_REGS) &&
        ((func = uba_signal_func[idx]) != NULL))
      func (sig);
    else
      uba_signal_delete (sig);
  }
}

/*
 * Register the function "f" to be called when a signal
 * is delivered to destination "dst".
 */
void
uba_signal_register_dst (int dst, uba_signal_function_t *f)
{
  uba_signal_func[dst >> 4] = f;
}

/*
 * Remove the registrated function for destination "dst".
 */
void
uba_signal_deregister (int dst)
{
  uba_signal_func[dst >> 4] = NULL;
}

/*
 * Delete a signal.
 */
void
uba_signal_delete (uba_signal_t *sig)
{
  if (sig == NULL)
    return;

  if (uba_signal_free_list_length < UBA_SIG_MAX_FREE_LIST_LENGTH) {
    sig->next = uba_signal_free_list;
    uba_signal_free_list = sig;
    uba_signal_free_list_length++;
  }
  else
    WE_MEM_FREE (WE_MODID_UBA, sig);
}

/*
 * Return TRUE if the global signal queue is empty,
 * FALSE otherwise.
 */
int
uba_signal_queue_is_empty (void)
{
  return uba_signal_queue_first == NULL;
}

/*
 * Add a new signal to the end of the global queue.
 */
void
uba_signal_sendto (int dst, int signum,
                 long int i_param,
                 unsigned long int u_param1,
                 unsigned long int u_param2,
                 void *p_param)
{
  uba_signal_t *sig;

  if (uba_signal_free_list == NULL) {
    sig = WE_MEM_ALLOCTYPE (WE_MODID_UBA, uba_signal_t);
    if (sig == NULL) {
      WE_MEM_FREE (WE_MODID_UBA, p_param);
      return;
    }
  }
  else {
    sig = uba_signal_free_list;
    uba_signal_free_list = sig->next;
    uba_signal_free_list_length--;
  }

  sig->type = signum;
  sig->dst = dst;
  sig->i_param = i_param;
  sig->u_param1 = u_param1;
  sig->u_param2 = u_param2;
  sig->p_param = p_param;

  sig->next = NULL;
  if (uba_signal_queue_last != NULL) {
    uba_signal_queue_last->next = sig;
    uba_signal_queue_last = sig;
  }
  else {
    uba_signal_queue_first = uba_signal_queue_last = sig;
  }
}

/*
 * Add an old signal to the beginning of the queue.
 */
void
uba_signal_prepend (uba_signal_t *sig)
{
  if (sig == NULL)
    return;

  sig->next = uba_signal_queue_first;
  if (uba_signal_queue_last == NULL)
    uba_signal_queue_last = sig;
  uba_signal_queue_first = sig;
}

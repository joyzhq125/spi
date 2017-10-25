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














#include "Frw_Sig.h"
#include "We_Mem.h"

#include "We_Def.h"






#define FRW_SIG_MAX_REGS               5


#define FRW_SIG_MAX_FREE_LIST_LENGTH   10







static frw_signal_t *frw_signal_queue_first;
static frw_signal_t *frw_signal_queue_last;



static frw_signal_t *frw_signal_free_list;
static int         frw_signal_free_list_length;


static frw_signal_function_t *frw_signal_func[FRW_SIG_MAX_REGS];






extern void
frw_signal_process_external (frw_signal_t *sig);









void
frw_signal_init (void)
{
  int i;

  frw_signal_queue_first = frw_signal_queue_last = NULL;
  frw_signal_free_list = NULL;
  frw_signal_free_list_length = 0;

  for (i = 0; i < FRW_SIG_MAX_REGS; i++)
    frw_signal_func[i] = NULL;
}




void
frw_signal_terminate (void)
{
  frw_signal_t *sig;
  int           i;

  while (!frw_signal_queue_is_empty ()) {
    sig = frw_signal_queue_first;
    frw_signal_queue_first = sig->next;
    frw_signal_delete (sig);
  }

  while (frw_signal_free_list != NULL) {
    sig = frw_signal_free_list;
    frw_signal_free_list = sig->next;
    WE_MEM_FREE (WE_MODID_FRW, sig);
  }

  for (i = 0; i < FRW_SIG_MAX_REGS; i++)
    frw_signal_func[i] = NULL;
}





void
frw_signal_process (void)
{
  frw_signal_t          *sig;
  frw_signal_function_t *func;
  int                    idx;

  sig = frw_signal_queue_first;
  if (sig != NULL) {
    frw_signal_queue_first = sig->next;
    if (frw_signal_queue_first == NULL)
      frw_signal_queue_last = NULL;
    idx = sig->dst >> 4;
    if ((idx >= 0) && (idx < FRW_SIG_MAX_REGS) &&
        ((func = frw_signal_func[idx]) != NULL))
      func (sig);
    else
      frw_signal_delete (sig);
  }
}





void
frw_signal_register_dst (int dst, frw_signal_function_t *f)
{
  frw_signal_func[dst >> 4] = f;
}




void
frw_signal_deregister (int dst)
{
  frw_signal_func[dst >> 4] = NULL;
}




void
frw_signal_delete (frw_signal_t *sig)
{
  if (sig == NULL)
    return;

  if (frw_signal_free_list_length < FRW_SIG_MAX_FREE_LIST_LENGTH) {
    sig->next = frw_signal_free_list;
    frw_signal_free_list = sig;
    frw_signal_free_list_length++;
  }
  else
    WE_MEM_FREE (WE_MODID_FRW, sig);
}





int
frw_signal_queue_is_empty (void)
{
  return frw_signal_queue_first == NULL;
}




void
frw_signal_sendto (int dst, int signum,
                 long int i_param,
                 unsigned long int u_param1,
                 unsigned long int u_param2,
                 void *p_param)
{
  frw_signal_t *sig;

  if (frw_signal_free_list == NULL) {
    sig = WE_MEM_ALLOCTYPE (WE_MODID_FRW, frw_signal_t);
    if (sig == NULL) {
      WE_MEM_FREE (WE_MODID_FRW, p_param);
      return;
    }
  }
  else {
    sig = frw_signal_free_list;
    frw_signal_free_list = sig->next;
    frw_signal_free_list_length--;
  }

  sig->type = signum;
  sig->dst = dst;
  sig->i_param = i_param;
  sig->u_param1 = u_param1;
  sig->u_param2 = u_param2;
  sig->p_param = p_param;

  sig->next = NULL;
  if (frw_signal_queue_last != NULL) {
    frw_signal_queue_last->next = sig;
    frw_signal_queue_last = sig;
  }
  else {
    frw_signal_queue_first = frw_signal_queue_last = sig;
  }
}




void
frw_signal_prepend (frw_signal_t *sig)
{
  if (sig == NULL)
    return;

  sig->next = frw_signal_queue_first;
  if (frw_signal_queue_last == NULL)
    frw_signal_queue_last = sig;
  frw_signal_queue_first = sig;
}

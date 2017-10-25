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








 
#include "Ubs_Sig.h"
#include "We_Def.h"
#include "We_Mem.h"
#include "Ubs_Main.h"






#define UBS_SIG_MAX_REGS               5


#define UBS_SIG_MAX_FREE_LIST_LENGTH   10






static ubs_signal_t *ubs_signal_queue_first;
static ubs_signal_t *ubs_signal_queue_last;



static ubs_signal_t *ubs_signal_free_list;
static int           ubs_signal_free_list_length;


static ubs_signal_function_t *ubs_signal_func[UBS_SIG_MAX_REGS];




 



void
ubs_signal_init (void)
{
  int i;

  ubs_signal_queue_first = ubs_signal_queue_last = NULL;
  ubs_signal_free_list = NULL;
  ubs_signal_free_list_length = 0;

  for (i = 0; i < UBS_SIG_MAX_REGS; i++)
    ubs_signal_func[i] = NULL;
}




void
ubs_signal_terminate (void)
{
  ubs_signal_t *sig;
  int           i;

  while (!ubs_signal_queue_is_empty ()) {
    sig = ubs_signal_queue_first;
    ubs_signal_queue_first = sig->next;
    ubs_signal_delete (sig);
  }

  while (ubs_signal_free_list != NULL) {
    sig = ubs_signal_free_list;
    ubs_signal_free_list = sig->next;
    UBS_MEM_FREE (sig);
  }

  for (i = 0; i < UBS_SIG_MAX_REGS; i++)
    ubs_signal_func[i] = NULL;

}





void
ubs_signal_register_dst (int dst, ubs_signal_function_t *f)
{
  ubs_signal_func[dst >> 4] = f;
}




void
ubs_signal_process (void)
{
  ubs_signal_t          *sig;
  ubs_signal_function_t *func;
  int                    idx;

  sig = ubs_signal_queue_first;
  if (sig != NULL) {
    ubs_signal_queue_first = sig->next;
    if (ubs_signal_queue_first == NULL)
      ubs_signal_queue_last = NULL;
    idx = sig->dst >> 4;
    if ((idx >= 0) && (idx < UBS_SIG_MAX_REGS) &&
        ((func = ubs_signal_func[idx]) != NULL))
      func (sig);
    else
      ubs_signal_delete (sig);
  }
}





int
ubs_signal_queue_is_empty (void)
{
  return ubs_signal_queue_first == NULL;
}




void
ubs_signal_sendto (int dst, int signum,
                   long int i_param,
                   unsigned long int u_param,
                   void *p_param)
{
  ubs_signal_t *sig;

  if (ubs_signal_free_list == NULL) {
    sig = UBS_MEM_ALLOCTYPE (ubs_signal_t);
    if (sig == NULL) {
      UBS_MEM_FREE (p_param);
      UBS_MEM_FREE (p_param);
      return;
    }
  }
  else {
    sig = ubs_signal_free_list;
    ubs_signal_free_list = sig->next;
    ubs_signal_free_list_length--;
  }

  sig->type = signum;
  sig->dst = dst;
  sig->i_param = i_param;
  sig->u_param = u_param;
  sig->p_param = p_param;

  sig->next = NULL;
  if (ubs_signal_queue_last != NULL) {
    ubs_signal_queue_last->next = sig;
    ubs_signal_queue_last = sig;
  }
  else {
    ubs_signal_queue_first = ubs_signal_queue_last = sig;
  }
}




void
ubs_signal_delete (ubs_signal_t *sig)
{
  if (sig == NULL)
    return;

  if (ubs_signal_free_list_length < UBS_SIG_MAX_FREE_LIST_LENGTH) {
    sig->next = ubs_signal_free_list;
    ubs_signal_free_list = sig;
    ubs_signal_free_list_length++;
  }
  else
    UBS_MEM_FREE (sig);
}

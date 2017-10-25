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








 
#include "Pus_Sig.h"
#include "we_def.h"
#include "we_mem.h"
#include "Pus_Main.h"






#define PUS_SIG_MAX_REGS               10


#define PUS_SIG_MAX_FREE_LIST_LENGTH   10






static Pus_Signal_t *Pus_Signal_queue_first;
static Pus_Signal_t *Pus_Signal_queue_last;



static Pus_Signal_t *Pus_Signal_free_list;
static int           Pus_Signal_free_list_length;


static Pus_Signal_function_t *Pus_Signal_func[PUS_SIG_MAX_REGS];




 



void
Pus_Signal_init (void)
{
  int i;

  Pus_Signal_queue_first = Pus_Signal_queue_last = NULL;
  Pus_Signal_free_list = NULL;
  Pus_Signal_free_list_length = 0;

  for (i = 0; i < PUS_SIG_MAX_REGS; i++)
    Pus_Signal_func[i] = NULL;
}




void
Pus_Signal_terminate (void)
{
  Pus_Signal_t *sig;
  int           i;

  while (!Pus_Signal_queue_is_empty ()) {
    sig = Pus_Signal_queue_first;
    Pus_Signal_queue_first = sig->next;
    Pus_Signal_delete (sig);
  }

  while (Pus_Signal_free_list != NULL) {
    sig = Pus_Signal_free_list;
    Pus_Signal_free_list = sig->next;
    PUS_MEM_FREE (sig);
  }

  for (i = 0; i < PUS_SIG_MAX_REGS; i++)
    Pus_Signal_func[i] = NULL;

}





void
Pus_Signal_register_dst (int dst, Pus_Signal_function_t *f)
{
  Pus_Signal_func[dst >> 4] = f;
}




void
Pus_Signal_process (void)
{
  Pus_Signal_t          *sig;
  Pus_Signal_function_t *func;
  int                    idx;

  sig = Pus_Signal_queue_first;
  if (sig != NULL) {
    Pus_Signal_queue_first = sig->next;
    if (Pus_Signal_queue_first == NULL)
      Pus_Signal_queue_last = NULL;
    idx = sig->dst >> 4;
    if ((idx >= 0) && (idx < PUS_SIG_MAX_REGS) &&
        ((func = Pus_Signal_func[idx]) != NULL))
      func (sig);
    else
      Pus_Signal_delete (sig);
  }
}





int
Pus_Signal_queue_is_empty (void)
{
  return Pus_Signal_queue_first == NULL;
}




void
Pus_Signal_sendto (int dst, int signum,
                   long int i_param1,
                   long int i_param2,
                   unsigned long int u_param,
                   void *p_param)
{
  Pus_Signal_t *sig;

  if (Pus_Signal_free_list == NULL) {
    sig = PUS_MEM_ALLOCTYPE (Pus_Signal_t);
    if (sig == NULL) {
      PUS_MEM_FREE (p_param);
      PUS_MEM_FREE (p_param);
      return;
    }
  }
  else {
    sig = Pus_Signal_free_list;
    Pus_Signal_free_list = sig->next;
    Pus_Signal_free_list_length--;
  }

  sig->type = signum;
  sig->dst = dst;
  sig->i_param1 = i_param1;
  sig->i_param2 = i_param2;
  sig->u_param = u_param;
  sig->p_param = p_param;

  sig->next = NULL;
  if (Pus_Signal_queue_last != NULL) {
    Pus_Signal_queue_last->next = sig;
    Pus_Signal_queue_last = sig;
  }
  else {
    Pus_Signal_queue_first = Pus_Signal_queue_last = sig;
  }
}




void
Pus_Signal_delete (Pus_Signal_t *sig)
{
  if (sig == NULL)
    return;

  if (Pus_Signal_free_list_length < PUS_SIG_MAX_FREE_LIST_LENGTH) {
    sig->next = Pus_Signal_free_list;
    Pus_Signal_free_list = sig;
    Pus_Signal_free_list_length++;
  }
  else
    PUS_MEM_FREE (sig);
}

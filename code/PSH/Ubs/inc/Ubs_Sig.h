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








 






#ifndef UBS_SIG_H
#define UBS_SIG_H








#define UBS_MODULE_MAIN           0x000
#define UBS_MODULE_MSG_STORE      0x020





#define UBS_SIGNAL_SENDTO_UP(d, sig, u, p) \
  ubs_signal_sendto ((d), (sig), 0, (u), (p))

#define UBS_SIGNAL_SENDTO_IU(d, sig, i, u) \
  ubs_signal_sendto ((d), (sig), (i), (u), NULL)

#define UBS_SIGNAL_SENDTO_P(d, sig, p) \
  ubs_signal_sendto ((d), (sig), 0, 0, (p))

#define UBS_SIGNAL_SENDTO_I(d, sig, i) \
  ubs_signal_sendto ((d), (sig), (i), 0, NULL)






typedef struct ubs_signal_st {
  struct ubs_signal_st *next;
  int                   dst;      
  int                   type;     
  long int              i_param;  
  unsigned long int     u_param;
  void                 *p_param;
} ubs_signal_t;




typedef void ubs_signal_function_t (ubs_signal_t *);









void
ubs_signal_init (void);




void
ubs_signal_terminate (void);




void
ubs_signal_process (void);





int
ubs_signal_queue_is_empty (void);




void
ubs_signal_delete (ubs_signal_t *sig);





void
ubs_signal_register_dst (int dst, ubs_signal_function_t *f);










void
ubs_signal_sendto (int dst,
                 int sig,
                 long int i_param,
                 unsigned long int u_param,
                 void *p_param);


#endif      /*UBS_SIG_H*/

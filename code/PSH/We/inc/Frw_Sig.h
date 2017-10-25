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













#ifndef _frw_sig_h
#define _frw_sig_h










#define FRW_SIG_DST_FRW_MAIN          0x010
#define FRW_SIG_DST_FRW_TIMER         0x020
#define FRW_SIG_DST_FRW_REGISTRY      0x030
#define FRW_SIG_DST_FRW_CMMN           0x040






#define FRW_SIGNAL_SENDTO(d, sig) \
  frw_signal_sendto ((d), (sig), 0, 0, 0, NULL)

#define FRW_SIGNAL_SENDTO_I(d, sig, i) \
  frw_signal_sendto ((d), (sig), i, 0, 0, NULL)

#define FRW_SIGNAL_SENDTO_U(d, sig, u) \
  frw_signal_sendto ((d), (sig), 0, (u), 0, NULL)

#define FRW_SIGNAL_SENDTO_IU(d, sig, i, u) \
  frw_signal_sendto ((d), (sig), (i), (u), 0, NULL)

#define FRW_SIGNAL_SENDTO_UU(d, sig, u1, u2) \
  frw_signal_sendto ((d), (sig), 0, (u1), (u2), NULL)

#define FRW_SIGNAL_SENDTO_IUU(d, sig, i, u1, u2) \
  frw_signal_sendto ((d), (sig), (i), (u1), (u2), NULL)

#define FRW_SIGNAL_SENDTO_P(d, sig, p) \
  frw_signal_sendto ((d), (sig), 0, 0, 0, (p))

#define FRW_SIGNAL_SENDTO_IP(d, sig, i, p) \
  frw_signal_sendto ((d), (sig), (i), 0, 0, (p))

#define FRW_SIGNAL_SENDTO_IUP(d, sig, i, u, p) \
  frw_signal_sendto ((d), (sig), (i), (u), 0, (p))

#define FRW_SIGNAL_SENDTO_IUUP(d, sig, i, u1, u2, p) \
  frw_signal_sendto ((d), (sig), (i), (u1), (u2), (p))






typedef struct frw_signal_st {
  struct frw_signal_st *next;
  int                 dst;      
  int                 type;     
  long int            i_param;  
  unsigned long int   u_param1;
  unsigned long int   u_param2;
  void               *p_param;
} frw_signal_t;




typedef void frw_signal_function_t (frw_signal_t *);









void
frw_signal_init (void);




void
frw_signal_terminate (void);





void
frw_signal_process (void);





int
frw_signal_queue_is_empty (void);




void
frw_signal_delete (frw_signal_t *sig);





void
frw_signal_register_dst (int dst, frw_signal_function_t *f);




void
frw_signal_deregister (int dst);




void
frw_signal_sendto (int dst,
                 int sig,
                 long int i_param,
                 unsigned long int u_param1,
                 unsigned long int u_param2,
                 void *p_param);




void
frw_signal_prepend (frw_signal_t *sig);

#endif

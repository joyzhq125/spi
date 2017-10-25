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








 





#ifndef _Pus_Sig_h
#define _Pus_Sig_h








#define PUS_MODULE_MAIN           0x000
#define PUS_MODULE_PARSER         0x020
#define PUS_MODULE_SIA            0x030
#define PUS_MODULE_HTTP           0x040
#define PUS_MODULE_MPRT           0x050
#define PUS_MODULE_MSG            0x060






#define PUS_SIGNAL_SENDTO_UP(d, sig, u, p) \
  Pus_Signal_sendto ((d), (sig), 0, 0, (u), (p))

#define PUS_SIGNAL_SENDTO_IU(d, sig, i, u) \
  Pus_Signal_sendto ((d), (sig), (i), 0, (u), NULL)

#define PUS_SIGNAL_SENDTO_IIU(d, sig, i1, i2, u) \
  Pus_Signal_sendto ((d), (sig), (i1), (i2), (u), NULL)

#define PUS_SIGNAL_SENDTO_P(d, sig, p) \
  Pus_Signal_sendto ((d), (sig), 0, 0, 0, (p))

#define PUS_SIGNAL_SENDTO_I(d, sig, i) \
  Pus_Signal_sendto ((d), (sig), (i), 0, 0, NULL)






typedef struct Pus_Signal_st {
  struct Pus_Signal_st *next;
  int                   dst;      
  int                   type;     
  long int              i_param1;  
  long int              i_param2;  
  unsigned long int     u_param;
  void                 *p_param;
} Pus_Signal_t;




typedef void Pus_Signal_function_t (Pus_Signal_t *);









void
Pus_Signal_init (void);




void
Pus_Signal_terminate (void);




void
Pus_Signal_process (void);





int
Pus_Signal_queue_is_empty (void);




void
Pus_Signal_delete (Pus_Signal_t *sig);





void
Pus_Signal_register_dst (int dst, Pus_Signal_function_t *f);










void
Pus_Signal_sendto (int dst,
                 int sig,
                 long int i_param1,
                 long int i_param2,
                 unsigned long int u_param,
                 void *p_param);


#endif

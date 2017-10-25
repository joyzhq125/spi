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

#ifndef SIS_ISIG_H
#define SIS_ISIG_H













typedef enum
{
    SIS_FSM_OPEN,       
    SIS_FSM_ASYNC,      
    SIS_FSM_MAX_REGS		
} SlsStateMachine;






#define SIS_SIGNAL_SENDTO(d, sig) \
  slsSignalSendTo ((d), (sig), 0, 0, 0, NULL)

#define SIS_SIGNAL_SENDTO_I(d, sig, i) \
  slsSignalSendTo ((d), (sig), i, 0, 0, NULL)

#define SIS_SIGNAL_SENDTO_U(d, sig, u) \
  slsSignalSendTo ((d), (sig), 0, (u), 0, NULL)

#define SIS_SIGNAL_SENDTO_IU(d, sig, i, u) \
  slsSignalSendTo ((d), (sig), (i), (u), 0, NULL)

#define SIS_SIGNAL_SENDTO_UU(d, sig, u1, u2) \
  slsSignalSendTo ((d), (sig), 0, (u1), (u2), NULL)

#define SIS_SIGNAL_SENDTO_IUU(d, sig, i, u1, u2) \
  slsSignalSendTo ((d), (sig), (i), (u1), (u2), NULL)

#define SIS_SIGNAL_SENDTO_P(d, sig, p) \
  slsSignalSendTo ((d), (sig), 0, 0, 0, (p))

#define SIS_SIGNAL_SENDTO_IP(d, sig, i, p) \
  slsSignalSendTo ((d), (sig), (i), 0, 0, (p))

#define SIS_SIGNAL_SENDTO_UP(d, sig, u, p) \
  slsSignalSendTo ((d), (sig), 0, (u), 0, (p))

#define SIS_SIGNAL_SENDTO_IUP(d, sig, i, u, p) \
  slsSignalSendTo ((d), (sig), (i), (u), 0, (p))

#define SIS_SIGNAL_SENDTO_UUP(d, sig, u1, u2, p) \
  slsSignalSendTo ((d), (sig), 0, (u1), (u2), (p))

#define SIS_SIGNAL_SENDTO_IUUP(d, sig, i, u1, u2, p) \
  slsSignalSendTo ((d), (sig), (i), (u1), (u2), (p))







typedef struct SlsSignalStruct {
  struct SlsSignalStruct *next;
  int                    dst;      
  int                    type;     
  long int               i_param;  
  unsigned long int      u_param1;
  unsigned long int      u_param2;
  void                  *p_param;
} SlsSignal;




typedef void SlsSignalFunction (SlsSignal *);









void slsSignalInit (void);




void slsSignalTerminate (void);





void slsSignalProcess(void);





int slsSignalQueueIsEmpty(void);




void slsSignalDelete (SlsSignal *sig);





void slsSignalRegisterDst (SlsStateMachine dst, SlsSignalFunction *f);




void slsSignalDeregister (SlsStateMachine dst);




SlsSignal *slsSignalSendTo(SlsStateMachine dst, 
                           int signum,
                           long i_param,
                           unsigned long u_param1,
                           unsigned long u_param2,
                           void *p_param);





void slsSignalPrepend(SlsSignal *sig);

#endif 

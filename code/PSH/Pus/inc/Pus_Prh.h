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








 
#ifndef _pus_prs_h
#define _pus_prs_h

#include "Pus_Cfg.h"
#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES

#ifndef _Pus_Sig_h
#include "Pus_Sig.h"
#endif








#define PUS_PRH_SIG_START_PARSING                    PUS_MODULE_PARSER + 1







#define PUS_PRH_SIG_PARSING_ABORTED                  PUS_MODULE_PARSER + 2
#define PUS_PRH_SIG_PARSING_COMPLETED                PUS_MODULE_PARSER + 3













#define PUS_PRH_SIG_PARSE_NEXT                       PUS_MODULE_PARSER + 4 











typedef struct 
{
  int                  instance_id;       
  int                  content_type_int;  
  int                  data_type;         
  unsigned char       *data;              

  WE_INT32            data_len;
  int                  charset;           
} pus_start_parsing_t;





void
Pus_Prh_init (void);

void
Pus_Prh_terminate (void);

void
Pus_Prh_main (Pus_Signal_t* signal);


#endif
#endif

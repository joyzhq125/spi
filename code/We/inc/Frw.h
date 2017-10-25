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










#ifndef _frw_h
#define _frw_h

#ifndef _we_lib_h
#include "We_Lib.h"
#endif

#ifndef _we_int_h
#include "We_Int.h"
#endif

#ifndef _we_core_h
#include "We_Core.h"
#endif





#define FRW_MAIN_SIG_START_MODULE         FRW_SIG_DST_FRW_MAIN + 1
#define FRW_MAIN_SIG_TERMINATE_MODULE     FRW_SIG_DST_FRW_MAIN + 2
#define FRW_MAIN_SIG_TERMINATE            FRW_SIG_DST_FRW_MAIN + 3
#define FRW_MAIN_SIG_MODULE_STATUS_NOTIFY FRW_SIG_DST_FRW_MAIN + 4
#define FRW_MAIN_SIG_MODULE_STATUS        FRW_SIG_DST_FRW_MAIN + 5
#define FRW_MAIN_SIG_REG_RESPONSE         FRW_SIG_DST_FRW_MAIN + 6
#define FRW_MAIN_SIG_CONTENT_SEND         FRW_SIG_DST_FRW_MAIN + 7
#define FRW_MAIN_SIG_AFI_FINISHED         FRW_SIG_DST_FRW_MAIN + 8
#define FRW_MAIN_SIG_SUB_MOD_INITIALISED  FRW_SIG_DST_FRW_MAIN + 9
#define FRW_MAIN_SIG_SUB_MOD_TERMINATED   FRW_SIG_DST_FRW_MAIN + 10







#define FRW_ALLOC(s)      WE_MEM_ALLOC(WE_MODID_FRW,s)
#define FRW_ALLOCTYPE(t)  WE_MEM_ALLOCTYPE(WE_MODID_FRW,t)
#define FRW_FREE(p)       WE_MEM_FREE(WE_MODID_FRW,p)






extern jmp_buf frw_jmpbuf;      
extern int     frw_inside_run;  










void
frw_start (const char* pcOptions, const int staticModules[], int iNumberOfStaticModules);







void
frw_sub_module_init_delay (void);







void
frw_sub_module_terminate_delay (void);





int
frw_modid_str2int (const char* pcStringModId);





char*
frw_get_sub_path (char* pcSrc, char* pcDst);





int
frw_get_module_status (WE_UINT8 uiModId);








void
frw_object_action (const we_act_content_t* pstContent,
                   const char*              pcExclActStrings[],
                   WE_INT32                iExclActStringsCnt,
                   WE_UINT8                uiAckDestModid,
                   WE_UINT8                uiAckId);










void
frw_file_notify (int file);







void
frw_register_file_handle (int file, int iRespDst, int iRespSig);




void
frw_deregister_file_handle (int file);

void
frw_start_module_internal (WE_UINT8 uiModid, int iRespDst, int iRespSig);






void
frw_register_mod_term_notif (WE_UINT8 modid, int iRespDst, int iRespSig);








int
frw_handle_package_signal(WE_UINT16 signal, void *p);


#endif

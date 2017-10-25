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










#ifndef _Pus_Cmmn_h
#define _Pus_Cmmn_h

#include "Pus_Main.h"




void
Pus_Cmmn_widget_action (we_widget_action_t *p); 





void
Pus_Cmmn_get_content_type (char *headers, WE_UINT32 *content_type_int);

int
Pus_Cmmn_read_data_from_stream_to_buf (int stream_handle, Pus_Msg_list_t* msg);

void
Pus_Cmmn_build_buf_from_data_buf_list (Pus_Msg_list_t* msg);




int
pus_disp_start_whitelist_dialog (int mod_id, char* info);

void
pus_register_dialog (int pus_module, Pus_Dlg_t *dlg);

int
pus_deregister_dialog (Pus_Dlg_t *dlg);




const char*
Pus_Main_get_key_as_string (int key);
 
const char*
Pus_Main_get_path_as_string (int key);

int
Pus_Main_get_path_as_int (const char* path);

int
Pus_Main_get_key_as_int (const char* key);




Pus_Msg_list_t*
pus_find_msg_by_id (int wid);

void
pus_delete_msg_from_list (int wid);

#endif

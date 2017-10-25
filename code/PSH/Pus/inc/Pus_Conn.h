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









#ifndef _Pus_Conn_h
#define _Pus_Conn_h

#ifndef _we_core_h
#include "we_core.h"
#endif

#ifndef _stk_if_h
#include "stk_if.h"
#endif

#ifndef _Pus_If_h
#include "Pus_If.h"
#endif

#ifndef _Pus_Main_h
#include "Pus_Main.h"
#endif





void
Pus_Conn_handle_open_connection (WE_UINT8 module_id, pus_open_push_connection_t *p);

void
Pus_Conn_handle_close_connection (WE_UINT8 module_id, WE_UINT16 handle);

void
Pus_Conn_handle_connection_created (stk_connection_created_t *p);

void
Pus_Conn_handle_connection_closed (stk_connection_closed_t *p);

Pus_Connection_t*
pus_find_server_connection_by_connection_id (int connection_id);

int 
Pus_Conn_get_connection_type_by_connection_id (int connection_id);

int 
Pus_Conn_get_network_acc_id_type_by_connection_id (int connection_id);

unsigned char*
Pus_Conn_get_cpi_tag_by_connection_id (int connection_id);

#endif

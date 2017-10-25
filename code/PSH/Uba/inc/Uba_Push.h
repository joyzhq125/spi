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
/*
 * uba_push.h
 * 
 * Description:
 *    This file contains code to handle Push messages(s)
 *
 *
 * Created by Martin Andersson, 2003/09/16.
 *
 * Revision history:
 *
 */
#ifndef UBA_PUSH_H
#define UBA_PUSH_H

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef _we_int_h
  #include "We_Int.h"
#endif

#ifndef _we_core_h
  #include "We_Core.h"
#endif

#ifndef UBA_DATA_H
  #include "Uba_Data.h"
#endif

#ifndef UBA_PUSH_H
  #include "Uba_Push.h"
#endif

#ifndef UBA_CUST_H
  #include "Uba_Cust.h"
#endif

/************************************************************************/
/* Functions                                                            */
/************************************************************************/

/*
 * Initiates View of a push message
 *
 * PARAMETERS
 *
 * RETURN    : TRUE if ok, false if data data dont point to a push msg or widget error
 */
int
uba_push_view_init (uba_custom_page_t *p, 
                    uba_data_obj_t    *data,
                    WeScreenHandle    screen,
                    int                i);

void 
uba_push_view_release(uba_custom_page_t *p);

int 
uba_push_view_handle_sig (uba_custom_page_t   *p, 
                          uba_signal_t                *sig);

void 
uba_push_view_handle_data_event (uba_custom_page_t    *p, 
                                 const uba_data_obj_t *data,
                                 uba_data_event_t      event);

void 
uba_push_view_show (uba_custom_page_t* p);


/*
 * This function is called in response to a request to get the message
 * form the UBS module. The received message is either displayed or 
 * the browser is launched with the specified url.
 *
 * PARAMETERS
 *
 * p         : Void pointer to message data struct (ubs_reply_full_msg_t)
 *
 * RETURN    : void
 */
void
uba_push_message_data (void *p);


#endif      /*UBA_PUSH_H*/

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
 * uba_cust.h
 *
 * Created by Martin Andersson, 2003/09/11.
 *
 * Revision history:
 *
 */
/*
 *  Defines types and functions for a custom page
 */

#ifndef UBA_CUST_H
#define UBA_CUST_H

/************************************************************************/
/* Includes                                                             */
/************************************************************************/
#ifndef uba_data_h
  #include "Uba_Data.h"
#endif

#ifndef we_core_h
  #include "We_Core.h"
#endif

#ifndef uba_data_h
  #include "Uba_Data.h"
#endif

#ifndef _we_int_h
  #include "We_Int.h"
#endif
 
#ifndef UBA_SIG_H
  #include "Uba_Sig.h"
#endif

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/

/* Forward declarations */
struct uba_custom_page_func_st;

/* Data for one custom page */
typedef struct uba_custom_page_st{
  const struct uba_custom_page_func_st  *operation; /*pointer to struct with function pointers */
  void                                  *usr_data;  /*internal data managed by the custom page */
  int                                   i;
}uba_custom_page_t;

/*
 *  Function def for custom pages
 */
/* Called to initiate the page */
typedef int  uba_custom_page_init_func_t(uba_custom_page_t *p, 
                                         uba_data_obj_t    *data,
                                         WeScreenHandle    screen,
                                         int                i);
/* Called tell the custom page to release all allocated resources*/
typedef void uba_custom_page_release_func_t(uba_custom_page_t *p);

/* Called for every signal, return true if signal used */
typedef int uba_custom_page_handle_sig_func_t(uba_custom_page_t* p, uba_signal_t *sig);

/* Called to notify cp of a data event */
typedef void uba_custom_page_handle_data_event_func_t(uba_custom_page_t    *p, 
                                               const uba_data_obj_t *data,
                                               uba_data_event_t      event);
/* Called to make cp visible and in focus */
typedef void uba_custom_page_show_func_t (uba_custom_page_t* p);


/* Declarations of functions pointers for one class of function pointers */
typedef struct uba_custom_page_func_st{
  uba_custom_page_init_func_t                 *init;
  uba_custom_page_release_func_t              *release;
  uba_custom_page_handle_sig_func_t           *handle_signal;
  uba_custom_page_handle_data_event_func_t    *data_event;
  uba_custom_page_show_func_t                 *show;
}uba_custom_page_func_t;

/************************************************************************/
/* Functions                                                            */
/************************************************************************/

void
uba_custom_page_init (uba_custom_page_t* p , int custom_page_id, 
                      uba_data_obj_t* data, WeScreenHandle screen, int i);



#endif      /*UBA_CUST_H*/


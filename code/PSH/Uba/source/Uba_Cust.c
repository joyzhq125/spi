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
 * uba_cust.c
 *
 * Created by Martin Andersson, 2003/09/11.
 *
 * Revision history:
 *
 */


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "Uba_Cust.h"

/************************************************************************/
/* Static variables                                                     */
/************************************************************************/

extern const uba_custom_page_func_t * uba_custom_pages[];

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void
uba_custom_page_init (uba_custom_page_t* p , int custom_page_id, 
                      uba_data_obj_t* data, WeScreenHandle screen, int i)
{
  p->operation = uba_custom_pages[custom_page_id];
  p->usr_data = NULL;
  p->operation->init (p, data, screen, i);
}




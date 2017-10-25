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









#ifndef UBS_KEYV_H
#define UBS_KEYV_H 




#ifndef _we_def_h
#include "We_Def.h"
#endif 

#ifndef UBS_MAIN_H
#include "Ubs_Main.h"
#endif





typedef struct{
  WE_UINT16        propertyArraySize;
  ubs_key_value_t  *propertyArray;
} ubs_key_value_array_t;





void
ubs_key_value_array_init (ubs_key_value_array_t* p);




void 
ubs_key_value_array_release (ubs_key_value_array_t* p);






void
ubs_key_value_update (ubs_key_value_array_t* p, ubs_key_value_array_t* change);




int
ubs_cvt_key_value (we_dcvt_t *obj, ubs_key_value_t *p);




int
ubs_cvt_key_value_array (ubs_key_value_array_t *p, we_dcvt_t *obj);





int
ubs_key_value_array_is_subset (ubs_key_value_array_t* p, ubs_key_value_array_t* subset);






void
ubs_key_value_array_get_subset (ubs_key_value_array_t* p, 
                                int keyListSize, const unsigned *keyList, 
                                ubs_key_value_array_t *result);


#endif      /*UBS_KEYV_H*/

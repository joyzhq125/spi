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









#include "Ubs_Keyv.h"






#define SWAP_KEY(a,b)             \
  {                               \
    ubs_key_value_t tmp = (a);    \
    (a) = (b);                    \
    (b) = tmp;                    \
  }








static ubs_key_value_t*
ubs_key_value_array_find_key(const ubs_key_value_array_t* p, unsigned  key);

static int
ubs_key_value_equal(const ubs_key_value_t* a, const ubs_key_value_t* b);






void
ubs_key_value_array_init(ubs_key_value_array_t* p)
{
  p->propertyArraySize = 0;
  p->propertyArray = NULL;
}

void 
ubs_key_value_array_release(ubs_key_value_array_t* p)
{
  ubs_key_value_t* it  = p->propertyArray;
  ubs_key_value_t* end = it + p->propertyArraySize;
  
  for (;it != end; ++it){
    UBS_MEM_FREE (it->value);
  }
  
  UBS_MEM_FREE (p->propertyArray);
  p->propertyArray = NULL;
  p->propertyArraySize = 0;
}




void
ubs_key_value_update(ubs_key_value_array_t* p, ubs_key_value_array_t* change)
{
  





  
  ubs_key_value_t* c_it  = change->propertyArray;
  
  ubs_key_value_t* c_end = c_it + change->propertyArraySize;
  

  ubs_key_value_t* c_new = c_it;
  
  ubs_key_value_t* p_it;
  ubs_key_value_t* p_end = p->propertyArray + p->propertyArraySize;
  int new_count;
    
  
  if (p->propertyArraySize == 0)
  {
    *p = *change;
    change->propertyArray = NULL;
    change->propertyArraySize = 0;
    return;
  }
  
  



  for( p_it  = p->propertyArray; c_it != c_end ;++c_it)
  {
     
     
     for(p_it = p->propertyArray; (p_it != p_end) && (p_it->key != c_it->key); ++p_it){
       
     }
     
     if (p_it != p_end){
       
       SWAP_KEY (*p_it, *c_it);
     }
     else{
       
       
       SWAP_KEY(*c_new,*c_it);
       ++c_new; 
     }

  }

  
  
  
  c_it = change->propertyArray;
  new_count = c_new - c_it;
  
  if (new_count)
  {
    int tmp_arr_size = p->propertyArraySize + new_count;
    void* tmp_arr   = UBS_MEM_ALLOC (sizeof (ubs_key_value_t) * tmp_arr_size);
    memcpy (tmp_arr, 
            p->propertyArray, 
            sizeof (ubs_key_value_t) * p->propertyArraySize);
    UBS_MEM_FREE(p->propertyArray);
    p->propertyArray = tmp_arr;
    p_it = p->propertyArray + p->propertyArraySize; 
    


    memset(p_it,0,new_count * sizeof(ubs_key_value_t)); 
    p->propertyArraySize = (WE_UINT16)tmp_arr_size; 
    for (c_it = change->propertyArray; c_it != c_new; ++c_it, ++p_it){
      SWAP_KEY(*c_it,*p_it);
    }
  }
}


int
ubs_cvt_key_value_array(ubs_key_value_array_t *p, we_dcvt_t *obj)
{
  return we_dcvt_uint16 (obj, &(p->propertyArraySize)) &&
         we_dcvt_array (obj, 
                         sizeof (ubs_key_value_t), 
                         p->propertyArraySize, 
                         (void **)&(p->propertyArray), 
                         (we_dcvt_element_t *)ubs_cvt_key_value);
}





int
ubs_key_value_array_is_subset (ubs_key_value_array_t* p, ubs_key_value_array_t* subset)
{
  int i;
  ubs_key_value_t* a;

  if (!subset){
    return TRUE;
  }

  if (subset->propertyArraySize > p->propertyArraySize)
    return FALSE;

  for(i = 0; i < subset->propertyArraySize; ++i)
  {
    a = ubs_key_value_array_find_key(p,subset->propertyArray[i].key);
    if (!a || !ubs_key_value_equal (a, &subset->propertyArray[i]))
    {
      return FALSE;
    }
  }
  return TRUE;
}





void
ubs_key_value_array_get_subset (ubs_key_value_array_t* p, 
                                int keyListSize, const unsigned *keyList, 
                                ubs_key_value_array_t *result)
{
  ubs_key_value_t* insert_it= result->propertyArray;
  ubs_key_value_t* tmp;
  const unsigned *keyListEnd=keyList+keyListSize;
  
  for(; keyList != keyListEnd; ++keyList)
  {
    tmp = ubs_key_value_array_find_key (p,*keyList);
    if (tmp)
    {
      *insert_it = *tmp;
      ++insert_it;
    }
  }
  result->propertyArraySize= (WE_UINT16) (insert_it - result->propertyArray);
  
}






static ubs_key_value_t*
ubs_key_value_array_find_key(const ubs_key_value_array_t* p, unsigned  key)
{
  int i;
  for(i = 0; i< p->propertyArraySize; ++i)
  {
    if (p->propertyArray[i].key == key)
      return &p->propertyArray[i];
  }
  return NULL;
}




static int
ubs_key_value_equal(const ubs_key_value_t* a, const ubs_key_value_t* b)
{
  return (a->key == b->key) &&
         (a->valueLen == b->valueLen) &&
         (a->valueType == b->valueType) &&
         memcmp(a->value, b->value, a->valueLen) == 0;
}





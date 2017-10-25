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











 
#include "Ubs_Hash.h"
#include "We_Mem.h"





#define UBS_HASH_MODID(h) (h->buckets->mod_id)




void
ubs_hash_init (ubs_hash_t* set, 
                   WE_UINT8 mod_id, 
                   unsigned int bucket_count,
                   ubs_hash_func_t *hash,
                   ubs_hash_func_compare_t *compare)
{
  unsigned int i;
  set->bucket_count = bucket_count;
  set->buckets = WE_MEM_ALLOC (mod_id, bucket_count * sizeof (ubs_list_t));
  set->hash = hash;
  set->compare = compare;
  set->size = 0;

  for (i = 0; i < bucket_count; ++i) {
    ubs_list_init (&set->buckets [i], mod_id);
  }
}

void
ubs_hash_release (ubs_hash_t* set)
{
  unsigned int i;
  for ( i = 0; i < set->bucket_count; ++i) {
    ubs_list_release (&set->buckets[i]);
  }

  WE_MEM_FREE (UBS_HASH_MODID(set), set->buckets);
  set->buckets = NULL;
  set->bucket_count = 0;
}


int
ubs_hash_insert (ubs_hash_t* set, void *data)
{
  unsigned int bucket = ((unsigned)set->hash ( data )) % set->bucket_count;
  ubs_list_t *list = &set->buckets[bucket];
  
  ubs_list_iterator_t end = ubs_list_end (list);
  ubs_list_iterator_t it = ubs_list_find (ubs_list_begin (list), 
                                          end, 
                                          data, 
                                          set->compare);
  if (UBS_LIST_ITERATOR_NOT_EQUAL(it,end)) {
    
    return FALSE;
  }

  ++set->size;
  ubs_list_push_back(list,data);
  return TRUE;
}

ubs_hash_iterator_t
ubs_hash_find (ubs_hash_t* set, void* data)
{
  unsigned int bucket = ((unsigned int)set->hash (data)) % set->bucket_count;
  ubs_list_t *list = &set->buckets[bucket];
  
  ubs_hash_iterator_t it;
  ubs_list_iterator_t end = ubs_list_end (list);
  it.it = ubs_list_find (ubs_list_begin (list), 
                                          end, 
                                          data, 
                                          set->compare);


  if (UBS_LIST_ITERATOR_EQUAL(it.it,end)) {
    
    return ubs_hash_end (set);
  }
  
  
  it.set = set;
  return it;  
}


ubs_hash_iterator_t
ubs_hash_begin (ubs_hash_t* set)
{
  unsigned int i;
  ubs_hash_iterator_t  it;
  ubs_list_t          *list = NULL ;
  
  it.set = set;
  
  for ( i = 0; i < set->bucket_count; ++i) {
    list = &set->buckets[i];
    if (UBS_LIST_SIZE(list)) {
      UBS_LIST_BEGIN(list,it.it);
      return it;
    }
  }
  
  UBS_LIST_END(list, it.it);
  return it;
}

ubs_hash_iterator_t
ubs_hash_end (ubs_hash_t* set)
{
  ubs_hash_iterator_t it;
  UBS_HASH_END (set,it);
  return it;
}


void
ubs_hash_iterator_next(ubs_hash_iterator_t* it)
{
  ubs_list_iterator_t list_end;
  
  UBS_LIST_ITERATOR_NEXT(it->it);
  UBS_LIST_END(it->it.list, list_end);
  
  if( UBS_LIST_ITERATOR_EQUAL( list_end, it->it) )
  {
    ubs_list_t* list= it->it.list;
    ubs_hash_iterator_t end;
    UBS_HASH_END(it->set,end);
    
    
    while ( UBS_LIST_ITERATOR_EQUAL (it->it, list_end ) &&
           !UBS_HASH_ITERATOR_EQUAL(*it,end))
    {
      ++list; 
      UBS_LIST_BEGIN(list,it->it);
      UBS_LIST_END(it->it.list, list_end); 
    }
  }
}





ubs_hash_iterator_t
ubs_hash_erase(ubs_hash_iterator_t it)
{
  ubs_hash_iterator_t res = it;
  UBS_HASH_ITERATOR_NEXT (res);
  ubs_list_erase(it.it);
  
  --it.set->size;
  
  return res;
}





int
ubs_hash_erase_data( ubs_hash_t *set, void* data)
{
  ubs_hash_iterator_t it = ubs_hash_find(set, data);
  ubs_hash_iterator_t end;
  UBS_HASH_END(set,end);
  
  if (UBS_HASH_ITERATOR_EQUAL (it, end)) {
    return FALSE;
  }
  
  ubs_list_erase(it.it);
  --set->size;
  
  return TRUE;
}


ubs_hash_iterator_t
ubs_hash_iterator_at (ubs_hash_t* set, int index)
{
  ubs_list_t* it = set->buckets;
  ubs_list_t* end = it + set->bucket_count;
  ubs_hash_iterator_t hit;

  for(;(it != end) && (index >= UBS_LIST_SIZE(it)) ;++it)
  {
    index-=UBS_LIST_SIZE(it);
  }

  if (it == end){
    UBS_HASH_END(set,hit);
  }else{
    hit.set = set;
    hit.it = ubs_list_iterator_at (it, index);
  }

  return hit;

}



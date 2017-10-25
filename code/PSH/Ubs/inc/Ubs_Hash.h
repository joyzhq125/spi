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









#ifndef UBS_HASH_H
#define UBS_HASH_H

#ifndef UBS_LIST_H
#include "Ubs_List.h"
#endif






#define UBS_HASH_ITERATOR_GET(i)  \
  ((i).it.current->value)
#define UBS_HASH_ITERATOR_GET_TYPE(i,type) \
  ((type*)((i).it.current->value))




#define UBS_HASH_ITERATOR_NEXT(it) ubs_hash_iterator_next(&(it))


#define UBS_HASH_ITERATOR_EQUAL(i,ii)       \
  (((i).set        == (ii).set)         &&  \
   ((i).it.list    == (ii).it.list)     &&  \
   ((i).it.current == (ii).it.current))

#define UBS_HASH_ITERATOR_NOT_EQUAL(i,ii)     \
  (!(((i).set        == (ii).set)         &&  \
   ((i).it.list    == (ii).it.list)       &&  \
   ((i).it.current == (ii).it.current)))



#define UBS_HASH_SIZE(set) ((set)->size)

#define UBS_HASH_END(hash, end_it)                                         \
  ((end_it).set = (hash),                                                  \
  (end_it).it = ubs_list_end( &(hash)->buckets[(hash)->bucket_count-1] ))







typedef int ubs_hash_func_t(void* data);




typedef ubs_list_func_compare_t ubs_hash_func_compare_t;




typedef struct {
  unsigned int             bucket_count;
  ubs_list_t              *buckets;
  ubs_hash_func_t         *hash;
  ubs_hash_func_compare_t *compare;
  int                      size;
} ubs_hash_t;





typedef struct {
  ubs_list_iterator_t it;
  ubs_hash_t          *set;
} ubs_hash_iterator_t;













void
ubs_hash_init (ubs_hash_t              *set, 
               WE_UINT8                mod_id,                
               unsigned int             bucket_count,
               ubs_hash_func_t         *hash,
               ubs_hash_func_compare_t *compare);





void
ubs_hash_release (ubs_hash_t* set);




int
ubs_hash_insert (ubs_hash_t* set, void* data);





ubs_hash_iterator_t
ubs_hash_find (ubs_hash_t* set, void* data);





ubs_hash_iterator_t
ubs_hash_erase(ubs_hash_iterator_t it);





int
ubs_hash_erase_data( ubs_hash_t *set, void* data);

ubs_hash_iterator_t
ubs_hash_begin (ubs_hash_t* set);



ubs_hash_iterator_t
ubs_hash_end (ubs_hash_t* set);

ubs_hash_iterator_t
ubs_hash_iterator_at (ubs_hash_t* set, int index);




void
ubs_hash_iterator_next (ubs_hash_iterator_t* it);




#endif      /*UBS_HASH_H*/

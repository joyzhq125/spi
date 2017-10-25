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





















#ifndef UBS_LIST_H
#define UBS_LIST_H




#ifndef _we_def_h
#include "We_Def.h"
#endif 










typedef int ubs_list_func_compare_t(void* key_a, void* key_b);


typedef struct ubs_list_element_st{
  struct ubs_list_element_st *prev; 
  struct ubs_list_element_st *next;
  void                       *value;
} ubs_list_element_t;



typedef struct {
  ubs_list_element_t  head;
  int                 size;
  WE_UINT8           mod_id;
}ubs_list_t;

typedef struct {
  ubs_list_element_t *current;
  ubs_list_t         *list;
}ubs_list_iterator_t;


typedef ubs_list_t          ubs_queue_t;





#define UBS_LIST_ITERATOR_EQUAL(a,b) \
  ((a.current == b.current) && (a.list == b.list))

#define UBS_LIST_ITERATOR_NOT_EQUAL(a,b) \
  ((a.current != b.current) || (a.list != b.list))

#define UBS_LIST_ITERATOR_GET(it) (it.current->value)
#define UBS_LIST_ITERATOR_GET_TYPE(it,type) ((type*) (it.current->value))
#define UBS_LIST_ITERATOR_NEXT(it) \
  ((it).current = (it).current->next)
#define UBS_LIST_ITERATOR_PREV(it) \
  ((it).current = (it).current->prev)




#define UBS_LIST_SIZE(list) \
  ((list)->size)

#define UBS_LIST_EMPTY(list) \
  (UBS_LIST_SIZE  (list) <= 0 )

#define UBS_LIST_END(l,i)    \
  ((i).current = &(l)->head, \
  (i).list = (l))

#define UBS_LIST_BEGIN(l,it)        \
  ((it).current = (l)->head.next,   \
  (it).list = (l))




#define UBS_LIST_FRONT(list)   \
  ((list)->head.next->value)






#define UBS_LIST_FOR_EACH(list, type, func)                              \
{                                                                        \
  ubs_list_iterator_t it ;                                               \
  ubs_list_iterator_t end;                                               \
  UBS_LIST_BEGIN(list,it);                                               \
  UBS_LIST_END(list,end);                                                \
  for ( ; UBS_LIST_ITERATOR_NOT_EQUAL(it,end); UBS_LIST_ITERATOR_NEXT(it)\
  {                                                                      \
    func((type*)it.current->value);                                      \
  }                                                                      \
}




#define ubs_list_declare (TYPE) \
  TYPE ubs_list_get_##TYPE (ubs_list_iterator_t it){   \
    return (TYPE*)it->current->value;                  \
}                                                      \












#define UBS_QUEUE_DECLARE(type,prefix)                \
void                                                  \
prefix##init(ubs_queue_t* p,                          \
               WE_UINT8 mod_id)                      \
{                                                     \
  ubs_list_init(p,mod_id);                            \
}                                                     \
int                                                   \
prefix##empty(ubs_queue_t* que)                       \
{                                                     \
  return UBS_LIST_SIZE(que)==0;                       \
}                                                     \
int                                                   \
prefix##size(ubs_queue_t* que)                        \
{                                                     \
  return UBS_LIST_SIZE(que);                          \
}                                                     \
type*                                                 \
prefix##top(ubs_queue_t* que)                         \
{                                                     \
  return (type*)ubs_list_front(que);                  \
}                                                     \
void                                                  \
prefix##push(ubs_queue_t* que, type *p)               \
{                                                     \
  ubs_list_push_back(que,p);                          \
}                                                     \
void                                                  \
prefix##pop(ubs_queue_t* que)                         \
{                                                     \
  ubs_list_pop_front(que);                            \
}










void 
ubs_list_init (ubs_list_t* list, 
               WE_UINT8 mod_id);






void
ubs_list_release (ubs_list_t* list);





ubs_list_iterator_t
ubs_list_begin(ubs_list_t* list);





ubs_list_iterator_t
ubs_list_end(ubs_list_t* list);





ubs_list_iterator_t
ubs_list_erase(ubs_list_iterator_t it);





ubs_list_iterator_t
ubs_list_erase_sequence(ubs_list_iterator_t first, ubs_list_iterator_t last);




void 
ubs_list_pop_front (ubs_list_t* list);




void 
ubs_list_pop_back (ubs_list_t *list);




ubs_list_iterator_t
ubs_list_insert (ubs_list_iterator_t it, void* data);




void 
ubs_list_push_front (ubs_list_t* list, void* data);




void 
ubs_list_push_back (ubs_list_t* list, void* data );





void* 
ubs_list_back (ubs_list_t* list);







ubs_list_iterator_t
ubs_list_find (ubs_list_iterator_t begin, 
               ubs_list_iterator_t end, 
               void* value,
               ubs_list_func_compare_t* compare);


ubs_list_iterator_t
ubs_list_iterator_at( ubs_list_t* list, int index);

#endif      /*UBS_LIST_H*/

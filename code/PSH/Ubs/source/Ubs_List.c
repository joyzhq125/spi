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











#include "Ubs_List.h"
#include "We_Mem.h"






void
ubs_list_init (ubs_list_t* list, 
               WE_UINT8 mod_id)
{
  list->head.value = NULL;
  list->head.next = &list->head;
  list->head.prev = &list->head;
  list->size = 0;
  list->mod_id = mod_id;


}

void
ubs_list_release (ubs_list_t* list)
{
  ubs_list_erase_sequence (ubs_list_begin (list), ubs_list_end (list));
}

ubs_list_iterator_t
ubs_list_begin(ubs_list_t* list)
{
  ubs_list_iterator_t it;
  UBS_LIST_BEGIN(list,it);
  return  it;
}


ubs_list_iterator_t
ubs_list_end(ubs_list_t* list)
{
  ubs_list_iterator_t it;
  it.current = &(list->head);
  it.list = list;  
  return  it;
}

ubs_list_iterator_t
ubs_list_erase(ubs_list_iterator_t it)
{
  

  ubs_list_element_t* prev = it.current->prev;
  ubs_list_element_t* next = it.current->next;

  prev->next = next;
  next->prev = prev;

  WE_MEM_FREE (it.list->mod_id, it.current);
  
  it.current=next;

  --it.list->size;
  return it;
}



ubs_list_iterator_t
ubs_list_erase_sequence(ubs_list_iterator_t first, ubs_list_iterator_t last)
{
  while(!UBS_LIST_ITERATOR_EQUAL(first,last)){
    first = ubs_list_erase (first);
  }
  return first;
}

void 
ubs_list_pop_front (ubs_list_t* list)
{
  ubs_list_erase (ubs_list_begin (list));
}

ubs_list_iterator_t
ubs_list_back_iterator (ubs_list_t* list)
{
  ubs_list_iterator_t it;
  UBS_LIST_END(list,it);  
  UBS_LIST_ITERATOR_PREV (it);
  return it;
}
  

void 
ubs_list_pop_back (ubs_list_t* list){
  ubs_list_erase (ubs_list_back_iterator (list));
}

ubs_list_iterator_t
ubs_list_insert (ubs_list_iterator_t it, void* data)
{
  ubs_list_element_t* element = 
    WE_MEM_ALLOCTYPE (it.list->mod_id, ubs_list_element_t);

  
  element->value = data;
  
  
  element->prev = it.current->prev;
  element->prev->next = element;
  
  
  element->next       = it.current;
  element->next->prev = element ;

  
  ++(it.list->size);

  
  it.current= element;

  return it;
}

void 
ubs_list_push_front (ubs_list_t* list, void* data)
{
  ubs_list_insert (ubs_list_begin (list), data);
}

void 
ubs_list_push_back( ubs_list_t* list, void* data )
{
  ubs_list_insert( ubs_list_end(list), data);
}


void* 
ubs_list_back (ubs_list_t* list)
{
  ubs_list_iterator_t it = ubs_list_end(list);
  UBS_LIST_ITERATOR_PREV(it);
  return UBS_LIST_ITERATOR_GET(it);
}



ubs_list_iterator_t
ubs_list_find (ubs_list_iterator_t begin, 
               ubs_list_iterator_t end, 
               void* value,
               ubs_list_func_compare_t* compare)
{
  while (UBS_LIST_ITERATOR_NOT_EQUAL (begin, end) && 
        (!compare (UBS_LIST_ITERATOR_GET (begin), value))) {
    UBS_LIST_ITERATOR_NEXT (begin);
  }
  return begin;
}



ubs_list_iterator_t
ubs_list_iterator_at( ubs_list_t* list, int index)
{
  ubs_list_iterator_t it;
  ubs_list_iterator_t end;
  int i = 0;
  
  UBS_LIST_BEGIN(list,it);
  UBS_LIST_END(list,end);
  
  for (;
        (i < index) && UBS_LIST_ITERATOR_NOT_EQUAL (it,end);
        ++i, UBS_LIST_ITERATOR_NEXT(it))
  {
    
  }
  return it;
}







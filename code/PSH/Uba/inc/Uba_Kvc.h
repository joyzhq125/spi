
#ifndef UBA_KVC_H
#define UBA_KVC_H

#ifndef _we_def_h
  #include "We_Def.h"
#endif



/* 
 * If the integrator needs more than 16 key value criterias  the itegrator MUST change 
 * the type of the key value criteria (uba_page_criteria_t) to WE_UINT32,
 * NOTE. the integrator must also change the macro UBA_END_KVC_MASK
 */
typedef WE_UINT32    uba_page_criteria_t;


/* 
 * Markes the end of the uba_page_item_icon_menu_map, every byte should be 0xff and the 
 * same size as uba_page_criteria_t -> 
 *      uba_page_criteria_t == WE_UINT16 ->UBA_END_KVC_MASK = 0xffff
 *      uba_page_criteria_t == WE_UINT32 ->UBA_END_KVC_MASK = 0xffffffff
 */
#define UBA_END_KVC_MASK 0xffffffff


/* 
 * Define the max length of the value parameter in uba_page_item_key_value_criteria_t 
 */
#define UBA_KEY_VALUE_CRITERIA_MAX_LENGTH 2





#endif      /*UBA_KVC_H*/

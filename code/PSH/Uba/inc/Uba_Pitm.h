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
 * uba_pitm.h
 *
 * Created by Martin Andersson, 2003/08/25.
 *
 *
 * Handles one item on a page. A page item is responsible for the text and icon resource
 * to be shown in the widget menu.
 *
 *
 * Revision history:
 *
 */

#ifndef UBA_PITM_H
#define UBA_PITM_H

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef UBA_TEXT_H
  #include "Uba_Text.h"
#endif

#ifndef UBA_DATA_H
  #include "Uba_Data.h"
#endif

#ifndef UBA_RCE_H
#include "Uba_Rce.h"
#endif


#ifndef UBA_KVC_H
  #include "Uba_Kvc.h"
#endif

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/

/*
 *	The page item object
 */
typedef struct {
  uba_text_t                    text1;
  const uba_rce_page_item_t     *rc;
  uba_data_obj_t                data;
}uba_page_item_t;


 

/* Resource specified in uba_cfg.h. criteria_mask and msg_type is used for
 * finding the right struct from an array of structs, the other members are
 * data holders
 */
typedef struct  {
  const uba_page_criteria_t criteria_mask; /* Criteria mask see UBA_ALL_KEY_VALUE_CRITERIA*/
  const WE_UINT8           msg_type;      /* Message type */
  const int                 menu_id;       /* Id of the menu resource */
  const WE_UINT32          icon_id;       /* Id of the icon resource*/
  const int                 text_property_id;   /* the key of the property that is to 
                                                 be used as the text after the icon */
  const int                 text_property_id_2; /* the key of the property that is to 
                                                 be used if text_property_id is empty */
}uba_page_item_icon_menu_t;





/*
 *	A key value criteria.
 *  Must match the pattern in UBA_ALL_KEY_VALUE_CRITERIA
 *  
 */
typedef struct  {
  const WE_UINT16            key;        /* Value of the key */
  const WE_UINT8             valueType;  /* Type of the data */
  const WE_UINT16            valueLen;   /* actuall length of the date */
  const unsigned char         value[UBA_KEY_VALUE_CRITERIA_MAX_LENGTH];  /* Data */
}uba_page_item_key_value_criteria_t;





/* Marks the end of the UBA_ALL_KEY_VALUE_CRITERIA array, must be the same size as 
   uba_page_criteria_t and not an even power of 2 (not 0,1,2,4,8 ... )
*/
#define UBA_END_KEY      0xffff
#define UBA_END_TYPE     0xff
#define UBA_END_MSG_TYPE 0xff


/************************************************************************/
/* MACORS                                                               */
/************************************************************************/

/* Get text one widget handle for the a uba_page_item_icon_menu_t struct */
#define UBA_PAGE_ITEM_GET_TEXT1_HANDLE(p) UBA_TEXT_GET_HANDLE(&(p)->text1)
/* Get tha data bobject form a uba_page_item_icon_menu_t struct*/
#define UBA_PAGE_ITEM_GET_DATA(p) (&((p)->data))

#define UBA_PAGE_ITEM_GET_RC(p) (p->rc)

/************************************************************************/
/* Exported functions                                                    */
/************************************************************************/

/* 
 * Init the paget_item p with the data object data and the resource object rc.
 * This will allocate the widget resources needed for the page to create a
 * widget menu item 
 */
void
uba_page_item_init (uba_page_item_t *p, uba_data_obj_t* data, 
                    const uba_rce_page_item_t* rc);

/*
 *	Frees all resources and memmory owned by p
 */
void
uba_page_item_release (uba_page_item_t *p);

/*
 *	Returns the wid of the menu to be used on this item
 */
int
uba_page_item_get_menu_id (const uba_page_item_t *p);

/*
 *	Get the widget icon handle to be used on this item
 */
WeImageHandle
uba_page_item_get_icon( const uba_page_item_t *p );

/* Look up one RC item that defines the resources needed to display a msg */
const uba_page_item_icon_menu_t*
uba_page_item_get_icon_menu_msg(const uba_data_item_t* msg);

#endif      /*UBA_PITM_H*/

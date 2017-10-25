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
 * pus_hdef.h
 *
 * Created by Kent Olsson, Sun Oct 27 14:23:35 2001.
 *
 * Revision history:
 *
 */

#ifndef _Wbs_Hdef_h
#define _Wbs_Hdef_h

#include "we_cmmn.h"
/***********************************************
 *               SI Documents                  *
 ***********************************************/

/*****************  ELEMENTS  *****************/

#define PUS_ELEMENT_SI                   5
#define PUS_ELEMENT_INDICATION           6
#define PUS_ELEMENT_INFO                 7
#define PUS_ELEMENT_ITEM                 8


/****************  ATTRIBUTES  ****************/

#define PUS_ATTRIBUTE_ACTION             0
#define PUS_ATTRIBUTE_CREATED            1
#define PUS_ATTRIBUTE_HREF               2
#define PUS_ATTRIBUTE_SI_EXPIRES         3
#define PUS_ATTRIBUTE_SI_ID              4
#define PUS_ATTRIBUTE_CLASS              5


/*************  ATTRIBUTE VALUES  *************/

/* ENUM_A */
#define PUS_ATTRVAL_SIGNAL_NONE          0x0A00
#define PUS_ATTRVAL_SIGNAL_LOW           0x0A01
#define PUS_ATTRVAL_SIGNAL_MEDIUM        0x0A02
#define PUS_ATTRVAL_SIGNAL_HIGH          0x0A03
#define PUS_ATTRVAL_SIGNAL_DELETE        0x0A04


/**********************************************/

extern const we_strtable_info_t pus_SI_Elements;

extern const we_strtable_info_t pus_SI_Attributes;

extern const we_strtable_info_t pus_SI_Attrvals;


extern const unsigned char pus_SI_Element_table[];

extern const WE_UINT16    pus_SI_AttributeTypes[];


#define PUS_SI_TRANSFORM_SIZE 19

extern const WE_UINT8           pus_SI_AttrTransform[];

extern const we_strtable_info_t pus_SI_AttrStart;





/***********************************************
 *               SL Documents                  *
 ***********************************************/

/*****************  ELEMENTS  *****************/

#define PUS_ELEMENT_SL                   5


/****************  ATTRIBUTES  ****************/

/* The same as for SI is used */
/* 
#define PUS_ATTRIBUTE_ACTION             0
#define PUS_ATTRIBUTE_HREF               2
*/

/*************  ATTRIBUTE VALUES  *************/

/* ENUM_A */
#define PUS_ATTRVAL_EXECUTE_LOW          0x0A00
#define PUS_ATTRVAL_EXECUTE_HIGH         0x0A01
#define PUS_ATTRVAL_CACHE                0x0A02


/**********************************************/

extern const we_strtable_info_t pus_SL_Elements;

extern const we_strtable_info_t pus_SL_Attributes;

extern const we_strtable_info_t pus_SL_Attrvals;


extern const unsigned char pus_SL_Element_table[];

extern const WE_UINT16        pus_SL_AttributeTypes[];


#define PUS_SL_TRANSFORM_SIZE 13

extern const WE_UINT8           pus_SL_AttrTransform[];

extern const we_strtable_info_t pus_SL_AttrStart;





/***********************************************
 *               CO Documents                  *
 ***********************************************/

/*****************  ELEMENTS  *****************/

#define PUS_ELEMENT_CO                   5
#define PUS_ELEMENT_INVALIDATE_OBJECT    6
#define PUS_ELEMENT_INVALIDATE_SERVICE   7


/****************  ATTRIBUTES  ****************/

#define PUS_ATTRIBUTE_URI                0


/*************  ATTRIBUTE VALUES  *************/


/**********************************************/

extern const we_strtable_info_t pus_CO_Elements;

extern const we_strtable_info_t pus_CO_Attributes;


extern const unsigned char pus_CO_Element_table[];

extern const WE_UINT16        pus_CO_AttributeTypes[];


#define PUS_CO_TRANSFORM_SIZE 10

extern const WE_UINT8           pus_CO_AttrTransform[];

extern const we_strtable_info_t pus_CO_AttrStart;





/***********************************************
 *       COMMON FOR ALL PUSH DOCUMENTS         *
 ***********************************************/

extern const we_strtable_info_t pus_AttrValue;

#endif

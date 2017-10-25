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









#include "Pus_Cfg.h"
#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES

#include "Pus_Def.h"
#include "we_prsr.h"







static const we_strtable_entry_t pus_SI_Elements_stringTable[] = 
{
  {"si",                 PUS_ELEMENT_SI                },
  {"indication",         PUS_ELEMENT_INDICATION        },
  {"info",               PUS_ELEMENT_INFO              },
  {"item",               PUS_ELEMENT_ITEM              }
};

static const unsigned char pus_SI_Elements_hashTable[] = 
{
  255, 255, 1, 0, 2, 3, 255
};

const we_strtable_info_t pus_SI_Elements = 
{
  pus_SI_Elements_hashTable,
  sizeof pus_SI_Elements_hashTable,
  WE_TABLE_SEARCH_BINARY,
  pus_SI_Elements_stringTable,
  sizeof pus_SI_Elements_stringTable / sizeof pus_SI_Elements_stringTable[0]
};





static const we_strtable_entry_t pus_SI_Attributes_stringTable[] = 
{
  {"action",             PUS_ATTRIBUTE_ACTION          },
  {"created",            PUS_ATTRIBUTE_CREATED         },
  {"href",               PUS_ATTRIBUTE_HREF            },
  {"si-expires",         PUS_ATTRIBUTE_SI_EXPIRES      },
  {"si-wid",              PUS_ATTRIBUTE_SI_ID           },
  {"class",              PUS_ATTRIBUTE_CLASS           }
};

static const unsigned char pus_SI_Attributes_hashTable[] = 
{
  4, 255, 0, 3, 1, 255, 5, 255, 255, 2,
  255
};

const we_strtable_info_t pus_SI_Attributes = 
{
  pus_SI_Attributes_hashTable,
  sizeof pus_SI_Attributes_hashTable,
  WE_TABLE_SEARCH_LINEAR,
  pus_SI_Attributes_stringTable,
  sizeof pus_SI_Attributes_stringTable / sizeof pus_SI_Attributes_stringTable[0]
};






static const we_strtable_entry_t pus_SI_Attrvals_stringTable[] = 
{
  {"signal-none",        PUS_ATTRVAL_SIGNAL_NONE       },
  {"signal-low",         PUS_ATTRVAL_SIGNAL_LOW        },
  {"signal-medium",      PUS_ATTRVAL_SIGNAL_MEDIUM     },
  {"signal-high",        PUS_ATTRVAL_SIGNAL_HIGH       },
  {"signal-delete",      PUS_ATTRVAL_SIGNAL_DELETE     }
};

static const unsigned char pus_SI_Attrvals_hashTable[] = 
{
  1, 255, 0, 4, 2, 255, 3
};

const we_strtable_info_t pus_SI_Attrvals = 
{
  pus_SI_Attrvals_hashTable,
  sizeof pus_SI_Attrvals_hashTable,
  WE_TABLE_SEARCH_LINEAR,
  pus_SI_Attrvals_stringTable,
  sizeof pus_SI_Attrvals_stringTable / sizeof pus_SI_Attrvals_stringTable[0]
};





const unsigned char pus_SI_Element_table[] = 
{
  0,                                      
  0,                                      
  0,                                      
  0,                                      
  0,                                      
  WE_PRSR_CANCELSPACE,                   
  WE_PRSR_CANCELSPACE | WE_PRSR_TEXT,   
  WE_PRSR_CANCELSPACE,                   
  0                                       
};





const WE_UINT16 pus_SI_AttributeTypes[] = 
{
  WE_PRSR_TYPE_ENUM_A,   
  WE_PRSR_TYPE_CDATA,    
  WE_PRSR_TYPE_CDATA,    
  WE_PRSR_TYPE_CDATA,    
  WE_PRSR_TYPE_CDATA,    
  WE_PRSR_TYPE_CDATA         
};





const WE_UINT8 pus_SI_AttrTransform[] = 
{
  WE_PRSR_NOT_USED,
  WE_PRSR_NOT_USED,
  WE_PRSR_NOT_USED,
  WE_PRSR_NOT_USED,
  WE_PRSR_NOT_USED,                         
  PUS_ATTRIBUTE_ACTION      | WE_PRSR_DEC_DATA,       
  PUS_ATTRIBUTE_ACTION      | WE_PRSR_DEC_DATA,  
  PUS_ATTRIBUTE_ACTION      | WE_PRSR_DEC_DATA,  
  PUS_ATTRIBUTE_ACTION      | WE_PRSR_DEC_DATA,  
  PUS_ATTRIBUTE_ACTION      | WE_PRSR_DEC_DATA,  
  PUS_ATTRIBUTE_CREATED,                          
  PUS_ATTRIBUTE_HREF,                             
  PUS_ATTRIBUTE_HREF        | WE_PRSR_DEC_DATA,  
  PUS_ATTRIBUTE_HREF        | WE_PRSR_DEC_DATA,  
  PUS_ATTRIBUTE_HREF        | WE_PRSR_DEC_DATA,  
  PUS_ATTRIBUTE_HREF        | WE_PRSR_DEC_DATA,  
  PUS_ATTRIBUTE_SI_EXPIRES,                       
  PUS_ATTRIBUTE_SI_ID,                            
  PUS_ATTRIBUTE_CLASS                             
};





static const we_strtable_entry_t pus_SI_AttrStart_stringTable[] = 
{
  {"signal-none",        0x05}, 
  {"signal-low",         0x06}, 
  {"signal-medium",      0x07}, 
  {"signal-high",        0x08}, 
  {"signal-delete",      0x09}, 
  {"http://",            0x0C}, 
  {"http://www.",        0x0D}, 
  {"https://",           0x0E}, 
  {"https://www.",       0x0F}  
};
 
const we_strtable_info_t pus_SI_AttrStart = 
{
  NULL,
  0,
  WE_TABLE_SEARCH_BINARY,
  pus_SI_AttrStart_stringTable,
  sizeof pus_SI_AttrStart_stringTable / sizeof pus_SI_AttrStart_stringTable[0]
};











static const we_strtable_entry_t pus_SL_Elements_stringTable[] = {
  {"sl",                 PUS_ELEMENT_SL                }
};

static const unsigned char pus_SL_Elements_hashTable[] = {
  0
};

const we_strtable_info_t pus_SL_Elements = {
  pus_SL_Elements_hashTable,
  sizeof pus_SL_Elements_hashTable,
  WE_TABLE_SEARCH_LINEAR,
  pus_SL_Elements_stringTable,
  sizeof pus_SL_Elements_stringTable / sizeof pus_SL_Elements_stringTable[0]
};





static const we_strtable_entry_t pus_SL_Attributes_stringTable[] = {
  {"action",             PUS_ATTRIBUTE_ACTION          },
  {"href",               PUS_ATTRIBUTE_HREF            },
};

static const unsigned char pus_SL_Attributes_hashTable[] = {
  255, 1, 0
};

const we_strtable_info_t pus_SL_Attributes = {
  pus_SL_Attributes_hashTable,
  sizeof pus_SL_Attributes_hashTable,
  WE_TABLE_SEARCH_LINEAR,
  pus_SL_Attributes_stringTable,
  sizeof pus_SL_Attributes_stringTable / sizeof pus_SL_Attributes_stringTable[0]
};





static const we_strtable_entry_t pus_SL_Attrvals_stringTable[] = {
  {"execute-low",        PUS_ATTRVAL_EXECUTE_LOW       },
  {"execute-high",       PUS_ATTRVAL_EXECUTE_HIGH      },
  {"cache",              PUS_ATTRVAL_CACHE             }
};

static const unsigned char pus_SL_Attrvals_hashTable[] = {
  0, 2, 1, 255, 255
};

const we_strtable_info_t pus_SL_Attrvals = {
  pus_SL_Attrvals_hashTable,
  sizeof pus_SL_Attrvals_hashTable,
  WE_TABLE_SEARCH_LINEAR,
  pus_SL_Attrvals_stringTable,
  sizeof pus_SL_Attrvals_stringTable / sizeof pus_SL_Attrvals_stringTable[0]
};





const unsigned char pus_SL_Element_table[] = {
  0,                   
  0,                   
  0,                   
  0,                   
  0,                   
  WE_PRSR_CANCELSPACE 
};





const WE_UINT16 pus_SL_AttributeTypes[] = {
  WE_PRSR_TYPE_ENUM_A,     
  0,                        
  WE_PRSR_TYPE_CDATA       
};





const WE_UINT8 pus_SL_AttrTransform[] = {
  WE_PRSR_NOT_USED,
  WE_PRSR_NOT_USED,
  WE_PRSR_NOT_USED,
  WE_PRSR_NOT_USED,
  WE_PRSR_NOT_USED,                         
  PUS_ATTRIBUTE_ACTION  | WE_PRSR_DEC_DATA,       
  PUS_ATTRIBUTE_ACTION  | WE_PRSR_DEC_DATA,  
  PUS_ATTRIBUTE_ACTION  | WE_PRSR_DEC_DATA,  
  PUS_ATTRIBUTE_HREF,                         
  PUS_ATTRIBUTE_HREF    | WE_PRSR_DEC_DATA,  
  PUS_ATTRIBUTE_HREF    | WE_PRSR_DEC_DATA,  
  PUS_ATTRIBUTE_HREF    | WE_PRSR_DEC_DATA,  
  PUS_ATTRIBUTE_HREF    | WE_PRSR_DEC_DATA   
};





static const we_strtable_entry_t pus_SL_AttrStart_stringTable[] = {
  {"execute-low",        0x05}, 
  {"execute-high",       0x06}, 
  {"cache",              0x07}, 
  {"http://",            0x09}, 
  {"http://www.",        0x0A}, 
  {"https://",           0x0B}, 
  {"https://www.",       0x0C}  
};
 
const we_strtable_info_t pus_SL_AttrStart = {
  NULL,
  0,
  WE_TABLE_SEARCH_BINARY,
  pus_SL_AttrStart_stringTable,
  sizeof pus_SL_AttrStart_stringTable / sizeof pus_SL_AttrStart_stringTable[0]
};











static const we_strtable_entry_t pus_CO_Elements_stringTable[] = {
  {"co",                 PUS_ELEMENT_CO                },
  {"invalidate-object",  PUS_ELEMENT_INVALIDATE_OBJECT },
  {"invalidate-service", PUS_ELEMENT_INVALIDATE_SERVICE}
};

static const unsigned char pus_CO_Elements_hashTable[] = {
  0, 2, 255, 1, 255
};

const we_strtable_info_t pus_CO_Elements = {
  pus_CO_Elements_hashTable,
    sizeof pus_CO_Elements_hashTable,
  WE_TABLE_SEARCH_LINEAR,
  pus_CO_Elements_stringTable,
  sizeof pus_CO_Elements_stringTable / sizeof pus_CO_Elements_stringTable[0]
};





static const we_strtable_entry_t pus_CO_Attributes_stringTable[] = {
  {"uri",                PUS_ATTRIBUTE_URI             }
};

static const unsigned char pus_CO_Attributes_hashTable[] = {
  0
};

const we_strtable_info_t pus_CO_Attributes = {
  pus_CO_Attributes_hashTable,
  sizeof pus_CO_Attributes_hashTable,
  WE_TABLE_SEARCH_LINEAR,
  pus_CO_Attributes_stringTable,
  sizeof pus_CO_Attributes_stringTable / sizeof pus_CO_Attributes_stringTable[0]
};









const unsigned char pus_CO_Element_table[] = {
  0,                    
  0,                    
  0,                    
  0,                    
  0,                    
  WE_PRSR_CANCELSPACE, 
  WE_PRSR_CANCELSPACE, 
  WE_PRSR_CANCELSPACE  
};





const WE_UINT16 pus_CO_AttributeTypes[] = {
  WE_PRSR_TYPE_CDATA        
};





const WE_UINT8 pus_CO_AttrTransform[] = {
  WE_PRSR_NOT_USED,
  WE_PRSR_NOT_USED,
  WE_PRSR_NOT_USED,
  WE_PRSR_NOT_USED,
  WE_PRSR_NOT_USED,                         
  PUS_ATTRIBUTE_URI,                        
  PUS_ATTRIBUTE_URI   | WE_PRSR_DEC_DATA,  
  PUS_ATTRIBUTE_URI   | WE_PRSR_DEC_DATA,  
  PUS_ATTRIBUTE_URI   | WE_PRSR_DEC_DATA,  
  PUS_ATTRIBUTE_URI   | WE_PRSR_DEC_DATA   
};





static const we_strtable_entry_t pus_CO_AttrStart_stringTable[] = {
  {"http://",            0x06}, 
  {"http://www.",        0x07}, 
  {"https://",           0x08}, 
  {"https://www.",       0x09}  
};
 
const we_strtable_info_t pus_CO_AttrStart = {
  NULL,
  0,
  WE_TABLE_SEARCH_BINARY,
  pus_CO_AttrStart_stringTable,
  sizeof pus_CO_AttrStart_stringTable / sizeof pus_CO_AttrStart_stringTable[0]
};










static const we_strtable_entry_t pus_AttrValue_stringTable[] = {
  {".com/",            0x85},
  {".edu/",            0x86},
  {".net/",            0x87},
  {".org/",            0x88}
};

const we_strtable_info_t pus_AttrValue = {
  NULL,
  0,
  WE_TABLE_SEARCH_BINARY,
  pus_AttrValue_stringTable,
  sizeof pus_AttrValue_stringTable / sizeof pus_AttrValue_stringTable[0]
};

#endif

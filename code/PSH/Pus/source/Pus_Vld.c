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

#include "we_hdr.h"
#include "Pus_Vld.h"
#include "Pus_Msg.h"
#include "Pus_Def.h"
#include "Pus_Main.h"
#include "Pus_Cmmn.h"
#include "we_pdec.h"
#include "we_lib.h"





#define WBXML_DOCCODE_TO_DOCTYPE_PUSH          11

#define PUS_DATE_LEN       7




#define DOCTYPE_PUSH_SI                 16
#define DOCTYPE_PUSH_SL                 17
#define DOCTYPE_PUSH_CO                 18
#define DOCTYPE_UNKNOWN                 99

const we_strtable_entry_t pus_hvld_PubidLiteral_table[] = 
{
  {"-//WAPFORUM//DTD SI 1.0//EN",  DOCTYPE_PUSH_SI},
  {"-//WAPFORUM//DTD SL 1.0//EN",  DOCTYPE_PUSH_SL},
  {"-//WAPFORUM//DTD CO 1.0//EN",  DOCTYPE_PUSH_CO}
};

static const unsigned char pus_hvld_PubidLiteral_hashTable[] = 
{
  255, 1, 2, 0, 255
};

const we_strtable_info_t pus_hvld_PubidLiteral = 
{
	pus_hvld_PubidLiteral_hashTable,
	sizeof pus_hvld_PubidLiteral_hashTable,
	WE_TABLE_SEARCH_LINEAR,
  pus_hvld_PubidLiteral_table,
  sizeof pus_hvld_PubidLiteral_table / sizeof pus_hvld_PubidLiteral_table[0]
};





static int
pus_hvld_SI_startElement (void                  *userData,
                          int                    type,
                          we_prsr_attribute_t  *attrs,
                          int                    size);

static int
pus_hvld_SL_startElement (void                  *userData,
                          int                    type,
                          we_prsr_attribute_t  *attrs,
                          int                    size);

static int
pus_hvld_CO_startElement (void                 *userData,
                          int                    type,
                          we_prsr_attribute_t  *attrs,
                          int                    size);

static int
pus_hvld_endElement (void *userData);

static int
pus_hvld_characterData (void *userData, const char *s, int len, int mode);

static WE_UINT32
pus_datetime2int (char *data, int data_len);

static WE_UINT32
pus_txtdatetime2int (char *data, int data_len);















int
pus_hvld_init_parser_object (we_prsr_parserData_t*  p,
                             we_prsr_decoderData_t* d,
                             const char*             doctype_str,
                             int                     len,
                             int                     doctype_int,
                             int                     charset)
{
  int                doctype;

  if (charset < 0)
    return WE_PRSR_APPLICATION_OK;

  if (doctype_str != NULL)
    doctype = we_cmmn_str2int (doctype_str, len, &pus_hvld_PubidLiteral);
  else if (doctype_int != 0) 
  {
    
    doctype = doctype_int + WBXML_DOCCODE_TO_DOCTYPE_PUSH;
  } 
  else
    doctype = DOCTYPE_UNKNOWN;

  if (doctype == -1 || 
      doctype == DOCTYPE_UNKNOWN) 
  {
    
    switch (((pus_hvld_user_data_t*) p->userData)->content_type) 
   {
    case WE_MIME_TYPE_TEXT_VND_WAP_SI:
    case WE_MIME_TYPE_APPLICATION_VND_WAP_SIC:
      doctype = DOCTYPE_PUSH_SI;
      break;

    case WE_MIME_TYPE_TEXT_VND_WAP_SL:
    case WE_MIME_TYPE_APPLICATION_VND_WAP_SLC:
      doctype = DOCTYPE_PUSH_SL;
      break;

    case WE_MIME_TYPE_TEXT_VND_WAP_CO:
    case WE_MIME_TYPE_APPLICATION_VND_WAP_COC:
      doctype = DOCTYPE_PUSH_CO;
      break;

    default:
      return WE_PRSR_APPLICATION_ERROR;
      break;
    }
  }
  
 switch (doctype) 
 {
  case DOCTYPE_PUSH_SI:
    p->startHandler  = pus_hvld_SI_startElement;

    p->elements      = &pus_SI_Elements;
    p->attributes    = &pus_SI_Attributes;
    p->attrvals      = &pus_SI_Attrvals; 
    p->elementTable  = pus_SI_Element_table;
    p->attributeType = pus_SI_AttributeTypes;

    break;

  case DOCTYPE_PUSH_SL:
    p->startHandler  = pus_hvld_SL_startElement;

    p->elements      = &pus_SL_Elements;
    p->attributes    = &pus_SL_Attributes;
    p->attrvals      = &pus_SL_Attrvals; 
    p->elementTable  = pus_SL_Element_table;
    p->attributeType = pus_SL_AttributeTypes;

    break;
  
  default: 
    p->startHandler  = pus_hvld_CO_startElement;

    p->elements      = &pus_CO_Elements;
    p->attributes    = &pus_CO_Attributes; 
    p->elementTable  = pus_CO_Element_table;
    p->attributeType = pus_CO_AttributeTypes;

    break;

  }
 
  p->endHandler    = pus_hvld_endElement;
  p->charHandler   = pus_hvld_characterData;

  if (d != NULL) 
  { 
    d->attrValue     = &pus_AttrValue;

    switch (doctype) 
   {
    case DOCTYPE_PUSH_SI:
      d->transform     = pus_SI_AttrTransform;
      d->transformSize = PUS_SI_TRANSFORM_SIZE;
      d->attrStart     = &pus_SI_AttrStart;
      d->specific      = we_dec_pushSpecific;
      break;

    case DOCTYPE_PUSH_SL:
      d->transform     = pus_SL_AttrTransform;
      d->transformSize = PUS_SL_TRANSFORM_SIZE;
      d->attrStart     = &pus_SL_AttrStart;
      d->specific      = NULL;
      break;

    case DOCTYPE_PUSH_CO:
      d->transform     = pus_CO_AttrTransform;
      d->transformSize = PUS_CO_TRANSFORM_SIZE;
      d->attrStart     = &pus_CO_AttrStart;
      d->specific      = NULL;
      break;
          
    default:
      return WE_PRSR_APPLICATION_ERROR;

    }
  }

  return WE_PRSR_APPLICATION_OK;
}





static int
pus_hvld_SI_startElement (void                  *userData,
                          int                    type,
                          we_prsr_attribute_t  *attrs,
                          int                    size)
{
  int                    i;
  pus_hvld_user_data_t  *data = (pus_hvld_user_data_t*) userData;
  Pus_Msg_list_t        *push_msg = pus_find_msg_by_id (data->instanceId);
  
  if (push_msg == NULL)
    return WE_PRSR_APPLICATION_ERROR;

  push_msg->parsed_info.priority = PUS_PRIO_MEDIUM;  ;


  if ((type == PUS_ELEMENT_SI) &&/*"si"*/
      ((data->content_type == WE_MIME_TYPE_TEXT_VND_WAP_SI) ||
       (data->content_type == WE_MIME_TYPE_APPLICATION_VND_WAP_SIC)) &&
       (push_msg->state == PUS_STATE_PRS_STARTED))
    {
    		push_msg->state = PUS_STATE_PARSING_SI;
     }
  
  else if ( (type == PUS_ELEMENT_INDICATION) &&/*indication*/
            (push_msg->state == PUS_STATE_PARSING_SI)) 
   {
      
    for (i = 0; i < size; i++) 
   {

      switch (attrs[i].type) 
     {
	/*action*/
      case PUS_ATTRIBUTE_ACTION:
        
        if ( attrs[i].u.i == PUS_ATTRVAL_SIGNAL_NONE ) 
          push_msg->parsed_info.priority = PUS_PRIO_NONE;
        
        else if ( attrs[i].u.i == PUS_ATTRVAL_SIGNAL_LOW ) 
          push_msg->parsed_info.priority = PUS_PRIO_LOW;
        
        else if ( attrs[i].u.i == PUS_ATTRVAL_SIGNAL_MEDIUM ) 
          push_msg->parsed_info.priority = PUS_PRIO_MEDIUM;
        
        else if ( attrs[i].u.i == PUS_ATTRVAL_SIGNAL_HIGH ) 
          push_msg->parsed_info.priority = PUS_PRIO_HIGH;
        
        else if ( attrs[i].u.i == PUS_ATTRVAL_SIGNAL_DELETE ) 
          push_msg->parsed_info.priority = PUS_PRIO_DELETE;

        else
          push_msg->parsed_info.priority = PUS_PRIO_MEDIUM;

        break;
      /*href*/
      case PUS_ATTRIBUTE_HREF:
        push_msg->parsed_info.href = attrs[i].u.s;
        break;
     /*created*/
      case PUS_ATTRIBUTE_CREATED: 
        if (attrs[i].u.s[0] == '$') 
          push_msg->parsed_info.created = pus_datetime2int (attrs[i].u.s + 1, PUS_DATE_LEN);
        else 
          push_msg->parsed_info.created = pus_txtdatetime2int (attrs[i].u.s, strlen (attrs[i].u.s));
          
        PUS_MEM_FREE (attrs[i].u.s);
        break;
     /*si-expries*/
      case PUS_ATTRIBUTE_SI_EXPIRES:
        if (attrs[i].u.s[0] == '$') 
          push_msg->parsed_info.expired = pus_datetime2int (attrs[i].u.s + 1, PUS_DATE_LEN);
        else 
          push_msg->parsed_info.expired = pus_txtdatetime2int (attrs[i].u.s, strlen (attrs[i].u.s));
         
        PUS_MEM_FREE (attrs[i].u.s);
        break;
      /*si-vid*/
      case PUS_ATTRIBUTE_SI_ID:
        push_msg->parsed_info.si_id = attrs[i].u.s;
        break;

      default:
        break;
      }
    }
  }
  else 
  {
    
    we_prsr_free_attributes (WE_MODID_PUS, attrs, size);
    return WE_PRSR_APPLICATION_ERROR;
  }

  PUS_MEM_FREE (attrs);
  return WE_PRSR_APPLICATION_OK;
}




static int
pus_hvld_SL_startElement (void                  *userData,
                          int                    type,
                          we_prsr_attribute_t  *attrs,
                          int                    size)
{
  int                   i;
  pus_hvld_user_data_t  *data = (pus_hvld_user_data_t*) userData;
  Pus_Msg_list_t        *push_msg = pus_find_msg_by_id (data->instanceId);
  
  if (push_msg == NULL)
    return WE_PRSR_APPLICATION_ERROR;

  push_msg->parsed_info.priority = PUS_PRIO_LOW  ;

  if ((type == PUS_ELEMENT_SL) &&
      ((data->content_type == WE_MIME_TYPE_TEXT_VND_WAP_SL) ||
       (data->content_type == WE_MIME_TYPE_APPLICATION_VND_WAP_SLC)) &&
      (push_msg->state == PUS_STATE_PRS_STARTED))
    {
    
    push_msg->state = PUS_STATE_PARSING_SL;

    for (i = 0; i < size; i++) 
   {

      if (attrs[i].type == PUS_ATTRIBUTE_ACTION) 
      {
        
        if ( attrs[i].u.i == PUS_ATTRVAL_EXECUTE_LOW ) 
          push_msg->parsed_info.priority = PUS_PRIO_LOW;
        
        else if ( attrs[i].u.i == PUS_ATTRVAL_EXECUTE_HIGH ) 
          push_msg->parsed_info.priority = PUS_PRIO_HIGH;
        
        else if ( attrs[i].u.i == PUS_ATTRVAL_CACHE ) 
          push_msg->parsed_info.priority = PUS_PRIO_CACHE;

        else
          push_msg->parsed_info.priority = PUS_PRIO_LOW;

      }

      else if (attrs[i].type == PUS_ATTRIBUTE_HREF)
        push_msg->parsed_info.href = attrs[i].u.s;
    }  
  }
    
  else
  {
    
    we_prsr_free_attributes (WE_MODID_PUS, attrs, size);
    return WE_PRSR_APPLICATION_ERROR;
  }

  PUS_MEM_FREE (attrs);
  return WE_PRSR_APPLICATION_OK;
}





static int
pus_hvld_CO_startElement (void                 *userData,
                         int                    type,
                         we_prsr_attribute_t  *attrs,
                         int                    size)
{
  int                    i;
  pus_co_data_t         *new_co;
  pus_hvld_user_data_t  *data = (pus_hvld_user_data_t*) userData;
  Pus_Msg_list_t        *push_msg = pus_find_msg_by_id (data->instanceId);

  if (push_msg == NULL)
    return WE_PRSR_APPLICATION_ERROR;
  
  if ( ( type == PUS_ELEMENT_CO) &&
       (( data->content_type == WE_MIME_TYPE_TEXT_VND_WAP_CO) ||
        ( data->content_type == WE_MIME_TYPE_APPLICATION_VND_WAP_COC)) &&
       ( push_msg->state == PUS_STATE_PRS_STARTED))
    {
      		push_msg->state = PUS_STATE_PARSING_CO;
     }

  else if ( (( type == PUS_ELEMENT_INVALIDATE_OBJECT) ||
             ( type == PUS_ELEMENT_INVALIDATE_SERVICE)) &&
            ( push_msg->state == PUS_STATE_PARSING_CO) ) 
  {

    for (i = 0; i < size; i++) 
   {

      if (attrs[i].type == PUS_ATTRIBUTE_URI) 
      {
        new_co = PUS_MEM_ALLOCTYPE (pus_co_data_t);
        new_co->type = type;
        new_co->text = attrs[i].u.s;
        if ( push_msg->parsed_info.co_data == NULL) 
	{
          push_msg->parsed_info.co_data =  new_co;
          new_co->next = NULL;
        }
        else 
	 {
          new_co->next = push_msg->parsed_info.co_data;
          push_msg->parsed_info.co_data = new_co;
        }
      }
    }  
  }

  else 
  {
    
    we_prsr_free_attributes (WE_MODID_PUS, attrs, size);
    return WE_PRSR_APPLICATION_ERROR;
  }

  PUS_MEM_FREE (attrs);
  return WE_PRSR_APPLICATION_OK;
}




static int
pus_hvld_endElement (void *userData) 
{
  userData = userData;
  
  return WE_PRSR_APPLICATION_OK;
}




static int
pus_hvld_characterData (void *userData, const char *s, int len, int mode)
{
  pus_hvld_user_data_t  *data = (pus_hvld_user_data_t*) userData;
  Pus_Msg_list_t        *push_msg = pus_find_msg_by_id (data->instanceId);
  mode = mode;

  if (push_msg->state == PUS_STATE_PARSING_SI) 
  {
    if (!push_msg->parsed_info.text) 
    {
      push_msg->parsed_info.text = we_cmmn_strndup (WE_MODID_PUS, s, len);
    }
    else  
   {
      char *old_text = push_msg->parsed_info.text;
      char *new_text = we_cmmn_strndup (WE_MODID_PUS, s, len);
      push_msg->parsed_info.text = we_cmmn_strcat (WE_MODID_PUS, old_text, new_text);
      PUS_MEM_FREE (old_text);
      PUS_MEM_FREE (new_text);
    }
  }
  

  return WE_PRSR_APPLICATION_OK;
}









static const WE_UINT16 monthdays[12] = {
  0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};






static WE_UINT32
pus_datetime2int (char *data, int data_len)
{
  WE_UINT32 century = 0;
  WE_UINT32 year = 0;
  WE_UINT32 month = 0;
  WE_UINT32 day = 0;
  WE_UINT32 hour = 0;
  WE_UINT32 min = 0;
  WE_UINT32 sec = 0;
  WE_UINT32 leapyears;

  switch (data_len) {
  default:
    sec = (((unsigned char)data[6]) >> 4) * 10 + (data[6] & 0x0f);
  case 6:
    min = (((unsigned char)data[5]) >> 4) * 10 + (data[5] & 0x0f);
  case 5:
    hour = (((unsigned char)data[4]) >> 4) * 10 + (data[4] & 0x0f);
  case 4:
    day = (((unsigned char)data[3]) >> 4) * 10 + (data[3] & 0x0f);
  case 3:
    month = (((unsigned char)data[2]) >> 4) * 10 + (data[2] & 0x0f);
  case 2:    
    year = (((unsigned char)data[1])>> 4) * 10 + (data[1] & 0x0f);
  case 1:
    century = (((unsigned char)data[0]) >> 4) * 10 + (data[0] & 0x0f);
    break;

  case 0:
    return 0;
  }

  year = century * 100 + year - 1970;

  leapyears = (year + 2) >> 2;
  if (((year & 0x03) == 0x02) && (month <= 2))
    leapyears--;

  return year * 31536000 +
    (leapyears + monthdays[month - 1] + day - 1) * 86400 +
    hour * 3600 + min * 60 + sec;
}






static WE_UINT32
pus_txtdatetime2int (char *data, int data_len)
{
  int month = 0, 
      date = 0, 
      hour = 0, 
      min = 0, 
      sec = 0,
      leapyears = 0,
      year = 0;

  year = atoi (data);
  year -= 1970; 
  if (data_len > 5)
    month = atoi (data + 5);
  if (data_len > 8)
    date = atoi (data + 8);
  if (data_len > 11)
    hour = atoi (data + 11);
  if (data_len > 14)
    min = atoi (data + 14);
  if (data_len > 17)
    sec = atoi (data + 17);

  leapyears = (year + 2) >> 2;
  if (((year & 0x03) == 0x02) && (month <= 2))
    leapyears--;

  return ( year * 31536000 +
           (leapyears + monthdays[month - 1] + date - 1) * 86400 +
           hour * 3600 + min * 60 + sec);
}

#endif

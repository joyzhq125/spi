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

#include "we_pipe.h"
#include "we_prsr.h"
#include "we_ptxt.h"
#include "we_hdr.h"
#include "we_lib.h"
#include "we_log.h"

#include "Pus_Main.h"
#include "Pus_Prh.h"
#include "Pus_Cfg.h"
#include "Pus_Msg.h"
#include "Pus_Vld.h"
#include "Pus_Sig.h"












static we_prsr_parser_t     *pus_parser_vec[PUS_CFG_MAX_CONCURRENT_PARSINGS];





static we_prsr_parser_t* 
pus_prs_create_parser_object (pus_start_parsing_t* start_parsing);

static void
pus_prs_parse_section (we_prsr_parser_t* parser);

static we_prsr_parser_t*
pus_prs_find_parser (int wid);

static void
Pus_Prh_delete_parser_object (int wid);







void
Pus_Prh_init (void)
{
  int i;

  for (i = 0; i < PUS_CFG_MAX_CONCURRENT_PARSINGS; i++)
    pus_parser_vec[i] = NULL;

  Pus_Signal_register_dst (PUS_MODULE_PARSER, Pus_Prh_main);
}
void
Pus_Prh_terminate (void)
{
  int i;

 for (i = 0; i < PUS_CFG_MAX_CONCURRENT_PARSINGS; i++) {
    if (pus_parser_vec[i] != NULL)
      Pus_Prh_delete_parser_object (
                      ((pus_hvld_user_data_t*) pus_parser_vec[i]->p.userData)->instanceId);
 }
}

/*
#define PUS_PRH_SIG_START_PARSING                   
#define PUS_PRH_SIG_PARSING_ABORTED                 
#define PUS_PRH_SIG_PARSING_COMPLETED                
#define PUS_PRH_SIG_PARSE_NEXT                     
*/
void
Pus_Prh_main (Pus_Signal_t* signal)
{
  we_prsr_parser_t* parser = NULL;

  if (signal->type == PUS_PRH_SIG_START_PARSING) 
  { 
    pus_start_parsing_t* start_parsing = (pus_start_parsing_t*) signal->p_param;
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS, "PUS_PRH: received PUS_PRH_SIG_START_PARSING\n"));
    parser = pus_prs_create_parser_object (start_parsing);
    if (parser == NULL) 
    { 
      PUS_SIGNAL_SENDTO_I (PUS_MODULE_MSG, PUS_PRH_SIG_PARSING_ABORTED,
                           start_parsing->instance_id);
    }
    PUS_MEM_FREE (start_parsing);
  } 
  else 
  {
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS, "PUS_PRH: received PUS_PRH_SIG_PARSE_NEXT\n"));
    if (signal->type == PUS_PRH_SIG_PARSE_NEXT) 
   {
         parser = pus_prs_find_parser (signal->i_param1);
    }
    else 
   {
        parser = pus_prs_find_parser (signal->i_param2);
    }
    if (parser == NULL) {
      Pus_Signal_delete (signal);
      return;
    }
  }

  pus_prs_parse_section (parser);
  Pus_Signal_delete (signal);
}







static we_prsr_parser_t* 
pus_prs_create_parser_object (pus_start_parsing_t* start_parsing)
{
  we_prsr_parser_t         *parser;
  we_prsr_init_t           *init_funct;
  we_prsr_piHandler_t      *pi_hndlr;
  pus_hvld_user_data_t      *data;
  void                      *user_data;
  char                      *buffer;      
  int                        text_mode = TRUE;
  int                        content_type;
  int                        i;
  int                        length;
  int                        handle;
  int                        type;
  int                        stream_handle;

  content_type = start_parsing->content_type_int;

  for (i = 0; i < PUS_CFG_MAX_CONCURRENT_PARSINGS; i++) 
  {
    if (pus_parser_vec[i] == NULL) 
   {

      if ( content_type == WE_MIME_TYPE_APPLICATION_VND_WAP_COC ||
            content_type == WE_MIME_TYPE_APPLICATION_VND_WAP_SIC ||
          content_type == WE_MIME_TYPE_APPLICATION_VND_WAP_SLC)
        text_mode = FALSE;
      
      if (start_parsing->data_type == WE_CONTENT_DATA_PIPE) 
     { 
        handle = WE_PIPE_OPEN (WE_MODID_PUS, (char*)start_parsing->data);
        if (handle < 0)
          return NULL;
        stream_handle = pus_stream_register_handle (start_parsing->instance_id,
                                                    PUS_STREAM_PIPE_READ, handle,
                                                    PUS_MODULE_PARSER,
                                                    (char*)start_parsing->data);
        buffer = NULL;
        length = -1;
        type = WE_PRSR_PIPE;

      } 
      else if (start_parsing->data_type == WE_CONTENT_DATA_RAW) 
      { 
        handle = -1;
        stream_handle = -1;
        buffer = (char*)start_parsing->data;
        if (text_mode)
          length = strlen (buffer);
        else
          length = start_parsing->data_len;
        type = WE_PRSR_BUFFER;
      }
      else 
      { 
        
        stream_handle = -1;
        handle = -1;
        buffer = NULL;
        length = -1;
        type = WE_PRSR_FILE;
      }

      data = PUS_MEM_ALLOCTYPE (pus_hvld_user_data_t);
      data->instanceId       = start_parsing->instance_id;
      data->state            = PUS_STATE_PRS_STARTED;
      data->content_type     = content_type;
      data->stream_handle    = stream_handle;
      
      user_data  = (void*) data;

      init_funct = pus_hvld_init_parser_object;
      pi_hndlr   = NULL;
      
      parser = we_prsr_create (WE_MODID_PUS, 
	  						user_data, init_funct, 
	  						pi_hndlr, type,
                               			 buffer, 
                               			 length,
                               			 handle,
                               			 PUS_PRSR_BUF_SIZE,
                                			start_parsing->charset, 
                                			PUS_CFG_DEFAULT_CHARSET, 
                                			text_mode);

      pus_parser_vec[i] = parser;
      return parser;
    }
  }
  
  return NULL;
}




static void
pus_prs_parse_section (we_prsr_parser_t* parser) {

  pus_hvld_user_data_t* user_data;
  int                   result;
  int                   instanceId;

  user_data = (pus_hvld_user_data_t*) parser->p.userData;
  instanceId = user_data->instanceId;

  result = we_prsr_parse (parser);
  
  switch (result) 
  {
  case WE_PRSR_STATUS_PARSING_COMPLETED:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS, "PUS PARSER: parsing completed\n"));
    PUS_SIGNAL_SENDTO_I (PUS_MODULE_MSG, PUS_PRH_SIG_PARSING_COMPLETED, instanceId);
    break;

  case WE_PRSR_STATUS_MORE_TO_DO:
    PUS_SIGNAL_SENDTO_I (PUS_MODULE_PARSER, PUS_PRH_SIG_PARSE_NEXT, instanceId);
    return; 
  
  case WE_PRSR_STATUS_WAIT:
    
    return; 

  








  default:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS, 
                  "PUS_PRH: parsing failed; error code %i.\n", result));
    PUS_SIGNAL_SENDTO_I (PUS_MODULE_MAIN, PUS_PRH_SIG_PARSING_ABORTED, instanceId);
    break;
  }

  Pus_Prh_delete_parser_object (instanceId);
}




static we_prsr_parser_t*
pus_prs_find_parser (int wid) 
{
  int i;

  for (i = 0; i < PUS_CFG_MAX_CONCURRENT_PARSINGS; i++)
    if (pus_parser_vec[i] != NULL &&
        ((pus_hvld_user_data_t*) pus_parser_vec[i]->p.userData)->instanceId == wid) {
      return pus_parser_vec[i];
    }

  return NULL;

}




static void
Pus_Prh_delete_parser_object (int wid)
{
  pus_hvld_user_data_t*  user_data;
  int                    i;
  int                    handle;
  int                    type;

  for (i = 0; i < PUS_CFG_MAX_CONCURRENT_PARSINGS; i++) {
    if (pus_parser_vec[i] != NULL &&
        ((pus_hvld_user_data_t*) pus_parser_vec[i]->p.userData)->instanceId == wid) {
      if (we_prsr_delete (pus_parser_vec[i], &type, &handle, (void*) &user_data))
        return;
      pus_parser_vec[i] = NULL;
      pus_stream_close (user_data->stream_handle);
      PUS_MEM_FREE (user_data);
      break;
    }
  }
}

#endif

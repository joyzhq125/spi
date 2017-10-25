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

#ifdef PUS_CONFIG_MULTIPART 

#include "Pus_Mprt.h"
#include "Pus_Sig.h"
#include "Pus_Cmmn.h"
#include "we_hdr.h"
#include "Pus_Hdr.h"




#define PUS_MPRT_LINE_BUF_SIZE                256

#define PUS_MPRT_STATUS_CONTINUE              1
#define PUS_MPRT_STATUS_DONE                  2
#define PUS_MPRT_STATUS_ERROR                 3




typedef int Pus_Mprt_extract_t (Pus_Msg_list_t* msg);

struct Pus_Mprt_st {
  Pus_Mprt_extract_t      *extract_func;
  we_hdr_t               *headers;
  WE_UINT32               content_len;
  char                    *buf;
  int                      start_pos;
  int                      end_pos;
  char                    *boundary;
  int                      boundary_len;
  unsigned char            jumpbuf[256];
  char                     line_buf[PUS_MPRT_LINE_BUF_SIZE];
  int                      line_pos;
} ;




static void
Pus_Mprt_handle_new_mprt_msg (int wid);

static void
Pus_Mprt_handle_stream_notification (int stream_handle, int msg_id);

static void
Pus_Mprt_whole_message_in_buf (Pus_Msg_list_t* msg);

static void
Pus_Mprt_create (Pus_Msg_list_t* msg, Pus_Mprt_extract_t *extract_func);

static int
Pus_Mprt_extract_bin_headers (Pus_Msg_list_t* msg);

static int
Pus_Mprt_extract_text_boundary (Pus_Msg_list_t* msg);

static int
Pus_Mprt_extract_bin_content (Pus_Msg_list_t* msg);

static int
Pus_Mprt_extract_text_headers (Pus_Msg_list_t* msg);

static int
Pus_Mprt_extract_text_content (Pus_Msg_list_t* msg);

static int
Pus_Mprt_find_boundary (Pus_Msg_list_t* msg, int *pos);







void
Pus_Mprt_init (void)
{

  




  Pus_Signal_register_dst (PUS_MODULE_MPRT, Pus_Mprt_main);
}






void
Pus_Mprt_terminate (void)
{
  



}
 




void
Pus_Mprt_main (Pus_Signal_t* sig)
{

  if (sig->type == PUS_MPRT_SIG_NEW_MESSAGE) { 
    Pus_Mprt_handle_new_mprt_msg (sig->i_param1);
  } 
  else if (sig->type == PUS_SIG_NOTIFY) {
    Pus_Mprt_handle_stream_notification (sig->i_param1, sig->i_param2);   
  }
   
  Pus_Signal_delete (sig);
}

static void
Pus_Mprt_handle_stream_notification (int stream_handle, int msg_id)
{
  Pus_Msg_list_t     *msg = pus_find_msg_by_id (msg_id);

  if (Pus_Cmmn_read_data_from_stream_to_buf (stream_handle, msg) == FALSE) {
    

    Pus_Cmmn_build_buf_from_data_buf_list (msg);
    
    Pus_Mprt_whole_message_in_buf(msg);  
  }
  
}





static void
Pus_Mprt_handle_new_mprt_msg (int wid)
{
  Pus_Msg_list_t*     msg = pus_find_msg_by_id (wid);
  int                 stream_handle;

  if (msg->content->dataType == WE_CONTENT_DATA_RAW) {
    
    msg->msg_data = msg->content->_u.data;
    msg->msg_len = msg->content->dataLen;
    Pus_Mprt_whole_message_in_buf(msg);
    return;
  }
  else if (msg->content->dataType == WE_CONTENT_DATA_PIPE) {
    stream_handle = pus_stream_init (PUS_MODULE_MPRT, msg->wid, PUS_STREAM_PIPE_READ, 
                                     NULL, 0, msg->content->_u.pipeName);
  }
  else if (msg->content->dataType == WE_CONTENT_DATA_FILE) {
    stream_handle = pus_stream_init (PUS_MODULE_MPRT, msg->wid, PUS_STREAM_FILE_READ, 
                                     NULL, 0, msg->content->_u.pathName);
  }
  else {
    
    return;
  }
  if (stream_handle != -1) {
    
    if (Pus_Cmmn_read_data_from_stream_to_buf (stream_handle, msg) == FALSE) {
      

      Pus_Cmmn_build_buf_from_data_buf_list (msg);
      
      Pus_Mprt_whole_message_in_buf(msg);
    }
  }
  else
    
    return;
}





static void
Pus_Mprt_whole_message_in_buf (Pus_Msg_list_t* msg)
{
  int status;

  if (msg->content_type_int == WE_MIME_TYPE_MULTIPART_MIXED)
    
    Pus_Mprt_create (msg, Pus_Mprt_extract_text_boundary);

  else if (msg->content_type_int == WE_MIME_TYPE_APPLICATION_VND_WAP_MULTIPART_MIXED)
    
    Pus_Mprt_create (msg, Pus_Mprt_extract_bin_headers);

  else {
    pus_delete_msg_from_list (msg->wid);
    return;
  }

  
  msg->mp->start_pos = 1;
  msg->mp->end_pos = msg->content->dataLen;

  










  do {
    status = msg->mp->extract_func (msg);
  } while (status == PUS_MPRT_STATUS_CONTINUE);

  if (status == PUS_MPRT_STATUS_ERROR) {
    pus_delete_msg_from_list (msg->wid);
    return;
  }
  

  pus_delete_msg_from_list (msg->wid);
}







static void
Pus_Mprt_create (Pus_Msg_list_t* msg, Pus_Mprt_extract_t *extract_func) 
{
  Pus_Mprt_t       *mp = PUS_MEM_ALLOCTYPE (Pus_Mprt_t);
  char             *temp;
  int               i, temp_len = 2;  
  we_hdr_t        *headers = we_hdr_create (WE_MODID_PUS);

  we_hdr_add_headers_from_text (headers, msg->headers);
 
  mp->extract_func = extract_func;  
  mp->headers = NULL;
  mp->content_len = 0;
  mp->start_pos = 0;
  mp->end_pos = 0;
  mp->buf = (char*)(msg->msg_data);
  temp = we_hdr_get_parameter (headers, WE_HDR_CONTENT_TYPE, "boundary");
  we_hdr_delete (headers);
  if (temp != NULL)
    temp_len += strlen(temp);
  mp->boundary = PUS_MEM_ALLOC (temp_len + 1);
  strcpy (mp->boundary,"--");
  memcpy (mp->boundary + 2, temp,temp_len-2);
  mp->boundary [temp_len] = '\0';
  mp->line_pos = 0;
  mp->boundary_len = temp_len;

  for (i = 0; i < 256; i++) {
    mp->jumpbuf[i] = (unsigned char)temp_len;
  }
  for (i = 0; i < temp_len - 1; i++) {
    mp->jumpbuf[(unsigned char)mp->boundary[i]] = (unsigned char)(temp_len - i - 1);
  }
  msg->mp = mp;
}









static void
Pus_Mprt_send_mprt_message_to_msg_module (Pus_Msg_list_t* mprt_msg, int content_len)
{
  Pus_Msg_list_t   *entity_msg = PUS_MEM_ALLOCTYPE (Pus_Msg_list_t);
  WE_UINT32        tmp;

  entity_msg->conn_id =mprt_msg->conn_id; 
  entity_msg->url = mprt_msg->url;
  entity_msg->parent_id = mprt_msg->parent_id;
  entity_msg->content = PUS_MEM_ALLOCTYPE (stk_content_t);
  entity_msg->content->dataLen = content_len;
  entity_msg->content->dataType = WE_CONTENT_DATA_RAW;
  entity_msg->content->_u.data = PUS_MEM_ALLOC (content_len);
  entity_msg->content->contentType = NULL;
  memcpy (entity_msg->content->_u.data, mprt_msg->mp->buf + mprt_msg->mp->start_pos, content_len);
  entity_msg->fromAddr = mprt_msg->fromAddr;
  we_hdr_make_string (mprt_msg->mp->headers, &(entity_msg->headers));
  entity_msg->method = mprt_msg->method;
  entity_msg->req_id = mprt_msg->req_id;
  entity_msg->content_location = NULL;
  Pus_Cmmn_get_content_type (entity_msg->headers, &tmp);
  entity_msg->content_type_int = (int)tmp;
  entity_msg->content_uri = NULL;
  entity_msg->dialog_id = 0;
  entity_msg->wid = (WE_INT16)pus_new_handle ();
  entity_msg->initiator_uri = NULL;
  entity_msg->parsed_info.co_data = NULL;
  entity_msg->parsed_info.si_id = NULL;
  entity_msg->parsed_info.href = NULL;
  entity_msg->parsed_info.text = NULL;
  entity_msg->parsed_info.priority = -1;
  entity_msg->next_msg = NULL;
  entity_msg->routing_type = 0;
  entity_msg->routing_identifier = NULL;
  entity_msg->state = PUS_STATE_NEW_MSG;
  entity_msg->whitelist_result = 0;
  entity_msg->msg_data = NULL;
  entity_msg->msg_len = 0;
  entity_msg->mp = NULL;
  entity_msg->url = NULL;

  
  entity_msg->next_msg = Pus_Msg_list;
  Pus_Msg_list = entity_msg;

  PUS_SIGNAL_SENDTO_I (PUS_MODULE_MSG, PUS_MSG_SIG_NEW_MESSAGE, entity_msg->wid);
}




static int
Pus_Mprt_extract_bin_headers (Pus_Msg_list_t* msg)
{
  we_dcvt_t      cvt_obj;
  WE_UINT32      ext_hdr_len;
  unsigned char  *ext_hdr;
  Pus_Mprt_t     *mp = msg->mp;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_DECODE, mp->buf + mp->start_pos, 
                 mp->end_pos - mp->start_pos, WE_MODID_PUS);

  if (!we_dcvt_uintvar (&cvt_obj, &ext_hdr_len))
    return PUS_MPRT_STATUS_ERROR;

   if (!we_dcvt_uintvar (&cvt_obj, &(mp->content_len)))
    return PUS_MPRT_STATUS_ERROR;
  
  if ((int)ext_hdr_len + cvt_obj.pos > cvt_obj.length)
    return PUS_MPRT_STATUS_ERROR;

  if (!we_dcvt_uchar_vector (&cvt_obj, (WE_UINT16)ext_hdr_len, &ext_hdr))
    return PUS_MPRT_STATUS_ERROR;

  

  mp->headers = we_hdr_create (WE_MODID_PUS);

  if (!Pus_Hdr_create_hdr_string_from_bin_hdrs (mp->headers, ext_hdr, ext_hdr_len))
  { 
    we_hdr_delete (mp->headers);
    mp->headers = NULL;
    return PUS_MPRT_STATUS_ERROR;
  }
  PUS_MEM_FREE (ext_hdr);

  mp->start_pos += cvt_obj.pos;
  
  mp->extract_func = Pus_Mprt_extract_bin_content;
  
  return PUS_MPRT_STATUS_CONTINUE;
}

static int
Pus_Mprt_extract_bin_content (Pus_Msg_list_t* msg)
{
  Pus_Mprt_t     *mp = msg->mp;
  int            write_len;

  write_len = (int)MIN (mp->content_len, (WE_UINT32)(mp->end_pos - mp->start_pos));
  
  Pus_Mprt_send_mprt_message_to_msg_module (msg, write_len);
  mp->start_pos += write_len;

  if (mp->start_pos < mp->end_pos) {
    
    mp->extract_func = Pus_Mprt_extract_bin_headers;
    return PUS_MPRT_STATUS_CONTINUE;
  } 
  else {
     
    return PUS_MPRT_STATUS_DONE;
  }
}








static int
Pus_Mprt_read_one_line (Pus_Msg_list_t* msg)
{
  Pus_Mprt_t     *mp = msg->mp;
  int             c;

  while (mp->start_pos < mp->end_pos) {
    c = (int)mp->buf[mp->start_pos++];

    if (c != '\n') {
      if (mp->line_pos + 1 < PUS_MPRT_LINE_BUF_SIZE)
        mp->line_buf[mp->line_pos++] = (char)c;
    }
    else {
      





      if ((mp->line_pos > 0) &&
          (mp->line_buf[mp->line_pos - 1] == '\r'))
        mp->line_pos--;
      mp->line_buf[mp->line_pos] = '\0';
      mp->line_pos = 0;
      return 1;
    }
  }
  return 0;
}


static int
Pus_Mprt_extract_text_boundary (Pus_Msg_list_t* msg)
{
  Pus_Mprt_t     *mp = msg->mp;
  char           *boundary = mp->boundary;
  int             boundary_len = mp->boundary_len;
  int             line_len;
 
  while (Pus_Mprt_read_one_line (msg)) {
    line_len = (int)strlen (mp->line_buf);

    if ((line_len >= boundary_len) &&
        (strncmp (mp->line_buf, boundary, boundary_len) == 0)) {
      if (strncmp (mp->line_buf + boundary_len, "--", 2) == 0) {
        
        return PUS_MPRT_STATUS_CONTINUE;
      }
      mp->extract_func = Pus_Mprt_extract_text_headers;
      mp->headers = we_hdr_create (WE_MODID_PUS);
      return PUS_MPRT_STATUS_ERROR;
    }
  }        
  
  return PUS_MPRT_STATUS_ERROR;
}

static int
Pus_Mprt_extract_text_headers (Pus_Msg_list_t* msg)
{
  Pus_Mprt_t     *mp = msg->mp;
  
  while (Pus_Mprt_read_one_line (msg))
  {
    if (strlen (mp->line_buf) > 0) {
      
      we_hdr_add_header_from_text_line (mp->headers, mp->line_buf, strlen (mp->line_buf));
    }
    else {
      
      mp->extract_func = Pus_Mprt_extract_text_content;
      return PUS_MPRT_STATUS_CONTINUE;
    }
  }        
  
  return PUS_MPRT_STATUS_ERROR;
}


static int
Pus_Mprt_extract_text_content (Pus_Msg_list_t* msg)
{
  Pus_Mprt_t     *mp = msg->mp;
  int             write_len, pos;
  int             end_of_entity = FALSE;

  
  end_of_entity = Pus_Mprt_find_boundary (msg, &pos);
  write_len = pos - mp->start_pos;

  if (end_of_entity) {
    

    int k = pos - 1;

    if ((k >= mp->start_pos) && (mp->buf[k] == '\n')) {
      write_len--;
      k--;
    }
    if ((k >= mp->start_pos) && (mp->buf[k] == '\r')) {
      write_len--;
    }
  }

  if (end_of_entity){
    

    Pus_Mprt_send_mprt_message_to_msg_module (msg, write_len);
    mp->extract_func = Pus_Mprt_extract_text_boundary;

    


    if ((mp->start_pos + pos + mp->boundary_len + 4) < mp->end_pos)  
      return PUS_MPRT_STATUS_CONTINUE;
    else 
      return PUS_MPRT_STATUS_DONE;
  } 
  else {
     
    return PUS_MPRT_STATUS_ERROR;
  }
}











static int
Pus_Mprt_find_boundary (Pus_Msg_list_t* msg, int *pos)
{
  Pus_Mprt_t     *mp = msg->mp;
  int             m = mp->boundary_len;
  int             k;

  k = mp->start_pos + m - 1;

  while (k < mp->end_pos) {
    int j = m - 1;
    int i = k;

    while ((j >= 0) && (mp->buf[i] == mp->boundary[j])) {
      j--;
      i--;
    }
    if (j == -1) {
      *pos = i + 1;
      return TRUE;
    }
    k += mp->jumpbuf[(unsigned char)mp->buf[k]] + j + 1 - m;
  }

  *pos = k - m + 1;

  return FALSE;
}

#endif

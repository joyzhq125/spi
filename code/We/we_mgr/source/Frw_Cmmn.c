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









#include "Frw_Cmmn.h"
#include "Frw_Sig.h"
#include "Frw.h"
#include "Frw_Int.h"
#include "Frw_Dlg.h"
#include "We_Wid.h"
#include "We_Core.h"
#include "We_Log.h"
#include "We_Mem.h"
#include "We_File.h"
#include "We_Pipe.h"
#include "We_Act.h"

#define WE_CMMN_ACT_STATE_GET_ACTION_REQ     1
#define WE_CMMN_ACT_STATE_OBJECT_ACTION_REQ  2 

typedef struct we_dlg_op_st {
  struct we_dlg_op_st    *next;
  WE_UINT32              wid;          
  int                     state;
  long                    status;    
  we_act_content_t       content;
  char**                  excl_act_strings;
  WE_INT32               excl_act_strings_cnt;
  WE_UINT8               ack_dest_modid;
  WE_UINT8               ack_id;
  WE_BOOL                caller_owns_pipe;
  frw_dlg_t              *window;
  WeScreenHandle         screen;
  WE_INT32               pending_op_id;
  WE_INT32               num_actions;
  we_act_action_entry_t *actions;  
} we_action_op_t;

static we_action_op_t *we_op_list;

static we_pck_handle_t *we_act_hdl;






static we_action_op_t *
get_new_op (void)
{
  static int iWeActUniqId = 1;

  we_action_op_t *pstOp = WE_MEM_ALLOCTYPE (WE_MODID_FRW, we_action_op_t);
  
  pstOp->wid = iWeActUniqId++;
  pstOp->state = WE_CMMN_ACT_STATE_GET_ACTION_REQ;
  pstOp->content.mime_type = NULL;
  pstOp->content.data_type = WeResourceFile;
  pstOp->content.data = NULL;
  pstOp->content.data_len = 0;
  pstOp->content.src_path = NULL;
  pstOp->content.default_name = NULL;
  pstOp->content.content_type = NULL;
  pstOp->excl_act_strings = NULL;
  pstOp->excl_act_strings_cnt= 0;
  pstOp->ack_dest_modid = 0;
  pstOp->ack_id = 0;
  pstOp->caller_owns_pipe = FALSE;
  pstOp->window = NULL;
  pstOp->screen = 0;
  pstOp->status = 0;
  pstOp->pending_op_id = 0;
  pstOp->num_actions   = 0;
  pstOp->actions = NULL;

  
  pstOp->next = we_op_list;
  we_op_list = pstOp;

  return pstOp;
}

static void
free_op (we_action_op_t *pstOp)
{
  we_action_op_t *pTempstOp = we_op_list;
  we_action_op_t *pPrevOp = NULL;
  WE_INT32        i;

  

  while (pTempstOp != pstOp) {
    pPrevOp = pTempstOp;
    pTempstOp = pTempstOp->next;
  }
  if (pPrevOp)
    pPrevOp->next = pstOp->next;
  else
    we_op_list = pstOp->next;

  if (we_op_list == NULL)
  {
    
    we_act_terminate (we_act_hdl);
    we_act_hdl = NULL;
  }

  if (pstOp->window != NULL) {
    frw_dlgDelete(pstOp->window);
    WE_WIDGET_RELEASE(pstOp->screen);
  }

  if (pstOp->content.mime_type)
    FRW_FREE(pstOp->content.mime_type);
  if (pstOp->content.src_path)
    FRW_FREE(pstOp->content.src_path);
  if (pstOp->content.data)
    FRW_FREE(pstOp->content.data);
  if (pstOp->content.default_name)
    FRW_FREE(pstOp->content.default_name);
  if (pstOp->content.content_type)
    FRW_FREE(pstOp->content.content_type);

  if (pstOp->excl_act_strings) {
    for (i=0; i<pstOp->excl_act_strings_cnt; i++) {
      if (pstOp->excl_act_strings[i])
        FRW_FREE(pstOp->excl_act_strings[i]);
    }
    FRW_FREE(pstOp->excl_act_strings);
  }

   if (pstOp->actions)
    FRW_FREE(pstOp->actions);

  FRW_FREE(pstOp);
  pstOp = NULL;

}

static void
delete_op_list (we_action_op_t **ppList)
{
  we_action_op_t *pstOp;

  while (*ppList != NULL)
  {
    pstOp = *ppList;
    free_op (pstOp);
  }
  
  *ppList = NULL;
}

static void
handle_obj_act_succ(we_action_op_t *pstOp)
{
  if (pstOp->ack_dest_modid) {
    frw_content_send_ack (pstOp->ack_dest_modid, pstOp->ack_id,
                          WE_CONTENT_STATUS_SUCCEED, FALSE);
  }
}

static void
handle_obj_act_err(we_action_op_t *pstOp)
{
  if (pstOp->ack_dest_modid) {
    frw_content_send_ack (pstOp->ack_dest_modid, pstOp->ack_id,
                          WE_CONTENT_STATUS_FAILED, pstOp->caller_owns_pipe);
  }
  else if (pstOp->content.src_path && (pstOp->content.data_type == WeResourcePipe)) {
    WE_PIPE_DELETE (pstOp->content.src_path);
  }
}

static void
we_cmmn_display_action_menu (we_action_op_t *pstOp)
{
  int i;
  WE_INT32 *piStrIds = (WE_INT32*) WE_MEM_ALLOC(WE_MODID_FRW,
                                                  pstOp->num_actions * sizeof(WE_INT32));

  for (i = 0; i < pstOp->num_actions; i++)
  {
    piStrIds[i] = pstOp->actions[i].string_id;
  }

  pstOp->screen = WE_WIDGET_SCREEN_CREATE (WE_MODID_FRW, 0);
  if (pstOp->screen) {
    WE_WIDGET_DISPLAY_ADD_SCREEN(pstOp->screen);
    pstOp->window = frw_dlgCreate (pstOp->screen, FRW_DLG_OBJACT_ACTION_MENU, 0, NULL,
                                pstOp->num_actions, piStrIds);
  }

  WE_MEM_FREE(WE_MODID_FRW, piStrIds);
}

static void
we_cmmn_display_no_action_dialog (we_action_op_t *pstOp)
{
  pstOp->screen = WE_WIDGET_SCREEN_CREATE (WE_MODID_FRW, 0);
  if (pstOp->screen) {
    WE_WIDGET_DISPLAY_ADD_SCREEN(pstOp->screen);
    pstOp->window = frw_dlgCreate (pstOp->screen, FRW_DLG_OBJACT_NO_ACTION_INFO,
                                0, NULL, 0, NULL);
  }
}

static void
we_cmmn_check_obj_act_result
(
    we_action_op_t *pstOp, 
    int* piRes, 
    we_pck_result_t* pstResult
)
{
  if (*piRes != WE_PACKAGE_BUSY) {
    switch (pstResult->type) {
    


    case WE_ACT_OBJECT_ACTION :
      if ((*piRes == WE_PACKAGE_COMPLETED) && (pstResult->_u.i_val == WE_ACT_OK))
        handle_obj_act_succ(pstOp);
      else {
        if ((pstResult->_u.i_val == WE_ACT_CANCELLED) ||
            (*piRes == WE_PACKAGE_ERROR)) {
          we_act_error_result_object_action_t *err_res;
          err_res = (we_act_error_result_object_action_t*) pstResult->additional_data;
          pstOp->caller_owns_pipe = err_res->caller_owns_pipe;
        }
        handle_obj_act_err(pstOp);
      }
      break;

    default :
      break;
    } 
  }
}






typedef struct {
  const char *mime_type;
  WE_UINT8   resource_type;
  const char *resource_str_id;
  WE_UINT32  resource_int_id;
}we_icon_mime_map_table_t;


typedef struct we_cmmn_elem_st {
  WE_UINT32       hash;
  const char      *s_mime;
  WE_INT32        i_mime;
  WE_UINT8        resource_type;
  const char      *resource_str_id;
  WE_UINT32       resource_int_id;
  struct we_cmmn_elem_st *next;
} we_cmmn_elem_t;


static we_icon_mime_map_table_t we_icon_mime_map_table[] = WE_ICON_MIME_MAP_TABLE;


static we_cmmn_elem_t *
get_new_elem (void)
{
  we_cmmn_elem_t *pstElem = WE_MEM_ALLOCTYPE (WE_MODID_FRW, we_cmmn_elem_t);

  pstElem->hash = 0;
  pstElem->i_mime = -1;
  pstElem->s_mime = NULL;
  pstElem->resource_type = WE_ICON_RESOURCE_TYPE_PREDEFINED;
  pstElem->resource_str_id = NULL;
  pstElem->resource_int_id = 0;
  pstElem->next = NULL;

  return pstElem;
}

static void
insert_elem 
(
    we_cmmn_elem_t **ppList, 
    we_cmmn_elem_t *pstInsertElem
)
{
  we_cmmn_elem_t * p;

  if (pstInsertElem == NULL)
    return;

  p = *ppList;

  while (p != NULL){
    if (p->hash == pstInsertElem->hash)
    {
      if (strcmp(p->s_mime,pstInsertElem->s_mime) == 0)
      {
        
        return;
      }
    }
    p = p->next;
  }
  
  
  pstInsertElem->next = *ppList;
  *ppList = pstInsertElem;
}

static we_cmmn_elem_t*
frw_find_icon_by_str (const char * pcMimeType) 
{
  int i = 0;
  int n;
  int iStrLen;
  char *pcEnd;
  const char *p1,*p2;
  we_cmmn_elem_t *pstElem = NULL;

  if (pcMimeType == NULL)
    return NULL;

  pcEnd = strchr(pcMimeType,'/');

  if (pcEnd == NULL)
    return NULL;

  iStrLen = (pcEnd - pcMimeType);

  while (we_icon_mime_map_table[i].mime_type != NULL){

    p1 = we_icon_mime_map_table[i].mime_type;
    p2 = pcMimeType;
    for (n = 0; n < iStrLen; n++)
    {
      if (*p1++ != *p2++)
        goto next;
    }
    
    

    if(*p1 != '*'){
    
      p1++;
      p2++;
    }

    if (*p1 == '*'){
      
      if (pstElem == NULL){
        pstElem = get_new_elem ( );
        pstElem->hash = we_cmmn_strhash (pcMimeType, strlen (pcMimeType));
        pstElem->resource_type = we_icon_mime_map_table[i].resource_type;
        pstElem->resource_str_id = we_icon_mime_map_table[i].resource_str_id;
        pstElem->resource_int_id = we_icon_mime_map_table[i].resource_int_id;
        pstElem->s_mime = pcMimeType;
      }
    }
    else{
      while ((*p1 != '\0') && (*p2 != '\0')){
        if (*p1++ != *p2++)
          goto next;
      }
      if ((*p1 == '\0') && (*p2 == '\0'))
      {
        
        if (pstElem == NULL)
          pstElem = get_new_elem ( );

        pstElem->hash = we_cmmn_strhash (pcMimeType, strlen (pcMimeType));
        pstElem->resource_type = we_icon_mime_map_table[i].resource_type;
        pstElem->resource_str_id = we_icon_mime_map_table[i].resource_str_id;
        pstElem->resource_int_id = we_icon_mime_map_table[i].resource_int_id;
        pstElem->s_mime = pcMimeType;

        return pstElem;
      }
    }

next:
    i++;
  }

  return pstElem;
}





static void 
frw_cmmn_object_action 
(
    frw_object_action_t *p,
    WE_UINT8            uiAckDestModid,
    WE_UINT8            uiAckId
)
{
  we_action_op_t *pstOp;
  we_pck_result_t stResult;
  we_act_result_get_actions_t *pstGetResult;
  int              r;

  pstOp = get_new_op ();
  if (pstOp == NULL)
    return;

  pstOp->content.mime_type    = p->content.mime_type;
  pstOp->content.data_type    = p->content.data_type;
  pstOp->content.data_len     = p->content.data_len;
  pstOp->content.data         = p->content.data;
  pstOp->content.src_path     = p->content.src_path;
  pstOp->content.default_name = p->content.default_name;
  pstOp->content.content_type = p->content.content_type;
  pstOp->excl_act_strings     = p->excl_act_strings;
  pstOp->excl_act_strings_cnt = p->excl_act_strings_cnt;
  pstOp->ack_dest_modid       = uiAckDestModid;
  pstOp->ack_id               = uiAckId;
  pstOp->caller_owns_pipe     = p->content.src_path &&
                             (p->content.data_type == WeResourcePipe);

  if (we_act_hdl == NULL)
  {
    we_act_hdl = we_act_init (WE_MODID_FRW);
    if (we_act_hdl == NULL)
      return;
  }

  
  pstOp->pending_op_id = we_act_get_actions (we_act_hdl,
                                           pstOp->content.mime_type,
                                           pstOp->content.data_type,
                                           pstOp->content.content_type,
                                           0,
                                           (const char**) (pstOp->excl_act_strings),
                                           pstOp->excl_act_strings_cnt);

  if (pstOp->pending_op_id == WE_PACKAGE_ERROR)
  {
    handle_obj_act_err(pstOp);
    free_op (pstOp);
    return;
  }
  
  if ((r = we_act_get_result(we_act_hdl, pstOp->pending_op_id, &stResult)) != WE_PACKAGE_COMPLETED)
  {
    if (r == WE_PACKAGE_ERROR) {
      handle_obj_act_err(pstOp);
      free_op (pstOp);
    }

    return;
  }

  pstGetResult = (we_act_result_get_actions_t *)stResult._u.data;

  if (pstGetResult->result != WE_ACT_OK)
  {
    handle_obj_act_err(pstOp);
    free_op (pstOp);
    return;
  }
  
  pstOp->num_actions = pstGetResult->num_actions;

  if (pstOp->num_actions > 0)
  {
    pstOp->actions     = FRW_ALLOC( sizeof(we_act_action_entry_t) * pstGetResult->num_actions);
    memcpy (pstOp->actions, pstGetResult->action_list, sizeof(we_act_action_entry_t) * pstGetResult->num_actions);

    we_act_result_free(we_act_hdl, &stResult);
  
    we_cmmn_display_action_menu (pstOp);
  }
  else
  {
    we_act_result_free(we_act_hdl, &stResult);
  
    we_cmmn_display_no_action_dialog (pstOp);
  }
    
  FRW_FREE (p);

  return;
}


static void
frw_cmmn_get_icon_table (frw_get_icon_map_table_t *p)
{
  frw_get_icons_resp_t   *pstResp;
  we_dcvt_t              stCvtObj;
  WE_UINT16              lLength;
  void                   *pvBuffer;
  void                   *pvUserData;
  we_icon_list_t        *stIconList;
  int                     i;
  we_cmmn_elem_t         *pstElem      = NULL;
  we_cmmn_elem_t         *pstEelemList = NULL;
  int                     n_items = 0;

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                "WE: WE_SIG_GET_ICON_TABLE src_modId=%d, wid=%d", p->src, p->wid));

  if(p->n_items == 0) { 
    
    while (we_icon_mime_map_table[n_items++].mime_type != NULL);
    n_items--;
    
    stIconList = FRW_ALLOC (sizeof(we_icon_list_t)*n_items);
    
    for (i = 0;i < n_items; i++)
    {
      stIconList[i].mime_hash = we_cmmn_strhash (we_icon_mime_map_table[i].mime_type , strlen (we_icon_mime_map_table[i].mime_type));
      stIconList[i].s_mime = (char *)we_icon_mime_map_table[i].mime_type;
      stIconList[i].i_mime = -1;
      stIconList[i].resource_type = we_icon_mime_map_table[i].resource_type;
      stIconList[i].resource_str_id = (char *)we_icon_mime_map_table[i].resource_str_id;
      stIconList[i].resource_int_id = we_icon_mime_map_table[i].resource_int_id;
    }
  }
  else {
    for (i = 0; i < p->n_items; i++){
      pstElem  = frw_find_icon_by_str (p->mime_list[i]._u.s_value);
      if (pstElem != NULL){
        n_items++;
        insert_elem(&pstEelemList, pstElem);
      }
    }
    stIconList = FRW_ALLOC (sizeof(we_icon_list_t)*n_items);
    for (i = 0;i < n_items; i++)
    {
      stIconList[i].mime_hash = pstEelemList->hash;
      stIconList[i].s_mime = (char *)pstEelemList->s_mime;
      stIconList[i].i_mime = pstEelemList->i_mime;
      stIconList[i].resource_type = pstEelemList->resource_type;
      stIconList[i].resource_str_id = (char *)pstEelemList->resource_str_id;
      stIconList[i].resource_int_id = pstElem->resource_int_id;
      pstElem = pstEelemList;
      pstEelemList = pstElem->next;
      FRW_FREE (pstElem);
    }
  }

  pstResp = FRW_ALLOC (sizeof (frw_get_icons_resp_t));
  pstResp->wid = p->wid;
  pstResp->n_items = (WE_UINT16)n_items;
  pstResp->icon_list = stIconList;

  we_dcvt_init (&stCvtObj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_get_icons_resp (&stCvtObj, pstResp);
  lLength = (WE_UINT16)stCvtObj.pos;

  pvBuffer = WE_SIGNAL_CREATE (WE_SIG_GET_ICONS_RESPONSE, WE_MODID_FRW, p->src, lLength);
  if (pvBuffer != NULL){
    pvUserData = WE_SIGNAL_GET_USER_DATA (pvBuffer, &lLength);
    we_dcvt_init (&stCvtObj, WE_DCVT_ENCODE, pvUserData, lLength, p->src);
    we_cvt_get_icons_resp (&stCvtObj, pstResp);
    WE_SIGNAL_SEND (pvBuffer);
  }
  FRW_FREE (pstResp);
  FRW_FREE (stIconList);

  WE_SIGNAL_DESTRUCT (WE_MODID_FRW, WE_SIG_GET_ICONS, (void*)p);
}


static void
frw_cmmn_handle_widget_action 
(
    WE_UINT32 uiAction, 
    WE_UINT32 uiWindow
)
{
  we_action_op_t *pstOp = we_op_list;
  int i, iRes;
  we_pck_result_t stResult;

  while (pstOp != NULL) {
    if (frw_dlgGetDialogHandle(pstOp->window) == uiWindow)
      break;
    pstOp = pstOp->next;
  }

  if (pstOp == NULL)
    return;

  frw_dlgAction(pstOp->window, uiAction);
  if ((frw_dlgGetResponse(pstOp->window) == FRW_DLG_RESPONSE_POSITIVE) && (pstOp->num_actions > 0))
  {
    i = frw_dlgGetMenuIndex(pstOp->window);
    if ((i < 0) || (pstOp->state == WE_CMMN_ACT_STATE_OBJECT_ACTION_REQ))
      return;

    pstOp->state = WE_CMMN_ACT_STATE_OBJECT_ACTION_REQ;

    pstOp->pending_op_id = we_act_object_action (we_act_hdl,
                           pstOp->actions[i].action_id,
                           &(pstOp->content),
                           NULL,
                           0,
                           pstOp->screen,
                           (const char**) (pstOp->excl_act_strings),
                           pstOp->excl_act_strings_cnt);

    iRes = we_act_get_result(we_act_hdl, pstOp->pending_op_id, &stResult);
    we_cmmn_check_obj_act_result(pstOp, &iRes, &stResult);
    we_act_result_free(we_act_hdl, &stResult);

    if (iRes != WE_PACKAGE_BUSY){
      free_op(pstOp);
    }

  }
  else
  {
    
    handle_obj_act_err(pstOp);
    free_op(pstOp);
  }

}


static void
frw_cmmn_handle_act_finished(int wid)
{
  we_action_op_t *pstOp = we_op_list;
  we_pck_result_t stResult;
  int iRes;

  iRes = we_act_get_result(we_act_hdl, wid, &stResult);
  we_cmmn_check_obj_act_result(pstOp, &iRes, &stResult);
  we_act_result_free(we_act_hdl, &stResult);

  while (pstOp != NULL) {
    if (pstOp->pending_op_id == wid)
      break;
    pstOp = pstOp->next;
  }

  if (pstOp->pending_op_id == wid){
    
    free_op(pstOp);
  }
}


static void
frw_cmmn_main (frw_signal_t *pstSig)
{
  switch (pstSig->type) {
  case FRW_CMMN_SIG_OBJECT_ACTION:
    WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
      "FRW: received signal FRW_CMMN_SIG_OBJECT_ACTION\n"));
    frw_cmmn_object_action ((frw_object_action_t *)pstSig->p_param,
                            (WE_UINT8)pstSig->u_param1, (WE_UINT8)pstSig->u_param2);
    break;
  case FRW_CMMN_SIG_GET_ICONS:
    WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
      "FRW: received signal FRW_CMMN_SIG_GET_ICON_TABLE\n"));
    frw_cmmn_get_icon_table ((frw_get_icon_map_table_t *)pstSig->p_param);
    break;
  case FRW_CMMN_SIG_WIDGET_ACTION:
    frw_cmmn_handle_widget_action (pstSig->u_param1, pstSig->u_param2);
    break;
  case FRW_CMMN_SIG_ACT_FINISHED:
    frw_cmmn_handle_act_finished(pstSig->i_param);
    break;
  default:
    WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
      "FRW: received unknown signal: %d\n", pstSig->type));
  }
  frw_signal_delete (pstSig);
}







void
frw_cmmn_init (void)
{
  we_op_list = NULL;
  we_act_hdl = NULL;
  
  frw_signal_register_dst (FRW_SIG_DST_FRW_CMMN, frw_cmmn_main);
  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW, "FRW: CMMN - initialized\n"));

}

void
frw_cmmn_terminate (void)
{
  delete_op_list (&we_op_list);
  frw_signal_deregister (FRW_SIG_DST_FRW_CMMN);
  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW, "FRW: CMMN - terminated\n"));
}








int
frw_cmmn_handle_package_signal
(
    WE_UINT16 uiSignal, 
    void *p
)
{
  WE_INT32 wid;
  
  
  switch (we_act_handle_signal (we_act_hdl, uiSignal, p, &wid)) {
  case WE_PACKAGE_SIGNAL_HANDLED:
    
    return TRUE;
    break;
  case WE_PACKAGE_SIGNAL_NOT_HANDLED:
    
    break;
  case WE_PACKAGE_OPERATION_COMPLETE:
    FRW_SIGNAL_SENDTO_I (FRW_SIG_DST_FRW_CMMN, FRW_CMMN_SIG_ACT_FINISHED, wid);
    return TRUE;
  }
  return FALSE;
}







void
frw_content_send_ack 
(
    WE_UINT8 uiModId, 
    WE_UINT8 wid, 
    WE_UINT8 uiStatus,
    WE_BOOL bCallerOwnsPipe
)
{
  we_content_send_ack_t  stSendAck;
  we_dcvt_t              stCvtObj;
  void                    *pvBuffer;
  void                    *pvUserData;
  WE_UINT16              uiLength;

  stSendAck.wid = wid;
  stSendAck.status = uiStatus;
  stSendAck.caller_owns_pipe = bCallerOwnsPipe;
  we_dcvt_init (&stCvtObj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_content_send_ack (&stCvtObj, &stSendAck);
  uiLength = (WE_UINT16)stCvtObj.pos;

  pvBuffer = WE_SIGNAL_CREATE (WE_SIG_CONTENT_SEND_ACK, WE_MODID_FRW, uiModId, uiLength);
  if (pvBuffer != NULL){
    pvUserData = WE_SIGNAL_GET_USER_DATA (pvBuffer, &uiLength);
    we_dcvt_init (&stCvtObj, WE_DCVT_ENCODE, pvUserData, uiLength, WE_MODID_FRW);
    we_cvt_content_send_ack (&stCvtObj, &stSendAck);
    WE_SIGNAL_SEND (pvBuffer);
  }
}


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
 * Frw_Env.c
 *
 * Created by Ingmar Persson, 
 *
 * Revision history:
 *   021014  IPN  Added FRWc_kill.
 *   021014  IPN  Added memory exception functionality.
 *   021016  IPN  FRWc_externalResponse
 *
 */

#include "Frw_Env.h"
#include "Frw.h"
#include "Frw_Sig.h"
#include "Frw_Time.h"
#include "Frw_Reg.h"
#include "Frw_Cmmn.h"
#include "Frw_Int.h"

#include "We_Cfg.h"
#include "We_Core.h"
#include "We_Mem.h"
#include "We_Lib.h"
#include "We_Log.h"
#include "We_Errc.h"
#include "We_File.h"


static void
frw_receive 
(
    WE_UINT8 uiSrcModule, 
    WE_UINT16 uiSignal, 
    void* p
);


void
FRWc_startMobileSuite 
(
    const char* pcStartOptions, 
    const int *piStaticModules, 
    int iNumberOfStaticModules
)
{
  WE_LOG_FC_BEGIN(FRWc_startMobileSuite)
  WE_LOG_FC_PRE_IMPL

  frw_start (pcStartOptions, piStaticModules, iNumberOfStaticModules);
  WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_FRW, "FRW: FRWc_startMobileSuite\n"));

  WE_LOG_FC_END
}

void
FRWc_terminateMobileSuite (void)
{
  WE_LOG_FC_BEGIN(FRWc_terminateMobileSuite)
  WE_LOG_FC_PRE_IMPL

  FRW_SIGNAL_SENDTO(FRW_SIG_DST_FRW_MAIN, FRW_MAIN_SIG_TERMINATE);
  WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_FRW, "FRW: FRWc_terminateMobileSuite\n"));

  WE_LOG_FC_END
}

int
FRWc_wantsToRun (void)
{
  int iReturnVal = 0;
  WE_LOG_FC_BEGIN(FRWc_wantsToRun)
  WE_LOG_FC_PRE_IMPL

  iReturnVal = (WE_SIGNAL_QUEUE_LENGTH (WE_MODID_FRW) > 0 ) ||
            !frw_signal_queue_is_empty ();

  WE_LOG_FC_INT32(iReturnVal, NULL);
  WE_LOG_FC_END

  return iReturnVal;
}

void
FRWc_run (void)
{
  WE_LOG_FC_BEGIN(FRWc_run)
  WE_LOG_FC_PRE_IMPL

    /* Set the global variable, the manager is now inside the Run function. */
  frw_inside_run = 1;
    /* Make a setjmp, so the manager can jump back to this point if the
       memory runs out. */
  if (setjmp (frw_jmpbuf) == 0) {
    /*
     * Get external signals and process one internal signal.
     */
    while (WE_SIGNAL_RETRIEVE (WE_MODID_FRW, frw_receive));
    frw_signal_process ();
  }
  else {
    /*
     * A longjmp from frw_exception_handler. If we are here there is no more
     * memory, and the situation could not be worse.
     * Send a fatal error, the resources is released later by the FRWc_kill
     * function.
     */
    WE_ERROR (WE_MODID_FRW, WE_ERR_SYSTEM_FATAL);
  }
    /* Clear the global variable, the manager is now outside the Run function. */
  frw_inside_run = 0;

  WE_LOG_FC_END
}

void
FRWc_kill (void)
{
  WE_FILE_CLOSE_ALL (WE_MODID_FRW);
  WE_MEM_FREE_ALL (WE_MODID_FRW);
  WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_FRW, "FRW: FRWc_kill\n"));
  FRWa_terminated ();
}

void
FRWc_startModule 
(
    WE_UINT8 uiModId, 
    char* pcStartOptions
)
{
  we_module_start_t* pstCmd;

  WE_LOG_FC_BEGIN(FRWc_startModule)
  WE_LOG_FC_UINT8(uiModId, NULL)
  WE_LOG_FC_STRING(pcStartOptions, NULL)
  WE_LOG_FC_PRE_IMPL

  WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_FRW, "FRW: FRWc_startModule modId=%d startOptions=%s\n",
                                                   uiModId, (pcStartOptions ? pcStartOptions : "(null)")));
  pstCmd = FRW_ALLOCTYPE (we_module_start_t);
  if (pstCmd != NULL) {
    pstCmd->modId = uiModId;
    pstCmd->contentDataExists = 0;
    pstCmd->contentData = NULL;

    if (pcStartOptions != NULL){
      pstCmd->startOptions = FRW_ALLOC (strlen (pcStartOptions) + 1);
      if (pstCmd->startOptions != NULL)
        strcpy (pstCmd->startOptions, pcStartOptions);
      else
        pstCmd->startOptions = NULL;
    }
    else
      pstCmd->startOptions = NULL;
    FRW_SIGNAL_SENDTO_IP (FRW_SIG_DST_FRW_MAIN, FRW_MAIN_SIG_START_MODULE, WE_MODID_FRW, pstCmd);
  }

  WE_LOG_FC_END
}

void
FRWc_terminateModule (WE_UINT8 uiModId)
{
  WE_LOG_FC_BEGIN(FRWc_terminateModule)
  WE_LOG_FC_UINT8(uiModId, NULL)
  WE_LOG_FC_PRE_IMPL

  WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_FRW, "FRW: FRWc_terminateModule modId=%d\n", uiModId));
  FRW_SIGNAL_SENDTO_IU (FRW_SIG_DST_FRW_MAIN, FRW_MAIN_SIG_TERMINATE_MODULE,
                        uiModId, WE_MODID_FRW);

  WE_LOG_FC_END
}

void
FRWc_timerExpiredEvent (void)
{
  WE_LOG_FC_BEGIN(FRWc_timerExpiredEvent)
  WE_LOG_FC_PRE_IMPL

  WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_FRW, "FRW: FRWc_timerExpiredEvent\n"));
  FRW_SIGNAL_SENDTO (FRW_SIG_DST_FRW_TIMER, FRW_TIMER_SIG_EXPIRED);

  WE_LOG_FC_END
}
void 
FRWc_persistentTimerExpiredEvent (void)
{
  WE_LOG_FC_BEGIN(FRWc_persistentTimerExpiredEvent)
  WE_LOG_FC_PRE_IMPL

  WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_FRW, "FRW: FRWc_persistentTimerExpiredEvent\n"));
  FRW_SIGNAL_SENDTO (FRW_SIG_DST_FRW_TIMER, FRW_TIMER_SIG_PERSISTENT_EXPIRED);

  WE_LOG_FC_END
}


/****************************************
 * Registry
 ****************************************/

void
FRWc_registrySetInit (void)
{
  WE_LOG_FC_BEGIN(FRWc_registrySetInit)
  WE_LOG_FC_PRE_IMPL

  WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_FRW, "FRW: FRWc_registrySetInit\n"));
  frw_reg_int_set_init ();

  WE_LOG_FC_END
}

void
FRWc_registrySetPath (char* pcPath)
{
  WE_LOG_FC_BEGIN(FRWc_registrySetPath)
  WE_LOG_FC_STRING(pcPath, NULL)
  WE_LOG_FC_PRE_IMPL

  WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_FRW, "FRW: FRWc_registrySetPath path=%s\n",
                                                      (pcPath ? pcPath : "(null)")));
  frw_reg_int_set_path (pcPath);

  WE_LOG_FC_END
}

void
FRWc_registrySetAddKeyInt 
(
    char* pcKey, 
    WE_INT32 iValue
)
{
  WE_LOG_FC_BEGIN(FRWc_registrySetAddKeyInt)
  WE_LOG_FC_STRING(pcKey, NULL)
  WE_LOG_FC_INT32(iValue, NULL)
  WE_LOG_FC_PRE_IMPL

  WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_FRW, "FRW: FRWc_registrySetAddKeyInt name=%s value=%ld\n",
                                                      (pcKey ? pcKey : "(null)"), iValue));
  frw_reg_int_set_add_key_int (pcKey, iValue);

  WE_LOG_FC_END
}

void
FRWc_registrySetAddKeyStr 
(
    char* pcKey, 
    unsigned char* pcValue, 
    WE_UINT16 uiValueLength
)
{
  WE_LOG_FC_BEGIN(FRWc_registrySetAddKeyStr)
  WE_LOG_FC_STRING(pcKey, NULL)
  WE_LOG_FC_UINT16(uiValueLength, NULL)
  WE_LOG_FC_BYTES(pcValue, uiValueLength, NULL)
  WE_LOG_FC_PRE_IMPL

  WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_FRW, "FRW: FRWc_registrySetAddKeyStr name=%s valueLength=%d\n",
                                                      (pcKey ? pcKey : "(null)"), uiValueLength));
  frw_reg_int_set_add_key_str (pcKey, pcValue, uiValueLength);

  WE_LOG_FC_END
}

void
FRWc_registrySetCommit (void)
{
  WE_LOG_FC_BEGIN(FRWc_registrySetCommit)
  WE_LOG_FC_PRE_IMPL

  WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_FRW, "FRW: FRWc_registrySetCommit\n"));
  frw_reg_int_set_commit ();

  WE_LOG_FC_END
}

void
FRWc_registryGet 
(
    WE_UINT8 wid, 
    char* pcPath, 
    char* pcKey
)
{
  we_registry_identifier_t*  p;
  WE_LOG_FC_BEGIN(FRWc_registryGet)
  WE_LOG_FC_STRING(pcPath, NULL)
  WE_LOG_FC_STRING(pcKey, NULL)
  WE_LOG_FC_PRE_IMPL

  WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_FRW, "FRW: FRWc_registryGet path=%s key=%s\n",
                                                      (pcPath ? pcPath : "(null)"), (pcKey ? pcKey : "(null)")));
  if (pcPath != NULL) {
    p = FRW_ALLOCTYPE (we_registry_identifier_t);
    if (p != NULL) {
      p->path = FRW_ALLOC (strlen (pcPath) + 1);
      if (p->path != NULL) {
        strcpy (p->path, pcPath);
        if (pcKey != NULL && strlen (pcKey) > 0) {
          p->key = FRW_ALLOC (strlen (pcKey) + 1);
          if (p->key != NULL)
            strcpy (p->key, pcKey);
        }
        else
          p->key = NULL;

        FRW_SIGNAL_SENDTO_IUP (FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_GET,
                               WE_REG_MODID_INTEGRATION, wid, p);
      }
    }
  }
  WE_LOG_FC_END
}

void
FRWc_registryDelete 
(
    char* pcPath, 
    char* pcKey
)
{
  we_registry_identifier_t*  p;
  WE_LOG_FC_BEGIN(FRWc_registryDelete)
  WE_LOG_FC_STRING(pcPath, NULL)
  WE_LOG_FC_STRING(pcKey, NULL)
  WE_LOG_FC_PRE_IMPL

  WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_FRW, "FRW: FRWc_registryDelete path=%s key=%s\n",
                                                      (pcPath ? pcPath : "(null)"), (pcKey ? pcKey : "(null)")));
  if (pcPath != NULL)
  {
    p = FRW_ALLOCTYPE (we_registry_identifier_t);
    if (p != NULL) {
      p->path = FRW_ALLOC (strlen (pcPath) + 1);
      if (p->path != NULL) {
        strcpy (p->path, pcPath);
        if (pcKey != NULL && strlen (pcKey) > 0) {
          p->key = FRW_ALLOC (strlen (pcKey) + 1);
          if (p->key != NULL)
            strcpy (p->key, pcKey);
        }
        else
          p->key = NULL;

        FRW_SIGNAL_SENDTO_IP (FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_DELETE, WE_MODID_FRW, p);
      }
    }
  }
  WE_LOG_FC_END
}


void
FRWc_registrySubscribe 
(
    WE_UINT8 wid, 
    const char* pcPath, 
    const char* pcKey, 
    WE_BOOL bAdd
)
{
  we_registry_subscription_t*  p;
  WE_LOG_FC_BEGIN(FRWc_registrySubscribe)
  WE_LOG_FC_PRE_IMPL

  WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_FRW, "FRW: FRWc_registrySubscribe path=%s key=%s\n",
                                                      (pcPath ? pcPath : "(null)"), (pcKey ? pcKey : "(null)")));
  if (pcPath != NULL)
  {
    p = FRW_ALLOCTYPE (we_registry_subscription_t);
    if (p != NULL) {
      p->wid = (WE_UINT16) (0x00FF & wid);
      p->add = bAdd;
      p->path = FRW_ALLOC (strlen (pcPath) + 1);
      if (p->path != NULL) {
        strcpy (p->path, pcPath);
        if (pcKey != NULL && strlen (pcKey) > 0) {
          p->key = FRW_ALLOC (strlen (pcKey) + 1);
          if (p->key != NULL)
            strcpy (p->key, pcKey);
        }
        else
          p->key = NULL;

        FRW_SIGNAL_SENDTO_IP (FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_SUBSCRIBE,
                              WE_REG_MODID_INTEGRATION, p);
      }
    }
  }
  WE_LOG_FC_END
}

int
FRWc_registryRespGetNext 
(
    WE_UINT8 wid, 
    we_registry_param_t* pstParam
)
{
  int iReturnVal;
  WE_LOG_FC_BEGIN(FRWc_registryRespGetNext)
  WE_LOG_FC_UINT8(wid, NULL)
  WE_LOG_FC_PRE_IMPL

  WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_FRW, "FRW: FRWc_registryRespGetNext\n"));
  iReturnVal = frw_reg_int_resp_get_next (wid, pstParam);

  #ifdef WE_LOG_FC
  if(iReturnVal)
  {
    WE_LOG_FC_STRING(pstParam->key, NULL)
    WE_LOG_FC_STRING(pstParam->path, NULL)
    WE_LOG_FC_UINT8(pstParam->type, NULL)
    WE_LOG_FC_UINT16(pstParam->value_bv_length, NULL)
    WE_LOG_FC_BYTES(pstParam->value_bv, pstParam->value_bv_length, NULL)
    WE_LOG_FC_INT32(pstParam->value_i, NULL)
  }
  #endif
  WE_LOG_FC_INT(iReturnVal, NULL)
  WE_LOG_FC_END

  return iReturnVal; 
}

void
FRWc_registryRespFree (WE_UINT8 wid)
{
  WE_LOG_FC_BEGIN(FRWc_registryRespFree)
  WE_LOG_FC_UINT8(wid, NULL)
  WE_LOG_FC_PRE_IMPL

  WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_FRW, "FRW: FRWc_registryRespFree\n"));
  frw_reg_int_resp_free (wid);

  WE_LOG_FC_END
}

static void
frw_env_cvt_registry_param 
(
    we_dcvt_t *stObj, 
    we_registry_param_t* pstParam
)
{
  WE_UINT8 iType;

  if (pstParam != NULL && pstParam->path != NULL && pstParam->key != NULL) {
    iType = WE_REG_SET_TYPE_PATH;
    we_dcvt_uint8 (stObj, &iType);
    we_dcvt_string (stObj, &(pstParam->path));
    if (pstParam->type == WE_REGISTRY_TYPE_STR) {
      iType = WE_REG_SET_TYPE_STR;
      we_dcvt_uint8 (stObj, &iType);
      we_dcvt_string (stObj, &(pstParam->key));
      we_dcvt_uint16 (stObj, &(pstParam->value_bv_length));
      we_dcvt_uchar_vector (stObj, pstParam->value_bv_length, &(pstParam->value_bv));
    }
    else {
      iType = WE_REG_SET_TYPE_INT;
      we_dcvt_uint8 (stObj, &iType);
      we_dcvt_string (stObj, &(pstParam->key));
      we_dcvt_int32 (stObj, &(pstParam->value_i));
    }
  }
  else {  /* Act as a dummy value. */
    iType = 0;
    we_dcvt_uint8 (stObj, &iType);
  }
}

void
FRWc_externalResponse 
(
    WE_INT32 wid, 
    we_registry_param_t* pstParam
)
{
  we_dcvt_t  stCvtObj;
  void        *pvBuffer; 
  void        *pvUser_data;
  WE_UINT16  uiLength = 0;
  WE_UINT8   uiModId = (WE_UINT8)((wid & 0xFF0000) >> 16);
  WE_INT16   iReqId = (WE_INT16)(wid & 0xFFFF);

  WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_FRW, "FRW: FRWc_externalResponse\n"));

  we_dcvt_init (&stCvtObj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  frw_env_cvt_registry_param (&stCvtObj, pstParam);
  uiLength = (WE_UINT16)(stCvtObj.pos + 2);

  pvBuffer = WE_SIGNAL_CREATE (WE_SIG_REG_RESPONSE, WE_MODID_FRW, uiModId, uiLength);
  if (pvBuffer != NULL){
    pvUser_data = WE_SIGNAL_GET_USER_DATA (pvBuffer, &uiLength);
    we_dcvt_init (&stCvtObj, WE_DCVT_ENCODE, pvUser_data, uiLength, (WE_UINT8)uiModId);
    we_dcvt_int16 (&stCvtObj, &iReqId);
    frw_env_cvt_registry_param (&stCvtObj, pstParam);
    WE_SIGNAL_SEND (pvBuffer);
  }
}


void
FRWc_objectAction 
(
    const we_act_content_t* pstContent,
    const char*              pcExclActStrings[],
    WE_INT32                iExclActStringsCnt
)
{
  WE_LOG_FC_BEGIN(FRWc_objectAction)
  WE_LOG_FC_PTR(pstContent, NULL)

  if (!pstContent)
    return;

  WE_LOG_FC_STRING(pstContent->mime_type, NULL)
  WE_LOG_FC_INT32(pstContent->data_type, NULL)
  WE_LOG_FC_INT32(pstContent->data_len, NULL)
  WE_LOG_FC_BYTES(pstContent->data, pstContent->data_len, NULL)
  WE_LOG_FC_STRING(pstContent->src_path, NULL)
  WE_LOG_FC_STRING(pstContent->default_name, NULL)
  WE_LOG_FC_PTR(pcExclActStrings, NULL)
  WE_LOG_FC_INT32(iExclActStringsCnt, NULL)
  WE_LOG_FC_PRE_IMPL
  
  WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_FRW, "FRW: FRWc_objectAction src_path=%s mime_type=%s\n", 
    (pstContent->src_path ? pstContent->src_path : "(null)"), (pstContent->mime_type ? pstContent->mime_type : "(null)")));

  frw_object_action (pstContent, pcExclActStrings, iExclActStringsCnt,
                     /*ack_dest_modid*/ 0, /*ack_id*/ 0);

  WE_LOG_FC_END
}


static void
frw_receive 
(
    WE_UINT8 uiSrcModule, 
    WE_UINT16 uiSignal, 
    void* p
)
{
/*  if (frw_handle_package_signal(signal, p)){
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_FRW,
                "FRW: Signal=%u handled by package\n", signal));
    FRW_FREE (p);
    return;
  }*/
  if (frw_cmmn_handle_package_signal(uiSignal, p)){
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_FRW,
                "FRW_TMP: Signal=%u handled by package\n", uiSignal));
    FRW_FREE (p);
    return;
  }

  switch (uiSignal){
    case WE_SIG_MODULE_START:
        FRW_SIGNAL_SENDTO_IP (FRW_SIG_DST_FRW_MAIN, FRW_MAIN_SIG_START_MODULE, uiSrcModule, p);
      break;
    case WE_SIG_MODULE_TERMINATE:
      {
        we_module_terminate_t* pstStr = (we_module_terminate_t*)p;
        FRW_SIGNAL_SENDTO_IU (FRW_SIG_DST_FRW_MAIN, FRW_MAIN_SIG_TERMINATE_MODULE,
                              pstStr->modId, uiSrcModule);
        FRW_FREE (p);
      }
      break;
    case WE_SIG_MODULE_STATUS_NOTIFY:
      {
        we_module_status_t* pstStr = (we_module_status_t*)p;
        FRW_SIGNAL_SENDTO_IUP (FRW_SIG_DST_FRW_MAIN, FRW_MAIN_SIG_MODULE_STATUS_NOTIFY,
                              pstStr->status, pstStr->modId, pstStr->modVersion);
        FRW_FREE (p);
      }
      break;
    case WE_SIG_MODULE_STATUS:
      {
        we_module_status_t* pstStr = (we_module_status_t*)p;
        FRW_SIGNAL_SENDTO_IUP (FRW_SIG_DST_FRW_MAIN, FRW_MAIN_SIG_MODULE_STATUS,
                              pstStr->status, pstStr->modId, pstStr->modVersion);
        FRW_FREE (p);
      }
      break;
    case WE_SIG_TIMER_SET:
      {
        we_timer_set_t* pstStr = (we_timer_set_t*)p;
        FRW_SIGNAL_SENDTO_IUU (FRW_SIG_DST_FRW_TIMER, FRW_TIMER_SIG_SET,
                               uiSrcModule, pstStr->timerID, pstStr->timerInterval);
        FRW_FREE (p);
      }
      break;
    case WE_SIG_TIMER_RESET:
      {
        we_timer_reset_t* pstStr = (we_timer_reset_t*)p;
        FRW_SIGNAL_SENDTO_IU (FRW_SIG_DST_FRW_TIMER, FRW_TIMER_SIG_RESET,
                              uiSrcModule, pstStr->timerID);
        FRW_FREE (p);
      }
      break;
    case WE_SIG_TIMER_SET_PERSISTENT:
      {
        we_timer_set_t* pstStr = (we_timer_set_t*)p;
        FRW_SIGNAL_SENDTO_IUU (FRW_SIG_DST_FRW_TIMER, FRW_TIMER_SIG_SET_PERSISTENT,
                               uiSrcModule, pstStr->timerID, pstStr->timerInterval);
        FRW_FREE (p);
      }
      break;
    case WE_SIG_TIMER_RESET_PERSISTENT:
      {
        we_timer_reset_t* pstStr = (we_timer_reset_t*)p;
        FRW_SIGNAL_SENDTO_IU (FRW_SIG_DST_FRW_TIMER, FRW_TIMER_SIG_RESET_PERSISTENT,
                              uiSrcModule, pstStr->timerID);
        FRW_FREE (p);
      }
      break;
    case WE_SIG_FILE_NOTIFY:
      {
        frw_file_notify (((we_file_notify_t *)p)->fileHandle);
        FRW_FREE (p);
      }
      break;
    case WE_SIG_REG_SET:
      FRW_SIGNAL_SENDTO_IP (FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_SET, uiSrcModule, p);
      break;
    case WE_SIG_REG_GET:
      FRW_SIGNAL_SENDTO_IP (FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_GET, uiSrcModule, p);
      break;
    case WE_SIG_REG_DELETE:
      FRW_SIGNAL_SENDTO_IP (FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_DELETE, uiSrcModule, p);
      break;
    case WE_SIG_REG_SUBSCRIBE:
      FRW_SIGNAL_SENDTO_IP (FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_SUBSCRIBE, uiSrcModule, p);
      break;
    case WE_SIG_REG_RESPONSE:
      FRW_SIGNAL_SENDTO_IP (FRW_SIG_DST_FRW_MAIN, FRW_MAIN_SIG_REG_RESPONSE, uiSrcModule, p);
      break;
    case WE_SIG_CONTENT_SEND:
      FRW_SIGNAL_SENDTO_IP (FRW_SIG_DST_FRW_MAIN, FRW_MAIN_SIG_CONTENT_SEND, uiSrcModule, p);
      break;
    case WE_SIG_WIDGET_ACTION:
      FRW_SIGNAL_SENDTO_UU (FRW_SIG_DST_FRW_CMMN, FRW_CMMN_SIG_WIDGET_ACTION,
          ((we_widget_action_t *)p)->action,((we_widget_action_t *)p)->handle);
      FRW_FREE (p);
      break;
    case WE_SIG_GET_ICONS:
      FRW_SIGNAL_SENDTO_P (FRW_SIG_DST_FRW_CMMN, FRW_CMMN_SIG_GET_ICONS, p);
      break;
    case WE_SIG_OBJECT_ACTION :
      FRW_SIGNAL_SENDTO_P (FRW_SIG_DST_FRW_CMMN, FRW_CMMN_SIG_OBJECT_ACTION, p);
      break;
    default:
      WE_SIGNAL_DESTRUCT (WE_MODID_FRW, uiSignal, p);
      break;
  }
}



#ifdef WE_LOG_FC 
WE_LOG_FC_DISPATCH_MAP_BEGIN(FRW)

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(FRWc_startMobileSuite)
  FRWc_startMobileSuite(NULL,NULL,0);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(FRWc_terminateMobileSuite)
  FRWc_terminateMobileSuite();
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(FRWc_wantsToRun)
  FRWc_wantsToRun();
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(FRWc_run)
  FRWc_run();
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(FRWc_timerExpiredEvent)
  FRWc_timerExpiredEvent();
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(FRWc_terminateModule)
  FRWc_terminateModule(1);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(FRWc_startModule)
  char startOptions[1024] = "";

  FRWc_startModule(1, startOptions);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(FRWc_registrySetInit)
   FRWc_registrySetInit ();
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(FRWc_registrySetPath)
   char path[1024] = "";

   FRWc_registrySetPath (path);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(FRWc_registrySetAddKeyInt)
   char key[1024] = "";
   WE_INT32 value = 0;

   FRWc_registrySetAddKeyInt (key, value);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(FRWc_registrySetAddKeyStr)
   char key[1024] = "";
   unsigned char value[1024];
   WE_UINT16 valueLength = 0;
   memset(value, 0x00, sizeof(value));

   FRWc_registrySetAddKeyStr (key, value, valueLength);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(FRWc_registrySetCommit)
   FRWc_registrySetCommit ();
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(FRWc_registryGet)
   WE_UINT8 wid = 0;
   char path[1024] = "";
   char key[1024] = "";

   FRWc_registryGet (wid, path, key);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(FRWc_registryDelete)
   char path[1024] = "";
   char key[1024] = "";

   FRWc_registryDelete (path, key);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(FRWc_registrySubscribe)
   WE_UINT8 wid = 0;
   char path[1024] = "";
   char key[1024] = "";

   FRWc_registrySubscribe (wid, path, key, TRUE);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(FRWc_registryRespGetNext)
   WE_UINT8 wid = 0;
   we_registry_param_t param;
   memset(&param, 0x00, sizeof(param));

   FRWc_registryRespGetNext (wid, &param);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(FRWc_registryRespFree)
   WE_UINT8 wid = 0;

   FRWc_registryRespFree (wid);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(FRWc_objectAction)
  char          mime_type[1024] = "";
  unsigned char data[1024] = "";
  char          src_path[1024] = "";
  char          default_name[1024] = "";
  char          content_type[1024] = "";
  char*         excl_act_strings[4] = {"", "", "", ""};
  WE_INT32     excl_act_strings_cnt = 0;
  we_act_content_t content;

  content.mime_type    = mime_type;
  content.data_type    = WeResourceFile;
  content.data_len     = 0;
  content.data         = data;
  content.src_path     = src_path;
  content.default_name = default_name;
  content.content_type = content_type;

  FRWc_objectAction (&content, excl_act_strings, excl_act_strings_cnt);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_END
#endif

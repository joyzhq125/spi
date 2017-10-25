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


#include "Frw.h"
#include "Frw_Int.h"
#include "Frw_Sig.h"
#include "Frw_Time.h"
#include "Frw_Reg.h"
#include "Frw_Cmmn.h"
#include "Frw_Vrsn.h"

#include "We_Mem.h"
#include "We_Lib.h"
#include "We_Cmmn.h"
#include "We_Log.h"
#include "We_File.h"
#include "We_Pipe.h"

#include "We_Pck.h"
#include "We_Afi.h"
#include "We_Url.h"




#define FRW_CONTENT_ROUTING_PATH         "/FRW/RT_DB"
#define FRW_REG_KEY_ROUTING_MIME          "MimeType"
#define FRW_REG_KEY_ROUTING_EXT            "MimeExt"
#define FRW_REG_KEY_ROUTING_PAPPID_N  "PAppId_n"
#define FRW_REG_KEY_ROUTING_PAPPID_S  "PAppId_s"
#define FRW_REG_KEY_ROUTING_SCHEME     "Scheme"

#define FRW_UF_STATE_IDLE             1
#define FRW_UF_STATE_MKDIR_DELAYED    2

#define FRW_MAX_FILES                 5
#define FRW_FILE_NOT_USED            -1

#define WE_STARTUP_STATIC       0
#define WE_STARTUP_DYNAMIC      1



typedef struct frw_command_data_st{
  struct frw_command_data_st  *next;
  char                        *startOptions;      

  WE_UINT8                   startOptions_user;  

  we_content_data_t          *content_data;      

  WE_UINT8                   sendAck;            
  WE_UINT8                   wid;                 
} frw_command_data_t;

typedef struct {
  int                 status;           
  char                startup_policy;   
  char                *version;         
  int                 nr_of_users;      
  WE_UINT8           *users;           
  int                 len_users;        
  char                *mime_type;       
  char                *mime_ext;        
  char                *papp_id_number;  
  char                *papp_id_string;  
  char                *scheme;          
  int                 abort_user;       
  frw_command_data_t  *command_data;    
} frw_module_t;

typedef struct {
  const char  startup_policy_depr;
  const char  *short_name;
} frw_cfg_t;

typedef struct {
  int file;
  int resp_dst;
  int resp_sig;
} frw_file_handle_list_t;
 
typedef struct frw_mod_user_int_st {
  int                         resp_dst;
  int                         resp_sig;
  struct frw_mod_user_int_st *next;
} frw_mod_user_int_t;

typedef struct frw_mod_notify_int_st {
  int                            modid;
  frw_mod_user_int_t            *users;
  struct frw_mod_notify_int_st  *next;
} frw_mod_notify_int_t;



jmp_buf frw_jmpbuf;     
int     frw_inside_run; 


int frw_closing;

static int frw_sub_module_delayed; 
static frw_file_handle_list_t frw_files[FRW_MAX_FILES];

static frw_mod_notify_int_t *frw_started_modules_int;  

static frw_mod_notify_int_t *frw_notify_term_int;      


static frw_module_t frw_module_info[WE_NUMBER_OF_MODULES];
frw_cfg_t frw_modules_startup[WE_NUMBER_OF_MODULES] = WE_MODULES_STARTUP; 


static void
frw_init_done (void);

static void
frw_complete_exit (void);

void
frw_init_module_info 
(
    const int aiStaticModules[], 
    int iNumberOfStaticModules
);

static void
frw_start_module_list_destroy (frw_mod_notify_int_t **list);

static void
frw_start_static_modules (void);

static void
frw_start_module 
(
    WE_UINT8 uiModId, 
    WE_UINT8 uiUserModId, 
    char** ppcStartOptions,
    we_content_data_t** ppstContentData, 
    WE_UINT8* puiContentDataExist,
    int iContentSend, 
    WE_UINT8 uiContentSendAck, 
    WE_UINT8 uiContentId
);

static void
frw_get_content_routing_data (void);

static void
frw_subscribe_content_routing_data (void);

static char *
frw_str_dup_skip_blanks (char *s);

void
frw_module_status 
(
    WE_UINT8 uiModid, 
    int iStatus, 
    char* pcVersion
);

static void
frw_main (frw_signal_t *sig);

static void
frw_exception_handler (void);

static void
frw_init_file_list (void);

static void
frw_mod_user_notify_int 
(
    frw_mod_notify_int_t **list, 
    WE_UINT8 uiModid
);

static int
frw_create_folders (const char *path)
{
  char       *pcStop;
  char       *p;
  int         iRes = 0;
  int         iLen;
  
  iLen = strlen (path);
  p = FRW_ALLOC (iLen + 1);
  
  if (p == NULL)
    goto error;
  
  strcpy(p, path);

  if (p[iLen - 1] == '/')
    p[iLen - 1] = '\0';

  pcStop = p;

  do {
    pcStop  = strchr(pcStop + 1, '/');

    if (pcStop != NULL)
      *pcStop = '\0';

    iRes   = WE_FILE_MKDIR(p);
    
    if (!((iRes == WE_FILE_ERROR_EXIST) || (iRes == WE_FILE_OK)))
      goto error;
    
    if (pcStop != NULL)
      *pcStop = '/'; 
    
  }  while(pcStop != NULL);
  
  FRW_FREE (p);
  return TRUE;
  
error:
  FRW_FREE (p);
  WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_FRW,
                "FRW: User Root Folder Creation Failed : err %i\n",
                iRes));

  return FALSE;
}


static void
frw_create_user_folders (void)
{
  we_pck_root_folders_t  *pstRootResult;
  we_pck_user_folders_t  *pstUserResult;
  int                     i;

  we_pck_get_all_root_folders (WE_MODID_FRW, &pstRootResult);

  if (!pstRootResult) {
    return;
  }

  for (i = 0; i < pstRootResult->n_rf; i++) {
    frw_create_folders (pstRootResult->rf[i].pathname);
  }
  FRW_FREE (pstRootResult);

  we_pck_get_all_user_folders (WE_MODID_FRW, &pstUserResult);
  if (!pstUserResult) {
    return;
  }

  for (i = 0; i < pstUserResult->n_uf; i++) {
    frw_create_folders (pstUserResult->uf[i].pathname);
  }
  FRW_FREE (pstUserResult);

}


typedef struct frw_dir_stack_st
{
  int                      path_length;
  int                      name_length;
  struct frw_dir_stack_st *next;
} frw_dir_stack_t;


static void
frw_create_tmp_folder (void)
{
  int iSize;
  int r;
  long lLength;
  int iType;
  char acPath[63];
  int iPathLength;
  frw_dir_stack_t *pstStack = NULL;
  frw_dir_stack_t *p; 

  frw_create_folders (WE_CFG_TEMPORARY_FOLDER_PATH);
  
  iSize = TPIa_fileGetSizeDir(WE_CFG_TEMPORARY_FOLDER_PATH);

  if (iSize < 1 )
    return;

  strcpy (acPath, WE_CFG_TEMPORARY_FOLDER_PATH);
  iPathLength = strlen (WE_CFG_TEMPORARY_FOLDER_PATH);
  if (acPath[iPathLength - 1] != '/'){
    acPath[iPathLength] = '/';
    iPathLength++;
    acPath[iPathLength] = '\0';
  }

  for (;;)
  {
    if (TPIa_fileGetSizeDir(acPath) < 1)
    {
      if (pstStack == NULL)
        break;

      p = pstStack;
      pstStack = p->next;

      --iPathLength; 

      if (iPathLength > 1)
        while (acPath[--iPathLength] != '/');

      iPathLength += 1;

      acPath[iPathLength] = '\0';

      FRW_FREE (p);
      continue;
    }

    r = TPIa_fileReadDir(acPath, 0, &acPath[iPathLength], 63 - iPathLength, &iType, &lLength);

    if (r != WE_FILE_OK)
      return;
    
    if (iType == TPI_FILE_FILETYPE)
    {
      r = TPIa_fileRemove(acPath);
      if (r != WE_FILE_OK)
        return;
      acPath[iPathLength] = '\0';
    }
    else
    {
      if (TPIa_fileRmDir(acPath) == WE_FILE_OK)
      {
        if (iPathLength > 1)
          while (acPath[--iPathLength] != '/');
                
          acPath[++iPathLength] = '\0';
          continue;
      }

      p = FRW_ALLOCTYPE (frw_dir_stack_t);

      p->path_length = iPathLength;
      p->next = pstStack;
      pstStack = p;

      iPathLength = strlen (acPath) + 1;
      acPath[iPathLength - 1] = '/';
      acPath[iPathLength    ] = '\0';
    }
  }
}



static char*
frw_get_string_option
(
    const char *s, 
    char cOption
)
{
  const char *p = s;
  char       *r = NULL;
  int         n;

  cOption = we_cmmn_chrlc(cOption);
  while (p && !r) {
    p = strchr(p, '-');
    if (p) {
      if ((we_cmmn_chrlc(*(p+1)) == cOption) && 
          ((p == s) || (*(p-1) == ' ') || (*(p-1) == '\t')))
        r = we_cmmn_skip_blanks(p+2); 
      p++;
    }
  }

  if (r) {
    
    do {
      p++;
      p = strchr(p, '-');
    } while (p && (*(p-1) != ' ') && (*(p-1) != '\t'));
    n = p ? p - r : strlen(r);
    n = we_cmmn_skip_trailing_blanks(r, n);
    r =  n ? we_cmmn_strndup(WE_MODID_FRW, r, n) : NULL;
  }
  return r;
}


void
frw_start 
(
    const char* pcOptions, 
    const int staticModules[], 
    int iNumberOfStaticModules
)
{
  char *r;
  char *d;

  if (frw_closing)
    return;

  frw_inside_run = 0;
  WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_FRW,
                "FRW: Module Version: module=WE, version=%s\n",
                 WE_VERSION));

  WE_MEM_INIT (WE_MODID_FRW, frw_exception_handler, FALSE);
  frw_signal_init ();
  frw_signal_register_dst (FRW_SIG_DST_FRW_MAIN, frw_main);

  FRWa_moduleStatus (WE_MODID_FRW, FRW_MODULE_STATUS_CREATED);
  WE_SIGNAL_REGISTER_QUEUE (WE_MODID_FRW);

  frw_sub_module_delayed = 0;
  frw_started_modules_int = NULL;
  frw_notify_term_int = NULL;

  frw_init_file_list ();
  frw_init_module_info (staticModules, iNumberOfStaticModules);

  
  frw_timer_init ();
  frw_cmmn_init ();

  r = frw_get_string_option (pcOptions, 'r');
  d = frw_get_string_option (pcOptions, 'd');
  frw_reg_init (r, d);
  FRW_FREE (r);
  FRW_FREE (d);

  frw_create_user_folders ();
  frw_create_tmp_folder ();

  
  if(frw_sub_module_delayed == 0){
    frw_init_done ();
  }

}




static void
frw_init_done (void)
{
  frw_sub_module_delayed--;

  if(frw_sub_module_delayed <= 0){
    frw_get_content_routing_data ();
    frw_subscribe_content_routing_data ();
    frw_start_static_modules ();
    FRWa_moduleStatus (WE_MODID_FRW, FRW_MODULE_STATUS_ACTIVE);
    WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_FRW, "FRW: Initialised Done\n"));
    frw_sub_module_delayed = 0;
  }
}


void
frw_sub_module_init_delay (void)
{
  frw_sub_module_delayed++;  
}


void
frw_sub_module_terminate_delay (void)
{
  frw_sub_module_delayed++;
}


int
frw_modid_str2int (const char* pcStringModId)
{
  int   i;

  for (i = 0; i < WE_NUMBER_OF_MODULES; i++)
    if (we_cmmn_strcmp_nc (pcStringModId, frw_modules_startup[i].short_name) == 0)
      return i;
  return -1;
}


char*
frw_get_sub_path (char* pcSrc, char* pcDst)
{
  char* s;

  pcSrc++;
  if (*pcSrc == 0) {
    *pcDst = 0;
    return NULL;
  }
  if ((s = strchr (pcSrc, '/')) == NULL) {
    strcpy (pcDst, pcSrc);
    s = pcSrc + strlen (pcSrc) - 1;
  }
  else {
    strncpy (pcDst, pcSrc, s - pcSrc);
    pcDst[s-pcSrc] = 0;
  }
  return s;
}


int
frw_get_module_status (WE_UINT8 uiModId)
{
  return frw_module_info[uiModId].status;
}


void
frw_object_action 
(
    const we_act_content_t* pstContent,
    const char*              pcExclActStrings[],
    WE_INT32                iExclActStringsCnt,
    WE_UINT8                uiAckDestModid,
    WE_UINT8                uiAckId
)
{
  frw_object_action_t *p = NULL;
  WE_INT32            i;

  if ((pstContent == NULL) || (pstContent->mime_type == NULL))
    return;

  if (!((pstContent->data_type == WeResourceBuffer) && (pstContent->data != NULL)) && 
      !((pstContent->data_type == WeResourceFile)   && (pstContent->src_path != NULL)) &&
      !((pstContent->data_type == WeResourcePipe)   && (pstContent->src_path != NULL)))
    return;

  p = FRW_ALLOCTYPE (frw_object_action_t);
  if (p != NULL) {
    p->content.data = NULL;
    p->content.data_len = 0;
    p->content.data_type = pstContent->data_type;
    p->content.default_name = NULL;
    p->content.src_path = NULL;
    p->content.content_type = NULL;
    p->excl_act_strings_cnt = 0;
    p->excl_act_strings = NULL;

    p->content.mime_type = FRW_ALLOC (strlen (pstContent->mime_type) + 1);
    if (p->content.mime_type == NULL)
      goto error;
    strcpy (p->content.mime_type, pstContent->mime_type);
    
    if ((pstContent->data_type == WeResourceFile) || (pstContent->data_type == WeResourcePipe)){
      p->content.src_path = FRW_ALLOC (strlen (pstContent->src_path) + 1);
      if (p->content.src_path == NULL)
        goto error;
      strcpy (p->content.src_path, pstContent->src_path);
    }
    else {
      p->content.data_len = pstContent->data_len;
      p->content.data     = FRW_ALLOC (pstContent->data_len);
      if (p->content.data == NULL)
        goto error;
      memcpy (p->content.data, pstContent->data, pstContent->data_len);
    }
    
    if (pstContent->default_name != NULL){
      p->content.default_name = FRW_ALLOC (strlen (pstContent->default_name) + 1);
      if (p->content.default_name == NULL)
        goto error;
      strcpy (p->content.default_name, pstContent->default_name);
    }

    if (pstContent->content_type != NULL){
      p->content.content_type = FRW_ALLOC (strlen (pstContent->content_type) + 1);
      if (p->content.content_type == NULL)
        goto error;
      strcpy (p->content.content_type, pstContent->content_type);
    }

    if ((iExclActStringsCnt > 0) && pcExclActStrings) {
      p->excl_act_strings_cnt = iExclActStringsCnt;
      p->excl_act_strings = FRW_ALLOC (iExclActStringsCnt * sizeof(char*));
      if (p->excl_act_strings == NULL)
        goto error;
      memset(p->excl_act_strings, 0, iExclActStringsCnt * sizeof(char*));
      for (i=0; i<iExclActStringsCnt; i++) {
        p->excl_act_strings[i] = FRW_ALLOC (strlen (pcExclActStrings[i]) + 1);
        if (p->excl_act_strings[i] == NULL)
          goto error;
        strcpy (p->excl_act_strings[i], pcExclActStrings[i]);
      }
    }
    
    FRW_SIGNAL_SENDTO_IUUP (FRW_SIG_DST_FRW_CMMN, FRW_CMMN_SIG_OBJECT_ACTION,
                            0, uiAckDestModid, uiAckId, p);
  }
  return;

error:
  if (p != NULL){
    if (p->excl_act_strings_cnt)
      for (i=0; i<iExclActStringsCnt; i++)
        FRW_FREE(p->excl_act_strings[i]);
    FRW_FREE (p->content.mime_type);
    FRW_FREE (p->content.default_name);
    FRW_FREE (p->content.src_path);
    FRW_FREE (p->content.content_type);
    FRW_FREE (p->content.data);
    FRW_FREE (p);
  }
}


static void
frw_store_command_data 
(
    frw_module_t* stModuleInfo, 
    WE_UINT8 uiUserModId,
    char* pcStartOptions, 
    we_content_data_t *pstContentData,
    WE_UINT8 uiSendAck, 
    WE_UINT8 wid
)
{
  frw_command_data_t  *pstCommand, *p;

  pstCommand = FRW_ALLOCTYPE (frw_command_data_t);
  if (pstCommand == NULL)
    return;
  pstCommand->startOptions = pcStartOptions;
  pstCommand->startOptions_user = uiUserModId;
  pstCommand->content_data = pstContentData;
  pstCommand->sendAck = uiSendAck;
  pstCommand->wid = wid;
  pstCommand->next = NULL;

  for (p = stModuleInfo->command_data; (p != NULL && p->next != NULL); p = p->next);

  if (p == NULL)
    stModuleInfo->command_data = pstCommand;
  else
    p->next = pstCommand;
}




static void
frw_free_command_data (frw_command_data_t* pstCommandData)
{
  frw_command_data_t* p;
  we_dcvt_t          stCvtObj;

  while (pstCommandData != NULL) {
    FRW_FREE (pstCommandData->startOptions);
    if (pstCommandData->content_data != NULL) {
      
      if ((pstCommandData->sendAck == 0) &&
          (pstCommandData->content_data->contentDataType == WE_CONTENT_DATA_PIPE) &&
          (pstCommandData->content_data->contentSource != NULL)) {
        WE_PIPE_DELETE (pstCommandData->content_data->contentSource);
      }
      we_dcvt_init (&stCvtObj, WE_DCVT_FREE, NULL, 0, WE_MODID_FRW);
      we_cvt_content_data (&stCvtObj, pstCommandData->content_data);
      FRW_FREE (pstCommandData->content_data);
    }
    p = pstCommandData;
    pstCommandData = pstCommandData->next;
    FRW_FREE (p);
  }
}


static int
frw_send_signal_to_external (void* pvBuffer)
{
  we_dcvt_t  stCvtObj;
  WE_UINT16  uiLength;
  WE_UINT8   uiSrcModId, uiDestModId;

  if (pvBuffer != NULL){
      
      
    we_dcvt_init (&stCvtObj, WE_DCVT_DECODE, (char*)pvBuffer + 2, 4, 0);
    we_dcvt_uint8 (&stCvtObj, &uiSrcModId);
    we_dcvt_uint8 (&stCvtObj, &uiDestModId);
    we_dcvt_uint16 (&stCvtObj, &uiLength);
    if (TPIa_SignalSend (uiDestModId, pvBuffer, (WE_UINT16)(WE_SIGNAL_HEADER_LENGTH + uiLength)) < 0)
      return FALSE;
  }
  return TRUE;
}


static int
frw_send_execute_command 
(
    WE_UINT8 uiDestModId, 
    WE_UINT8 uiSrcModId, 
    char* pcCommandOptions,
    we_content_data_t* pstContentData, 
    int iSendToExternal
)
{
  we_module_execute_cmd_t    stExecuteCmd;
  we_dcvt_t                  stCvtObj;
  void                        *pvBuffer, *pvUserData;
  WE_UINT16                  uiLength;

  stExecuteCmd.srcModId = uiSrcModId;
  stExecuteCmd.commandOptions = pcCommandOptions;
  if (pstContentData == NULL) {
    stExecuteCmd.contentDataExists = 0;
    stExecuteCmd.contentData = NULL;
  }
  else {
    stExecuteCmd.contentDataExists = 1;
    stExecuteCmd.contentData = pstContentData;
  }

  we_dcvt_init (&stCvtObj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_module_execute_cmd (&stCvtObj, &stExecuteCmd);
  uiLength = (WE_UINT16)stCvtObj.pos;

  pvBuffer = WE_SIGNAL_CREATE (WE_SIG_MODULE_EXECUTE_COMMAND, WE_MODID_FRW,
                              uiDestModId, uiLength);
  if (pvBuffer != NULL){
    pvUserData = WE_SIGNAL_GET_USER_DATA (pvBuffer, &uiLength);
    we_dcvt_init (&stCvtObj, WE_DCVT_ENCODE, pvUserData, uiLength, WE_MODID_FRW);
    we_cvt_module_execute_cmd (&stCvtObj, &stExecuteCmd);
    if (iSendToExternal == FALSE)
      WE_SIGNAL_SEND (pvBuffer);
    else
      return frw_send_signal_to_external (pvBuffer);
  }
  return TRUE;
}


static void
frw_execute_commands 
(
    WE_UINT8 uiModId, 
    frw_module_t* pstModuleInfo
)
{
  frw_command_data_t  *pstCommand;
  we_dcvt_t          stCvtObj;

  while (pstModuleInfo->command_data != NULL) {
    frw_send_execute_command (uiModId, pstModuleInfo->command_data->startOptions_user,
                              pstModuleInfo->command_data->startOptions,
                              pstModuleInfo->command_data->content_data, FALSE);
    if (pstModuleInfo->command_data->sendAck > 0)
      frw_content_send_ack (pstModuleInfo->command_data->startOptions_user,
                            pstModuleInfo->command_data->wid, WE_CONTENT_STATUS_SUCCEED, FALSE);
    pstCommand = pstModuleInfo->command_data;
    pstModuleInfo->command_data = pstModuleInfo->command_data->next;

    FRW_FREE (pstCommand->startOptions);
    if (pstCommand->content_data != NULL) {
      we_dcvt_init (&stCvtObj, WE_DCVT_FREE, NULL, 0, WE_MODID_FRW);
      we_cvt_content_data (&stCvtObj, pstCommand->content_data);
      FRW_FREE (pstCommand->content_data);
    }
    FRW_FREE (pstCommand);
  }
}


static void
frw_send_module_status 
(
    WE_UINT8 uiModId, 
    WE_UINT8 uiUserModId, 
    int iStatus, 
    char* pcModVersion
)
{
  
  if (uiUserModId == WE_MODID_FRW){
    char *s;
    
    s = we_cmmn_strdup(WE_MODID_FRW, pcModVersion);
    
    FRW_SIGNAL_SENDTO_IUP (FRW_SIG_DST_FRW_MAIN, FRW_MAIN_SIG_MODULE_STATUS,
                           iStatus, uiModId, s);
  }
  else {
    we_module_status_t   stModStatus;
    we_dcvt_t            stCvtObj;
    void                  *pvBuffer, *pvUserData;
    WE_UINT16            uiLength;

    stModStatus.modId = uiModId;
    stModStatus.status = (WE_UINT8)iStatus;
    stModStatus.modVersion = pcModVersion;
    we_dcvt_init (&stCvtObj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
    we_cvt_module_status (&stCvtObj, &stModStatus);
    uiLength = (WE_UINT16)stCvtObj.pos;
    
    pvBuffer = WE_SIGNAL_CREATE (WE_SIG_MODULE_STATUS, WE_MODID_FRW, uiUserModId, uiLength);
    if (pvBuffer != NULL){
      pvUserData = WE_SIGNAL_GET_USER_DATA (pvBuffer, &uiLength);
      we_dcvt_init (&stCvtObj, WE_DCVT_ENCODE, pvUserData, uiLength, WE_MODID_FRW);
      we_cvt_module_status (&stCvtObj, &stModStatus);
      WE_SIGNAL_SEND (pvBuffer);
    }
  }
}


static void
frw_send_terminate_module (WE_UINT8 uiDestModId)
{
  we_module_terminate_t  stTerm;
  we_dcvt_t              stCvtObj;
  void                    *pvBuffer, *pvUserData;
  WE_UINT16              uiLength;

  stTerm.modId = uiDestModId;
  we_dcvt_init (&stCvtObj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_module_terminate (&stCvtObj, &stTerm);
  uiLength = (WE_UINT16)stCvtObj.pos;

  pvBuffer = WE_SIGNAL_CREATE (WE_SIG_MODULE_TERMINATE, WE_MODID_FRW, uiDestModId, uiLength);
  if (pvBuffer != NULL){
    pvUserData = WE_SIGNAL_GET_USER_DATA (pvBuffer, &uiLength);
    we_dcvt_init (&stCvtObj, WE_DCVT_ENCODE, pvUserData, uiLength, WE_MODID_FRW);
    we_cvt_module_terminate (&stCvtObj, &stTerm);
    WE_SIGNAL_SEND (pvBuffer);
  }

  frw_module_info[uiDestModId].status = MODULE_STATUS_SHUTTING_DOWN;
  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                "FRW: Module Status: module=%d, status=%d\n",
                uiDestModId, frw_module_info[uiDestModId].status));
  FRWa_moduleStatus (uiDestModId, frw_module_info[uiDestModId].status);
}


void
frw_init_module_info (const int aiStaticModules[], int iNumberOfStaticModules)
{
  int                 i;
  
  for (i = 0; i < WE_NUMBER_OF_MODULES; i++){
    frw_module_info[i].status = MODULE_STATUS_NULL;
    frw_module_info[i].startup_policy = WE_STARTUP_DYNAMIC;
    frw_module_info[i].version = NULL;
    frw_module_info[i].nr_of_users = 0;
    frw_module_info[i].users = NULL;
    frw_module_info[i].len_users = 0;
    frw_module_info[i].mime_type = NULL;
    frw_module_info[i].mime_ext = NULL;
    frw_module_info[i].papp_id_string = NULL;
    frw_module_info[i].papp_id_number = NULL;
    frw_module_info[i].scheme = NULL;
    frw_module_info[i].abort_user = -1;
    frw_module_info[i].command_data = NULL;
  }

  for (i = 0; i < iNumberOfStaticModules; i++){
    frw_module_info[aiStaticModules[i]].startup_policy = WE_STARTUP_STATIC;
  }
  
  
  frw_module_info[WE_MODID_FRW].startup_policy = WE_STARTUP_STATIC;

}


static void
frw_start_static_modules (void)
{
  char*               pcStartOptions = NULL;
  we_content_data_t* pstContentData = NULL;
  WE_UINT8           uiContentDataExist = FALSE;
  int                 i;

  frw_module_info[WE_MODID_FRW].status = MODULE_STATUS_ACTIVE;
  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                "FRW: Module Status: module=%d, status=%d\n",
                WE_MODID_FRW, MODULE_STATUS_ACTIVE));
  for (i = 1; i < WE_NUMBER_OF_MODULES; i++){
    if (frw_module_info[i].startup_policy == WE_STARTUP_STATIC)
      frw_start_module ((WE_UINT8)i, WE_MODID_FRW, &pcStartOptions, &pstContentData,
                         &uiContentDataExist, FALSE, FALSE, 0);
  }
}


static void
frw_free_module_info_memory (void)
{
  int           i;

  for (i = 0; i < WE_NUMBER_OF_MODULES; i++){
    FRW_FREE (frw_module_info[i].version);
    frw_module_info[i].version = NULL;
    FRW_FREE (frw_module_info[i].users);
    frw_module_info[i].users = NULL;
    frw_module_info[i].nr_of_users = 0;
    frw_module_info[i].len_users = 0;
    FRW_FREE (frw_module_info[i].mime_type);
    frw_module_info[i].mime_type = NULL;
    FRW_FREE (frw_module_info[i].mime_ext);
    frw_module_info[i].mime_ext = NULL;
    FRW_FREE (frw_module_info[i].papp_id_string);
    frw_module_info[i].papp_id_string = NULL;
    FRW_FREE (frw_module_info[i].papp_id_number);
    frw_module_info[i].papp_id_number = NULL;
    FRW_FREE (frw_module_info[i].scheme);
    frw_module_info[i].scheme = NULL;
    frw_free_command_data (frw_module_info[i].command_data);
  }
}



static void
frw_exit_manager (void)
{
  
  frw_closing = TRUE;

  frw_start_module_list_destroy (&frw_started_modules_int);
  frw_start_module_list_destroy (&frw_notify_term_int);

  frw_reg_terminate ();
  frw_timer_terminate ();

  
  if(frw_sub_module_delayed == 0){
    frw_complete_exit ();
  }

}


static void
frw_complete_exit (void)
{
  frw_sub_module_delayed--;
  
  if(frw_sub_module_delayed <= 0){
    
    WE_SIGNAL_DEREGISTER_QUEUE (WE_MODID_FRW);
    
    frw_cmmn_terminate ();
    frw_free_module_info_memory ();
    frw_signal_terminate ();
    WE_MEM_FREE_ALL (WE_MODID_FRW);
    
    WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_FRW, "FRW: Terminated\n"));
    frw_closing = FALSE;
    frw_sub_module_delayed = 0;
    FRWa_terminated ();
  }
}


static void
frw_terminate (void)
{
  int i, iCounter = 0;

  if (frw_module_info[WE_MODID_FRW].status != MODULE_STATUS_SHUTTING_DOWN) {
    frw_module_info[WE_MODID_FRW].status = MODULE_STATUS_SHUTTING_DOWN;
    WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                  "FRW: Module Status: module=%d, status=%d\n",
                  WE_MODID_FRW, frw_module_info[WE_MODID_FRW].status));
    FRWa_moduleStatus (WE_MODID_FRW, frw_module_info[WE_MODID_FRW].status);
  }
  for (i = 1; i < WE_NUMBER_OF_MODULES; i++){
    if (frw_module_info[i].status != MODULE_STATUS_NULL){
      frw_send_terminate_module ((WE_UINT8)i);
      iCounter++;
    }
  }
    
  if (iCounter == 0)
    frw_exit_manager();
}


static void
frw_add_user 
(
    WE_UINT8 uiModId, 
    WE_UINT8 uiUserModId
)
{
  WE_UINT8*    puiUsers;
  int           i;
  frw_module_t* pstMod = &(frw_module_info[uiModId]);

  for (i = 0; i < pstMod->nr_of_users; i++)
    if (pstMod->users[i] == (WE_UINT8)uiUserModId)
      return;

  pstMod->nr_of_users++;

  if (pstMod->len_users < pstMod->nr_of_users) {
    pstMod->len_users += 5;
    puiUsers = FRW_ALLOC(pstMod->len_users);
    if (puiUsers != NULL) {
      if (pstMod->nr_of_users > 1) {
        memcpy (puiUsers, pstMod->users, pstMod->nr_of_users - 1);
        FRW_FREE (pstMod->users);
      }
      puiUsers[pstMod->nr_of_users - 1] = uiUserModId;
      pstMod->users = puiUsers;
    }
    else
      pstMod->len_users -= 5;
  }
  else
    pstMod->users[pstMod->nr_of_users - 1] = uiUserModId;
}


static void
frw_remove_user (WE_UINT8 uiModId, int uiUserModId)
{
  int           i;
  frw_module_t* pstMod = &(frw_module_info[uiModId]);

  if (pstMod->nr_of_users == 0)
    return;

  if (uiUserModId < 0 || pstMod->nr_of_users == 1)
    pstMod->nr_of_users = 0;
  else
    for (i = 0; i < pstMod->nr_of_users; i++){
      if (pstMod->users[i] == uiUserModId) {
        pstMod->users[i] = pstMod->users[pstMod->nr_of_users - 1];
        pstMod->nr_of_users--;
        break;
      }
    }
}


static void
frw_start_module 
(
    WE_UINT8 uiModId, 
    WE_UINT8 uiUserModId, 
    char** ppcStartOptions,
    we_content_data_t** ppstContentData, 
    WE_UINT8* puiContentDataExist,
    int iContentSend, 
    WE_UINT8 uiContentSendAck, 
    WE_UINT8 uiContentId
)
{
  frw_module_t* pstMod = &(frw_module_info[uiModId]);

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                "FRW: Start Module: module=%d, user=%d, Old Status=%d, content_send=%d\n",
                uiModId, uiUserModId, pstMod->status, iContentSend));

  if (pstMod->startup_policy == WE_STARTUP_EXTERNAL){
    WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
      "FRW: Start Module Failed!!! Module is defined to be EXTERNAL"));
    return;
  }

  switch (pstMod->status) {
    case MODULE_STATUS_NULL:
        
      FRWa_createTask (uiModId);
      pstMod->status = MODULE_STATUS_STANDBY;

      if (!iContentSend)
        frw_add_user (uiModId, uiUserModId);
        
      if (pstMod->command_data != NULL) {
        frw_free_command_data (pstMod->command_data);
        pstMod->command_data = NULL;
      }
        
      frw_store_command_data (pstMod, uiUserModId,
                              *ppcStartOptions, *ppstContentData, uiContentSendAck, 
                              uiContentId);
      *ppcStartOptions = NULL;
      *ppstContentData = NULL;
      *puiContentDataExist = 0;
      break;

    case MODULE_STATUS_STANDBY:
        
      if (pstMod->abort_user != -1)
        pstMod->abort_user = -1;
      

    case MODULE_STATUS_CREATED:
      if (!iContentSend)
        frw_add_user (uiModId, uiUserModId);
        
      frw_store_command_data (pstMod, uiUserModId,
                              *ppcStartOptions, *ppstContentData, uiContentSendAck, 
                              uiContentId);
      *ppcStartOptions = NULL;
      *ppstContentData = NULL;
      *puiContentDataExist = 0;
      break;

    case MODULE_STATUS_ACTIVE:
      if (!iContentSend) {
        frw_add_user (uiModId, uiUserModId);
        frw_send_module_status (uiModId, uiUserModId, MODULE_STATUS_ACTIVE, pstMod->version);
      }
      frw_send_execute_command (uiModId, uiUserModId, *ppcStartOptions, *ppstContentData, FALSE);
      if (iContentSend == TRUE && uiContentSendAck > 0)
        frw_content_send_ack (uiUserModId, uiContentId, WE_CONTENT_STATUS_SUCCEED, FALSE);
      break;

    case MODULE_STATUS_SHUTTING_DOWN:
        
      if (!iContentSend)
        pstMod->abort_user = uiUserModId;
      else
        pstMod->abort_user = WE_MODID_FRW;

        
      frw_store_command_data (pstMod, uiUserModId,
                              *ppcStartOptions, *ppstContentData, uiContentSendAck, 
                              uiContentId);
      *ppcStartOptions = NULL;
      *ppstContentData = NULL;
      *puiContentDataExist = 0;
      break;
  }
}


static void
frw_terminate_module 
(
    WE_UINT8 uiModId, 
    WE_UINT8 uiUserModId
)
{
  frw_module_t* pstMod = &(frw_module_info[uiModId]);

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                "FRW: Terminate Module: module=%d, user=%d, Old Status=%d\n",
                uiModId, uiUserModId, pstMod->status));
  if (pstMod->startup_policy == WE_STARTUP_EXTERNAL){
    WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
      "FRW: Terminate Module Failed!!! Module is defined to be EXTERNAL"));
    return;
  }

  switch (pstMod->status) {
    case MODULE_STATUS_NULL:
        frw_send_module_status (uiModId, uiUserModId, MODULE_STATUS_TERMINATED, NULL);
      break;

    case MODULE_STATUS_STANDBY:
      if ((frw_module_info[uiModId].startup_policy == WE_STARTUP_DYNAMIC &&
           pstMod->nr_of_users == 1 &&
           pstMod->users[0] == uiUserModId))
          
        pstMod->abort_user = uiUserModId;
      else {
        frw_send_module_status (uiModId, uiUserModId, MODULE_STATUS_TERMINATED, NULL);
        frw_remove_user (uiModId, uiUserModId);
      }
      break;

    case MODULE_STATUS_CREATED:
    case MODULE_STATUS_ACTIVE:
      if ((frw_module_info[uiModId].startup_policy == WE_STARTUP_DYNAMIC &&
           pstMod->nr_of_users == 1 &&
           pstMod->users[0] == uiUserModId))
        frw_send_terminate_module (uiModId);
      else {
        frw_send_module_status (uiModId, uiUserModId, MODULE_STATUS_TERMINATED, NULL);
        frw_remove_user (uiModId, uiUserModId);
      }
      break;

    case MODULE_STATUS_SHUTTING_DOWN:
      if (pstMod->abort_user != -1 &&
          pstMod->abort_user == uiUserModId) {
          
        pstMod->abort_user = -1;
        if (pstMod->command_data != NULL) {
          frw_free_command_data (pstMod->command_data);
          pstMod->command_data = NULL;
        }
      }
      else
          
        frw_add_user (uiModId, uiUserModId);
      break;
  }
}


static void
frw_module_status_notify 
(
    WE_UINT8 uiModId, 
    int iStatus, 
    char* pcModVersion
)
{
  frw_module_t* pstMod = &(frw_module_info[uiModId]);
  int           i;

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                "FRW: Module Status: module=%d, New Status=%d, Old Status=%d\n",
                uiModId, iStatus, pstMod->status));

  switch (iStatus) {
    case MODULE_STATUS_CREATED:
      if (pstMod->status == MODULE_STATUS_STANDBY) {
        pstMod->status = iStatus;
        FRW_FREE (pstMod->version);
        pstMod->version = pcModVersion;
        WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_FRW,
                      "FRW: Module Version: module=%s, version=%s\n",
                       frw_modules_startup[uiModId].short_name, pcModVersion ? pcModVersion : "(null)"));

          
        FRWa_moduleStatus (uiModId, iStatus);

        if (pstMod->abort_user == -1)
          frw_execute_commands (uiModId, pstMod);
        else {
            
          frw_command_data_t* p;
          WE_BOOL is_pipe;
          for (p = pstMod->command_data; p; p = p->next) {
            if (p->sendAck) {
              is_pipe = p->content_data && p->content_data->contentSource &&
                        (p->content_data->contentDataType == WE_CONTENT_DATA_PIPE);
              frw_content_send_ack (p->startOptions_user, p->wid, WE_CONTENT_STATUS_FAILED, is_pipe);
            }
          }

          frw_send_terminate_module (uiModId);
          frw_free_command_data (pstMod->command_data);
          pstMod->command_data = NULL;
          pstMod->abort_user = -1;
        }
      }
      else
        FRW_FREE (pcModVersion);
      break;

    case MODULE_STATUS_ACTIVE:
      if (pstMod->status == MODULE_STATUS_CREATED) {
        pstMod->status = iStatus;
        FRWa_moduleStatus (uiModId, iStatus);

        

        for (i = 0; i < pstMod->nr_of_users; i++) {
          frw_send_module_status (uiModId, pstMod->users[i], iStatus, pstMod->version);
        }
      }
        
      break;

    case MODULE_STATUS_TERMINATED:
      if (pstMod->status == MODULE_STATUS_STANDBY ||
          pstMod->status == MODULE_STATUS_CREATED ||
          pstMod->status == MODULE_STATUS_ACTIVE ||
          pstMod->status == MODULE_STATUS_SHUTTING_DOWN) {
        FRWa_moduleStatus (uiModId, iStatus);

        

        for (i = 0; i < pstMod->nr_of_users; i++) {
          frw_send_module_status (uiModId, pstMod->users[i], iStatus, NULL);
        }
          
        frw_remove_user (uiModId, -1);
        
        frw_mod_user_notify_int (&frw_notify_term_int, uiModId);

        if (pstMod->status == MODULE_STATUS_SHUTTING_DOWN &&
            pstMod->abort_user != -1) {
            
          FRWa_createTask (uiModId);
          pstMod->status = MODULE_STATUS_STANDBY;

          frw_add_user (uiModId, (WE_UINT8)(pstMod->abort_user));
          pstMod->abort_user = -1;
        }
        else
          pstMod->status = MODULE_STATUS_NULL;

          

        if (frw_module_info[WE_MODID_FRW].status == MODULE_STATUS_SHUTTING_DOWN){
          for (i = 1; i < WE_NUMBER_OF_MODULES; i++){
            if (frw_module_info[i].status != MODULE_STATUS_NULL)
              return;
          }
          FRW_SIGNAL_SENDTO(FRW_SIG_DST_FRW_MAIN, FRW_MAIN_SIG_TERMINATE);
        }
      }
    break;
  }
}


static void
frw_get_content_routing_data (void)
{
  we_registry_identifier_t* p;

  p = FRW_ALLOCTYPE (we_registry_identifier_t);
  if (p != NULL) {
    p->wid = 2;
    p->path = we_cmmn_strdup (WE_MODID_FRW, FRW_CONTENT_ROUTING_PATH);
    p->key = NULL;

    FRW_SIGNAL_SENDTO_IP (FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_GET, WE_MODID_FRW, p);
  }
}


static void
frw_subscribe_content_routing_data (void)
{
  we_registry_subscription_t* p;

  p = FRW_ALLOCTYPE (we_registry_subscription_t);
  if (p != NULL) {
    p->wid = 1; 
    p->add = TRUE;
    p->path = we_cmmn_strdup (WE_MODID_FRW, FRW_CONTENT_ROUTING_PATH);
    p->key = NULL;

    FRW_SIGNAL_SENDTO_IP (FRW_SIG_DST_FRW_REGISTRY, FRW_REGISTRY_SIG_SUBSCRIBE, WE_MODID_FRW, p);
  }
}


static void
frw_response (void* p)
{
  we_registry_param_t      stParam;
  we_registry_response_t*  pstRresp = (we_registry_response_t*)p;
  int                       iLen = strlen (FRW_CONTENT_ROUTING_PATH);
  int                       iModId;
  char                      acSubPath[10];
  char*                     s;
#ifdef WE_LOG_MODULE
  int                       iFirst = 1;
#endif

  while (WE_REGISTRY_RESPONSE_GET_NEXT (pstRresp, &stParam)) {
      
    if (we_cmmn_strncmp_nc (stParam.path, FRW_CONTENT_ROUTING_PATH, iLen) == 0) {
      s = frw_get_sub_path (stParam.path + iLen, acSubPath);
      if (*acSubPath != 0 && *s != '/') {
          
        if ((iModId = frw_modid_str2int (acSubPath)) != -1) {
#ifdef WE_LOG_MODULE
          if (iFirst) {
            WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                          "FRW: Received Content Routing information from the registry\n"));
            iFirst = 0;
          }
#endif
            
          if (we_cmmn_strcmp_nc (stParam.key, FRW_REG_KEY_ROUTING_MIME) == 0) {
            if (stParam.type == WE_REGISTRY_TYPE_STR) {
              FRW_FREE (frw_module_info[iModId].mime_type);
              frw_module_info[iModId].mime_type = frw_str_dup_skip_blanks ((char*)(stParam.value_bv));
            }
            else if (stParam.type == WE_REGISTRY_TYPE_DELETED) {
              FRW_FREE (frw_module_info[iModId].mime_type);
              frw_module_info[iModId].mime_type = NULL;
            }
          }
          else if (we_cmmn_strcmp_nc (stParam.key, FRW_REG_KEY_ROUTING_EXT) == 0) {
            if (stParam.type == WE_REGISTRY_TYPE_STR) {
              FRW_FREE (frw_module_info[iModId].mime_ext);
              frw_module_info[iModId].mime_ext = frw_str_dup_skip_blanks ((char*)(stParam.value_bv));
            }
            else if (stParam.type == WE_REGISTRY_TYPE_DELETED) {
              FRW_FREE (frw_module_info[iModId].mime_ext);
              frw_module_info[iModId].mime_ext = NULL;
            }
          }
          else if (we_cmmn_strcmp_nc (stParam.key, FRW_REG_KEY_ROUTING_PAPPID_N) == 0) {
            if (stParam.type == WE_REGISTRY_TYPE_STR) {
              FRW_FREE (frw_module_info[iModId].papp_id_number);
              frw_module_info[iModId].papp_id_number = frw_str_dup_skip_blanks ((char*)(stParam.value_bv));
            }
            else if (stParam.type == WE_REGISTRY_TYPE_DELETED) {
              FRW_FREE (frw_module_info[iModId].papp_id_number);
              frw_module_info[iModId].papp_id_number = NULL;
            }
          }
          else if (we_cmmn_strcmp_nc (stParam.key, FRW_REG_KEY_ROUTING_PAPPID_S) == 0) {
            if (stParam.type == WE_REGISTRY_TYPE_STR) {
              FRW_FREE (frw_module_info[iModId].papp_id_string);
              frw_module_info[iModId].papp_id_string = frw_str_dup_skip_blanks ((char*)(stParam.value_bv));
            }
            else if (stParam.type == WE_REGISTRY_TYPE_DELETED) {
              FRW_FREE (frw_module_info[iModId].papp_id_string);
              frw_module_info[iModId].papp_id_string = NULL;
            }
          }
          else if (we_cmmn_strcmp_nc (stParam.key, FRW_REG_KEY_ROUTING_SCHEME) == 0) {
            if (stParam.type == WE_REGISTRY_TYPE_STR) {
              FRW_FREE (frw_module_info[iModId].scheme);
              frw_module_info[iModId].scheme = frw_str_dup_skip_blanks ((char*)(stParam.value_bv));
            }
            else if (stParam.type == WE_REGISTRY_TYPE_DELETED) {
              FRW_FREE (frw_module_info[iModId].scheme);
              frw_module_info[iModId].scheme = NULL;
            }
          }
        }
      }
    }
  }
  WE_REGISTRY_RESPONSE_FREE (WE_MODID_FRW, pstRresp);
}



static char *
frw_str_dup_skip_blanks (char *s)
{
  char  *t, *u;
  int   i, iLen;

  if (s == NULL)
    return NULL;
  iLen = strlen (s);
  t = u = FRW_ALLOC (iLen + 1);

  for (i = 0; i < iLen; i++, s++) {
    if ((*s != ' ') && (*s != '\t')) {
      *t = *s;
      t++;
    }
  }
  *t = 0;
  return u;
}


static int
frw_con_rout_search_for_mime 
(
    char* pcSrc, 
    char* pcSearch
)
{
  char* s;
  int   iLen;

  if (pcSrc == NULL)
    return FALSE;
  iLen = strlen (pcSearch);
  while ((s = strchr (pcSrc, ',')) != NULL) {
    if (we_cmmn_strncmp_nc (pcSrc, pcSearch, s - pcSrc) == 0 && ((s - pcSrc) == iLen))
      return TRUE;
    if (*(pcSrc = s + 1) == 0)
      return FALSE;
  }
  if (we_cmmn_strcmp_nc (pcSrc, pcSearch) == 0)
    return TRUE;

  return FALSE;
}


static int
frw_con_rout_get_module 
(
    int iRoutingFormat, 
    char* pcRoutingIdentifier
)
{
  char  *pcSearch;
  int   i;

  if (pcRoutingIdentifier == NULL ||
     (iRoutingFormat != WE_ROUTING_TYPE_MIME && iRoutingFormat != WE_ROUTING_TYPE_EXT &&
      iRoutingFormat != WE_ROUTING_TYPE_PAPPID && iRoutingFormat != WE_ROUTING_TYPE_SCHEME))
    return -1;

  pcSearch = frw_str_dup_skip_blanks (pcRoutingIdentifier);
  for (i = 1; i < WE_NUMBER_OF_MODULES; i++) {
    if (iRoutingFormat == WE_ROUTING_TYPE_MIME) {
      if (frw_con_rout_search_for_mime (frw_module_info[i].mime_type, pcSearch)) {
        FRW_FREE (pcSearch);
        return i;
      }
    }
    else if (iRoutingFormat == WE_ROUTING_TYPE_EXT) {
      if (frw_con_rout_search_for_mime (frw_module_info[i].mime_ext, pcSearch)) {
        FRW_FREE (pcSearch);
        return i;
      }
    }
    else if (iRoutingFormat == WE_ROUTING_TYPE_PAPPID) {
      if (frw_con_rout_search_for_mime (frw_module_info[i].papp_id_string, pcSearch) ||
          frw_con_rout_search_for_mime (frw_module_info[i].papp_id_number, pcSearch)) {
        FRW_FREE (pcSearch);
        return i;
      }
    }
    else { 
      if (frw_con_rout_search_for_mime (frw_module_info[i].scheme, pcSearch)) {
        FRW_FREE (pcSearch);
        return i;
      }
    }
  }
  FRW_FREE (pcSearch);
  return -1;
}


static void
frw_send_to_external 
(
    WE_UINT8 uiDstModId, 
    WE_UINT8 uiSrcModId, 
    we_content_send_t* p
)
{
  if (frw_send_execute_command (uiDstModId, uiSrcModId, p->startOptions,
                                p->contentData, TRUE) == TRUE) {
    if (p->sendAck > 0)
      frw_content_send_ack (uiSrcModId, p->wid, WE_CONTENT_STATUS_SUCCEED, FALSE);
  }
  else {
    WE_BOOL uIsPipe = p->contentData && p->contentData->contentSource &&
                       (p->contentData->contentDataType == WE_CONTENT_DATA_PIPE);
    if (p->sendAck > 0)
      frw_content_send_ack (uiSrcModId, p->wid, WE_CONTENT_STATUS_FAILED, uIsPipe);
    else if (uIsPipe)
      WE_PIPE_DELETE (p->contentData->contentSource);
  }
}



static void
frw_content_send 
(
    WE_UINT8 uiSrcModId, 
    we_content_send_t* p
)
{
  we_dcvt_t stCcvtObj;
  WE_UINT8  uiModId = p->destModId;
  WE_BOOL   uIsPipe = p->contentData && p->contentData->contentSource &&
                       (p->contentData->contentDataType == WE_CONTENT_DATA_PIPE);

  
  if ((uiModId == WE_MODID_FRW) && (p->contentData != NULL)) {
    
    int tmp_id = frw_con_rout_get_module (p->contentData->routingFormat,
                                          p->contentData->routingIdentifier); 
    if (tmp_id != -1)
      uiModId = (WE_UINT8) tmp_id;
  }

  if ((uiModId != WE_MODID_FRW) && (uiModId < WE_NUMBER_OF_MODULES)) {
      
      WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                    "FRW: Content Routing: Send data to module %d\n", uiModId));
      if (frw_modules_startup[uiModId].startup_policy_depr == WE_STARTUP_EXTERNAL)
        frw_send_to_external (uiModId, uiSrcModId, p);
      else
        frw_start_module (uiModId, uiSrcModId, &(p->startOptions),
                          &(p->contentData), &(p->contentDataExists), TRUE, p->sendAck, p->wid);
  }
  else if ((p->contentData != NULL) && !(p->contentData->embeddedObjectExists) &&
           (p->contentData->routingFormat == WE_ROUTING_TYPE_MIME) && p->useObjectAction) {
    

    we_act_content_t act_content;

    act_content.mime_type = p->contentData->routingIdentifier;
    act_content.data_len = p->contentData->contentDataLength;
    switch (p->contentData->contentDataType) {
      case WE_CONTENT_DATA_RAW :  act_content.data_type = WeResourceBuffer; break;
      case WE_CONTENT_DATA_FILE : act_content.data_type = WeResourceFile;   break;
      case WE_CONTENT_DATA_PIPE : act_content.data_type = WeResourcePipe;   break;
      default : 
        act_content.data_type = WeResourceBuffer; act_content.data_len = 0; break;
    }
    act_content.data = p->contentData->contentData;
    act_content.src_path = p->contentData->contentSource;
    act_content.default_name = NULL;
    if (p->contentData->contentUrl != NULL)
    {
      
      char *s;
      char *f;
      s = we_url_get_path(WE_MODID_FRW, p->contentData->contentUrl);
      if (s != NULL)
      {
        f = strrchr (s, '/');
        if (f != NULL)
          act_content.default_name = we_url_unescape_string(WE_MODID_FRW, (f + 1)); 
        else
          act_content.default_name = we_url_unescape_string(WE_MODID_FRW, s);

        FRW_FREE (s);
      }
    }
    act_content.content_type = p->contentData->contentType;

    frw_object_action (&act_content,  NULL,  0,
                       ((WE_UINT8) (p->sendAck ? uiSrcModId : 0)), p->wid);     
    FRW_FREE(act_content.default_name);
  } 
  else if (p->sendAck > 0) {
    frw_content_send_ack (uiSrcModId, p->wid, WE_CONTENT_STATUS_FAILED, uIsPipe);
  }
  else if (uIsPipe) {
    
    WE_PIPE_DELETE (p->contentData->contentSource);
  }

  we_dcvt_init (&stCcvtObj, WE_DCVT_FREE, NULL, 0, WE_MODID_FRW);
  we_cvt_content_send (&stCcvtObj, p);
  FRW_FREE (p);
}


static void
frw_main (frw_signal_t *sig)
{
  switch (sig->type) {
    case FRW_MAIN_SIG_START_MODULE:
      WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                    "FRW: received signal FRW_MAIN_SIG_START_MODULE\n"));
      {
        we_dcvt_t          stCvtObj;
        we_module_start_t* p = (we_module_start_t*)sig->p_param;
        frw_start_module (p->modId, (WE_UINT8)(sig->i_param), &(p->startOptions),
                          &(p->contentData), &(p->contentDataExists), FALSE, FALSE, 0);
        we_dcvt_init (&stCvtObj, WE_DCVT_FREE, NULL, 0, WE_MODID_FRW);
        we_cvt_module_start (&stCvtObj, p);
        FRW_FREE (p);
      }
      break;

    case FRW_MAIN_SIG_TERMINATE_MODULE:
      WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                    "FRW: received signal FRW_MAIN_SIG_TERMINATE_MODULE\n"));
      frw_terminate_module ((WE_UINT8)sig->i_param, (WE_UINT8)sig->u_param1);
      break;

    case FRW_MAIN_SIG_TERMINATE:
      WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                    "FRW: received signal FRW_MAIN_SIG_TERMINATE\n"));
      frw_signal_delete (sig);
      sig = NULL;
      frw_terminate ();
      return;

    case FRW_MAIN_SIG_MODULE_STATUS_NOTIFY:
      WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                    "FRW: received signal FRW_MAIN_SIG_MODULE_STATUS_NOTIFY\n"));
      frw_module_status_notify ((WE_UINT8)sig->u_param1, sig->i_param, sig->p_param);
      break;
    case FRW_MAIN_SIG_MODULE_STATUS:
      WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                    "FRW: received signal FRW_MAIN_SIG_MODULE_STATUS\n"));
      frw_module_status ((WE_UINT8)sig->u_param1, sig->i_param, sig->p_param);
      break;

    case FRW_MAIN_SIG_REG_RESPONSE:
      WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                    "FRW: received signal FRW_MAIN_SIG_REG_RESPONSE\n"));
      frw_response (sig->p_param);
      break;

    case FRW_MAIN_SIG_CONTENT_SEND:
      WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                    "FRW: received signal FRW_MAIN_SIG_CONTENT_SEND\n"));
      frw_content_send ((WE_UINT8)(sig->i_param), sig->p_param);
      break;

    case FRW_MAIN_SIG_SUB_MOD_INITIALISED:
      WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                    "FRW: received signal FRW_MAIN_SIG_SUB_MOD_INITIALISED\n"));
      frw_init_done ();
      break;
    case FRW_MAIN_SIG_SUB_MOD_TERMINATED:
      WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                    "FRW: received signal FRW_MAIN_SIG_SUB_MOD_TERMINATED\n"));
      
      frw_signal_delete (sig);
      sig = NULL;
      frw_complete_exit();
      return;

    default:
      WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                    "FRW: received unknown signal: %d\n", sig->type));
  }
  frw_signal_delete (sig);
}


static void
frw_exception_handler (void)
{
    

  if (frw_inside_run)
    longjmp (frw_jmpbuf, 1);
}


static void
frw_mod_add_user_int 
(
    frw_mod_notify_int_t **list, 
    WE_UINT8 uiModid,
    int iRespDst, 
    int iRespSig
)
{
  frw_mod_notify_int_t *p = *list;
  frw_mod_user_int_t   *pstUser;

  while ((p != NULL) && (p->modid != uiModid))
    p = p->next;

    pstUser = FRW_ALLOCTYPE (frw_mod_user_int_t);
    pstUser->resp_dst = iRespDst;
    pstUser->resp_sig = iRespSig;
    pstUser->next     = NULL;

  if (p == NULL){
    p = FRW_ALLOCTYPE (frw_mod_notify_int_t);
    p->modid = uiModid;
    p->next  = NULL;
    p->users = pstUser;
    p->next = *list;
    *list = p;
  }
  else {
    pstUser->next = p->users;
    p->users = pstUser;
  }
}

static void
frw_mod_user_notify_int 
(
    frw_mod_notify_int_t **list, 
    WE_UINT8 uiModid
)
{
  frw_mod_notify_int_t *p    = *list;
  frw_mod_notify_int_t *prev = *list;
  frw_mod_user_int_t   *pstUser;

  while ((p != NULL) && (p->modid != uiModid)){
    prev = p;
    p = p->next;
  }

  if (p == NULL){
    return;
  }
  else {
    
    while (p->users != NULL){
      pstUser = p->users;
      FRW_SIGNAL_SENDTO_I (pstUser->resp_dst, pstUser->resp_sig, uiModid);
      p->users = pstUser->next;
      FRW_FREE (pstUser);
    }
    if (prev == p){
      
      *list = p->next;
    }
    else {
      prev->next = p->next;
    }
    FRW_FREE (p);
  }
}

static void
frw_start_module_list_destroy (frw_mod_notify_int_t **list)
{
  frw_mod_notify_int_t *p    = *list;
  frw_mod_user_int_t   *pstUser;
  
  while (p != NULL){
    *list = p->next;
    
    while (p->users != NULL){
      pstUser = p->users;
      p->users = pstUser->next;
      FRW_FREE (pstUser);
    }
    FRW_FREE (p);
    p = *list;
  }
}

void
frw_start_module_internal 
(
    WE_UINT8 uiModid, 
    int iRespDst, 
    int iRespSig
)
{
  char*               pcStartOptions = NULL;
  we_content_data_t* pstContent_data = NULL;
  WE_UINT8           uiContentDataExist = FALSE;

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                "FRW: Starting module: module=%d, \n", uiModid));

  if (frw_module_info[uiModid].status == MODULE_STATUS_ACTIVE){
    
    FRW_SIGNAL_SENDTO_I (iRespDst, iRespSig, uiModid);
  }
  else {
    
    frw_mod_add_user_int (&frw_started_modules_int, uiModid,
                          iRespDst, iRespSig);
    
    frw_start_module (uiModid, WE_MODID_FRW, 
                      &pcStartOptions, &pstContent_data,
                      &uiContentDataExist, FALSE, FALSE, 0);
  }
}

void
frw_module_status 
(
    WE_UINT8 uiModid, 
    int iStatus, 
    char* pcVersion
)
{
    WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                 "FRW: Module Status: module=%d, Status=%d\n",
                 uiModid, iStatus));

  switch (iStatus) {
    case MODULE_STATUS_CREATED:
      break;

    case MODULE_STATUS_ACTIVE:
      



      frw_mod_user_notify_int (&frw_started_modules_int, uiModid);
      break;

    case MODULE_STATUS_TERMINATED:
      break;
  } 
  FRW_FREE (pcVersion);
}





static void
frw_init_file_list (void)
{
  int i;

  for (i = 0; i < FRW_MAX_FILES; i++) {
      frw_files[i].file     = FRW_FILE_NOT_USED;
      frw_files[i].resp_dst = 0; 
      frw_files[i].resp_sig = 0;
  }
}


void
frw_file_notify (int file)
{
  int i;

  for (i = 0; i < FRW_MAX_FILES; i++) {
    if (frw_files[i].file == file){
      FRW_SIGNAL_SENDTO_I (frw_files[i].resp_dst, frw_files[i].resp_sig, file);
      return;
    }
  }
  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                    "FRW: WARNING File handle not registered.\n"));

}



void
frw_register_file_handle 
(
    int file, 
    int iRespDst, 
    int iRespSig
)
{
  int i;

  for (i = 0; i < FRW_MAX_FILES; i++) {
    if (frw_files[i].file == FRW_FILE_NOT_USED){
      frw_files[i].file     = file;
      frw_files[i].resp_dst = iRespDst; 
      frw_files[i].resp_sig = iRespSig;
      return;
    }
  }
      
  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                    "FRW: ERROR File handle registration failed.\n"));
}


void
frw_deregister_file_handle (int file)
{
  int i;

  for (i = 0; i < FRW_MAX_FILES; i++) {
    if (frw_files[i].file == file){
      frw_files[i].file     = FRW_FILE_NOT_USED;
      frw_files[i].resp_dst = 0; 
      frw_files[i].resp_sig = 0;
      return;
    }
  }
      
  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW,
                    "FRW: WARNING File handle not registered\n"));
}


void
frw_register_mod_term_notif (WE_UINT8 modid, int iRespDst, int iRespSig)
{
  frw_mod_add_user_int(&frw_notify_term_int, modid, iRespDst, iRespSig);
}


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





#ifndef FLDMGR_H
#define FLDMGR_H



typedef enum
{
	STATE_FOLDER_STOPPED = 0,   
	STATE_FOLDER_IDLE,          
	STATE_FOLDER_READING,       
	STATE_FOLDER_WRITING,       
	STATE_FOLDER_RESTORING,     
	STATE_FOLDER_TERMINATING    
} e_MmsFmState;


#define MMS_FILE        "mms"             
#define MMS_MMTTMP      "mms.tp"          
#define MMS_MMTBACK     "mms.bk"             
#define MMS_MMT         "mms.cn"          
#define MMS_INFO_SUFFIX "i"               
#define MMS_MESSAGE_SUFFIX "m"            
#define MMS_FOLDER      "/mms/"          
#define MMS_MMT_BACKUP_FILE MMS_FOLDER MMS_MMTBACK  
#define MMS_MMT_TMP_FILE MMS_FOLDER MMS_MMTTMP 
#define MMS_MMT_FILE    MMS_FOLDER MMS_MMT  
#define MMS_FILE_LEN    13                
#define MMS_PATH_LEN    (sizeof(MMS_FOLDER) + MMS_FILE_LEN)   



typedef struct
{
    MmsFileType     eSuffix;     
    MmsFolderType   eLocation;   
    WE_UINT32      uiSize;       
} st_MmsFmgCreateParam;



typedef struct
{
    MmsMsgId   uiMsgId;   
} st_MmsFmgDeleteParam;



typedef enum
{
    M_FSM_MMT_OPERATION_FINISHED,
    MMS_SIG_FMG_TERMINATE,
    MMS_SIG_FMG_CREATE_MSG,
    MMS_SIG_FMG_MOVE_MSG, 
    MMS_SIG_FMG_DELETE_MSG, 
    MMS_SIG_FMG_GET_LIST, 
    MMS_SIG_FMG_GET_MSG_INFO,
    MMS_SIG_FMG_GET_NO_MSG,
    MMS_SIG_FMG_INT_CLEAN_UP, 
    MMS_SIG_FMG_INT_NOTIF_WRITE_MMT,
    MMS_SIG_FMG_INT_WRITE_MMT,
    MMS_SIG_FMG_INT_MMT_RESTORE, 
    MMS_SIG_FMG_INT_CREATE_MSG,
    MMS_SIG_FMG_INT_DELETE_MSG,
    MMS_SIG_FMG_INT_INF_DONE,
    MMS_SIG_FMG_INT_NOTIF_DONE,
    MMS_SIG_FMG_INT_RR_PDU_RESTORE,
    MMS_SIG_FMG_INT_DR_RESTORE
} e_MmsFldrMgrSignalId;

void Mms_FldrEmergencyAbort(void);
void Mms_FldrInit(void);
e_MmsFmState Mms_FldrGetWorkingState(void);
char *Mms_FldrMsgIdToName
(
	WE_UINT32 uiMsgId, 
	MmsFileType eSuffix
);

void Mms_FldrTerminate
(
	MmsStateMachine eFsm,
	WE_UINT32 uiSignal,
	long lInstance
);

WE_BOOL Mms_FldrSetServerMsgId
(
	MmsMsgId uiMsgId,
	const char *pcServerMsgId
);

void fldrMgrSetHeader
(
	MmsMsgId uiMsgId,
	const MmsGetHeader *pstMhead,
	WE_UINT32 uiFileSize,
       const char *pcMessageId
);

void fldrMgrSetReadMark
(
	MmsMsgId uiMsgId,
	WE_UINT8 uiValue
);

WE_BOOL checkMsgId(WE_UINT32 uiMsgId);

MmsResult moveMessage
(
	WE_UINT32 uiFolder,
	WE_UINT32 uiMsgId
);

MmsFileType fldrGetFileSuffix(MmsMsgId uiMsgId);

WE_UINT32 fldrGetMsgSize(MmsMsgId uiMsgId);

void fldrMgrSetNotify
(
	MmsMsgId uiMsgId,
	const MmsNotification *pstMmsNotification,
	WE_UINT32 uiFileSize
);

unsigned char *fldrGetServerMsgId(MmsMsgId uiMsgId);

WE_UINT32 fldrUpdateMmtDelReport 
(
	WE_UINT32 uiMsgId,
	const char *pcSrvMsgId, 
	const char *pcAddr,
	WE_UINT32 uiDate,
	WE_UINT32 uiSize
);

WE_UINT32 fldrUpdateMmtReadReport
(
	WE_UINT32 uiMsgId, 
  	const MmsReadOrigInd *pstMmsOrigInd,
  	WE_UINT32 uiSize
);

WE_UINT32 fldrMgrSearchMsgId
(
	WE_UINT32 uiStartMsgId, 
	const char *pcSrvMsgId,
	MmsFileType eSuffix
);

WE_INT32 fldrMgrSearchIndex
(
	WE_UINT32 uiStartMsgId, 
	const char *pcSrvMsgId,
	MmsFileType eSuffix
);


void fldrMgrSetDRsent(WE_UINT32 uiMsgId);

void fldrMgrSetRRsent(WE_UINT32 uiMsgId);

WE_UINT32 fldrMgrSearchImmNotif(void);

WE_UINT32 mmsCountNotifInQueue(void);

MmsFileType getMsgType (WE_UINT32 uiMsgId);

void fldrGetFolderStatus
(
	WE_UINT32 *puiUsedMsgBytes,
	WE_UINT32 *puiNumOfMsg, 
       WE_UINT32 *puiTotalFolderSize,
       WE_UINT32 uiFolderMask
);

WE_UINT32 fldrGetNoOfFreeMsg(void);

void createFolder( void);

void fldrGetInboxStatus
(
	WE_UINT32 *puiNumOfMsg,
	WE_UINT32 *puiNumOfUnreadMsg,
	WE_UINT32 *puiNumOfNotif,
	WE_UINT32 *puiNumOfUnreadNotif,
	WE_UINT32 *puiNumOfRR,
	WE_UINT32 *puiNumOfUnreadRR,
	WE_UINT32 *puiNumOfDR,
	WE_UINT32 *puiNumOfUnreadDR
);






void fldrMgrGetNumberOfMessages
(
	WE_UINT16 uiFolderId,
	WE_UINT16 *puiMsgs,
	WE_UINT16 *puiUnreadMsgs
);
MmsResult fldrMgrDeleteMessage
(
	WE_UINT32 uiMsgId,
	WE_BOOL bExternal
);

MmsResult mmsChangeMsgType
(
	WE_UINT32 uiMsgId,
	MmsFileType eNewSuffix
);
MmsResult mmsFldrMgrSetFolderQuiet
(
	WE_UINT32 uiMsgId,
	WE_UINT32 uiFolder
);

void mmsShowAllImmediateNotif(void);













char *fldrDcf2Name
(
	WE_UINT32 uiMsgId,
	WE_UINT32 uiBp,
	char *pStr,
	int iLen
);







void fldrMgrSetHasDcf(MmsMsgId uiMsgId);

#endif 


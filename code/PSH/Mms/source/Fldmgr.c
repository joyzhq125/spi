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






































#include "We_Lib.h"    
#include "We_Cfg.h"    
#include "We_Def.h"     
#include "We_Log.h"     
#include "We_Mem.h"    
#include "We_Core.h"   
#include "We_File.h"    
#include "We_Dcvt.h"    
#include "We_Cmmn.h"    

#include "Mms_Cfg.h"    
#include "Mms_Def.h"    
#include "Mms_If.h"     
#include "Mms_Int.h"    

#include "Mmsrpl.h"     
#include "Msig.h"       
#include "Mmsrpl.h"      
#include "Mms_If.h"     
#include "Mmem.h"       
#include "Fldmgr.h"     
#include "Masync.h"     
#include "Mhandler.h"   
#include "Mcpdu.h"       
#include "Mutils.h"     
#include "Mlpdup.h"     
#include "Mnotify.h"    
#include "Mconfig.h"
#include "Mltypes.h"
#include "Mlfetch.h"

#ifdef MMS_UBS_IN_USE
#include "Ubs_If.h"     
#include "Mmsui.h"      
#endif


#ifdef WE_MODID_DRS
#include "Drs_If.h"     
#endif




#define RESTORE_FOLDER MMS_INBOX; 






typedef struct
{
    MmsStateMachine eFsm;    
    long lInstance;          
    WE_UINT32 uiSignal;      
} st_Caller;


typedef struct {
    char acFileName[MMS_FILE_LEN];
    WE_UINT32 uiMsgId;
    WE_UINT32 uiSize; 
} st_FileInFolder;





 
typedef struct 
{
    WE_INT32 uiCheckSum;             
    WE_UINT32 uiNextFreeMsgId;        
    MmsMessageFolderInfo msg[1];    
 
} st_MmtList;






#ifdef MMS_CUSTOMDATA_IN_USE
extern MmsCustomDataSpec g_mmsFldrCustomDataSpec[];
#endif









static struct st_FsmData {
    char acFileName[MMS_PATH_LEN];            
    AsyncOperationHandle pAsyncOperation;   
    AsyncOperationHandle pTmpAsyncOperation; 
    e_MmsFmState eState;                      
    st_Caller stMyCaller;                        
    WE_BOOL bNoBackupFiles;                 
    unsigned int uiBytesToWrite;             
    st_FileInFolder * pstFilesToRestore;           
    WE_INT32 iIdxToRestore;                
    WE_INT32 iNoToRestore;                 
    st_MmtList * pstMasterTable;                  
    unsigned int uiMmtLen;                   
}stFsmFldrData; 

static WE_BOOL s_Mms_bNotificationSave = FALSE;


static void Mms_ClearFileIndex(unsigned int i);
static WE_INT32 Mms_CreateMessage
(
	const MmsFileType eSuffix, 
	MmsFolderType eLocation, 
	WE_UINT32 uiSize
);
static int Mms_GetIndexFromMasterTable(WE_UINT32 uiMsgId);
static int Mms_GetNextFreeFileId(WE_UINT32 *puiMsgId);
static void Mms_GetNumberOfMsg
(
	WE_UINT8 uiOrderer, 
	MmsFolderType eFolderId
);
static void Mms_FldrMain(MmsSignal *pstSig);
static void Mms_HandleMmtOperationFinished
(
	MmsResult eResult,
	WE_UINT32 uiBytes
);
static MmsResult Mms_RemoveFile(const char *pcName);
static void Mms_SetCheckSum(void);
static int Mms_RenameFile
(
	const char *pcSourceName,
	const char *pcTargetName
);
static int Mms_DirectoryCleanup
(
	unsigned char ucCheck,
	const char *pcDirName
);
static void Mms_StartFileOperation(void);
static void Mms_StartWriteMmtFile(void);
static void Mms_RestartWriteMmtFile(void);
static void Mms_RestoreFiles(void);
static void Mms_RestoredFile(MmsResult eResult);
static void Mms_StartRestoringFiles(const char *pcDirName);
static WE_BOOL Mms_ValidateCheckSum(void);
static int Mms_GetBackUpFiles(void);
static int Mms_AddMsgIdToMmt(WE_UINT32 uiMsgId);
static char* Mms_GetFilePath
(
	const char *pcName,
	char *pcPathBuf
); 
static WE_UINT32 Mms_FileNameToMsgId(char *acFileName);
static WE_UINT8 Mms_GetNoOfAddress(const MmsGetHeader *pstHead);
static WE_BOOL Mms_FldrMgrFillInMsgProperties
(
	int iMsgIndex,
	MmsMessageProperties *pstProperties
);

MmsMessageFolderInfo *Mms_FldrMgrGetMsgInfo(WE_UINT32 uiMsgId);

#ifdef MMS_UBS_IN_USE
WE_BOOL WeMmsUbs_SetMessageData
(
	ubs_msg_list_item_t *outData,
	MmsMessageFolderInfo *msgInfo
);
#endif


#ifdef WE_LOG_MODULE 



const char *fsmFldrMgrSigName(WE_UINT32 uiSigType)
{
    switch (uiSigType)
    {
    case MMS_SIG_FMG_INT_CLEAN_UP:
        return "MMS_SIG_FMG_INT_CLEAN_UP";
    case MMS_SIG_FMG_INT_NOTIF_WRITE_MMT:
        return "MMS_SIG_FMG_NOTIF_WRITE_MMT";
    case MMS_SIG_FMG_INT_WRITE_MMT:
        return "MMS_SIG_FMG_INT_WRITE_MMT";
    case MMS_SIG_FMG_INT_MMT_RESTORE:
        return "MMS_SIG_FMG_INT_MMT_RESTORE";
    case MMS_SIG_FMG_INT_CREATE_MSG:
        return "MMS_SIG_FMG_INT_CREATE_MSG";
    case MMS_SIG_FMG_INT_DELETE_MSG:
        return "MMS_SIG_FMG_INT_DELETE_MSG";
    case MMS_SIG_FMG_INT_INF_DONE:
        return "MMS_SIG_FMG_INT_INF_DONE";
    case MMS_SIG_FMG_INT_NOTIF_DONE:
        return "MMS_SIG_FMG_INT_NOTIF_DONE";
    case MMS_SIG_FMG_INT_RR_PDU_RESTORE:
        return "MMS_SIG_FMG_INT_RR_PDU_RESTORE";
    case MMS_SIG_FMG_INT_DR_RESTORE:
        return "MMS_SIG_FMG_INT_DR_RESTORE";
    default:
        return 0;
    }
} 
#endif











void Mms_FldrEmergencyAbort(void)
{
    mSignalDeregister(M_FSM_FLDR_MANAGER);
} 

#ifdef MMS_CUSTOMDATA_IN_USE

WE_BOOL verifyCustomData()
{
	WE_BOOL bResult = TRUE;
	int i;
	int iSize = 0;
	int iTypeSize;


	for(i=0; g_mmsFldrCustomDataSpec[i].dataLen > 0;i++)
	{
		iSize += g_mmsFldrCustomDataSpec[i].dataLen;
		iTypeSize = -1;
		switch (g_mmsFldrCustomDataSpec[i].valueType)
		{
		case UBS_VALUE_TYPE_UINT32:
			iTypeSize = sizeof(WE_UINT32);
			break;
		case UBS_VALUE_TYPE_INT32:
			iTypeSize = sizeof(WE_INT32);
			break;
		case UBS_VALUE_TYPE_UTF8:
			iTypeSize = -1;
			break;
		case UBS_VALUE_TYPE_BOOL:
			iTypeSize = sizeof(WE_BOOL);
			break;
		case UBS_VALUE_TYPE_BINARY:
			iTypeSize = -1;
			break;
		}
		if (iTypeSize != -1 && iTypeSize != g_mmsFldrCustomDataSpec[i].dataLen)
		{
			WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
				"%s(%d): Warning custom data integrety error: uiSize entry %d faulty %d != %d\n", 
				__FILE__, __LINE__, i, iTypeSize, g_mmsFldrCustomDataSpec[i].dataLen)); 
			bResult = FALSE;
		}
		if (iSize > MMS_CUSTOM_DATA_SIZE)
		{
			bResult = FALSE;
		}

		if (bResult == TRUE)
		{
			g_mmsFldrCustomDataSpec[i].offset = iSize;
		}
		else
		{
			g_mmsFldrCustomDataSpec[i].offset = -1;            
		}
	}
	if (MMS_CUSTOM_DATA_SIZE != iSize)
	{
		WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
			"%s(%d): Warning custom data integrety error: uiSize faulty %d != %d\n", 
			__FILE__, __LINE__, iSize, MMS_CUSTOM_DATA_SIZE)); 
		return FALSE;
	}

	return bResult;
} 

#endif





void Mms_FldrInit(void)
{ 
    s_Mms_bNotificationSave = FALSE;

#ifdef MMS_CUSTOMDATA_IN_USE
	 if (FALSE == verifyCustomData())
	 {
		WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
			"%s(%d): Warning custom data configuration error please adjust Mms_Cfg.h!\n", 
			__FILE__, __LINE__));         
	 }
#endif

    memset ( &stFsmFldrData, 0, sizeof(stFsmFldrData));
    stFsmFldrData.eState = STATE_FOLDER_STOPPED;
    stFsmFldrData.iNoToRestore = -1; 
    stFsmFldrData.iIdxToRestore = -1; 
    stFsmFldrData.pstFilesToRestore = NULL; 
    
    mSignalRegisterDst(M_FSM_FLDR_MANAGER, Mms_FldrMain);

    stFsmFldrData.uiMmtLen = (sizeof(st_MmtList) + (sizeof(MmsMessageFolderInfo)
        * MMS_MAX_NO_OF_MSG)); 
    stFsmFldrData.pstMasterTable = (st_MmtList*)M_CALLOC(stFsmFldrData.uiMmtLen);  
  
    
    if (WE_FILE_GETSIZE (MMS_MMT_FILE) <= 0) 
    {
        
        if (WE_FILE_GETSIZE (MMS_MMT_TMP_FILE ) > 0)
        {
            
            if (Mms_RenameFile(MMS_MMT_TMP_FILE, MMS_MMT_FILE))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Failed to rename  file %s to file  %s!\n", 
                    __FILE__, __LINE__, MMS_MMT_FILE, MMS_MMT_BACKUP_FILE)); 
            } 
        } 
        else if (WE_FILE_GETSIZE (MMS_MMT_BACKUP_FILE ) > 0)
        {
            if (Mms_RenameFile(MMS_MMT_BACKUP_FILE, MMS_MMT_FILE))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Failed to rename  file %s to file  %s!\n", 
                    __FILE__, __LINE__, MMS_MMT_FILE, MMS_MMT_BACKUP_FILE)); 
            }  
        }
        else 
        {    
            



            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): No MMT files exist in the MMS folder, restore started !\n", 
                 __FILE__, __LINE__));

            stFsmFldrData.bNoBackupFiles = 1;             
            
            M_SIGNAL_SENDTO( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_INT_MMT_RESTORE);   
            
        }
        
    }  

    if (stFsmFldrData.bNoBackupFiles == 0)
    {
      Mms_StartFileOperation();
    } 
    
} 

e_MmsFmState Mms_FldrGetWorkingState(void)
{
    return stFsmFldrData.eState;
} 
static MmsResult fldrMsgDeleteDcf
(
	MmsMsgId uiMsgId,
	WE_BOOL bDeleteAll
)
{
    
    int i = 0; 
    int iNoOfFiles;
    int iType = 0;
    long lSize = 0;
    char acFileName[MMS_FILE_LEN];
    char acDcfName[MMS_FILE_LEN];    
    char acMsgName[MMS_FILE_LEN];    
    char acFullName[MMS_FILE_LEN];
    MmsResult eResult = MMS_RESULT_OK;
    MmsResult eRes;
    
    
    iNoOfFiles =  WE_FILE_GETSIZE_DIR(MMS_FOLDER);

    if (iNoOfFiles <= 0)
    {
        return MMS_RESULT_FILE_NOT_FOUND; 
    }  

    


    sprintf(acDcfName, "%lx_", uiMsgId);
    sprintf(acMsgName, "%lx.", uiMsgId);
    
    


    for (i = 0; i < iNoOfFiles; i++)
    {
        if ( (WE_FILE_READ_DIR(MMS_FOLDER, i, acFileName,
                 MMS_FILE_LEN, &iType, &lSize) == WE_FILE_OK))       
        {
            *acFullName = '\0';
            strcat(acFullName, MMS_FOLDER); 
            strcat(acFullName + strlen(MMS_FOLDER), acFileName);

            if ((strstr(acFileName, ".dcf") != 0) &&                  
                (strstr(acFileName, acDcfName) == acFileName))            
            {
                



                eRes = Mms_RemoveFile(acFullName);
                if (eRes != MMS_RESULT_OK)
                {
                    


                    eResult = eRes;
                }

                

#ifdef WE_MODID_DRS 
                DRSif_newDcfPath(WE_MODID_MMS, acFullName, NULL, DRS_ACT_DCF_REMOVED, DRS_OPTIONS_BIT_DELETE_RO);
#endif
            }
            else if ((bDeleteAll == TRUE) &&                          
                     (strstr(acFileName, acMsgName) == acFileName))        
            {
                eRes = Mms_RemoveFile(acFullName);
                if (eRes != MMS_RESULT_OK)
                {
                    eResult = eRes;
                }
            }
        } 
    }      

    return eResult;
} 
WE_UINT32 fldrUpdateMmtDelReport
(
	WE_UINT32 uiMsgId,
	const char *pcSrvMsgId, 
	const char *pcAddr,
	WE_UINT32 uiDate,
	WE_UINT32 uiSize
)
{
    int i = Mms_GetIndexFromMasterTable(uiMsgId); 
    unsigned int uiLen = 0; 
    WE_UINT32 uiRet = 0;
    int iMsgIndex = 0;
     
    if (i < 0)
    { 
        
       mmsNotifyError(MMS_RESULT_FILE_NOT_FOUND);
       return uiRet; 
    }  

	
	if (uiSize != 0)
	{
		stFsmFldrData.pstMasterTable->msg[i].size = uiSize; 
	} 

     
    if (uiDate != 0) 
    {
        stFsmFldrData.pstMasterTable->msg[i].date = uiDate; 
    } 

     
    if (pcAddr != NULL) 
    {
        strncpy(stFsmFldrData.pstMasterTable->msg[i].address, pcAddr, MAX_MMT_ADDRESS_LEN - 1); 
    }  

     
    if (pcSrvMsgId != NULL) 
    { 
        memset(stFsmFldrData.pstMasterTable->msg[i].serverMsgId, 0 ,MAX_MMT_SERVER_MSG_ID_LEN);
        strncpy(stFsmFldrData.pstMasterTable->msg[i].serverMsgId, pcSrvMsgId, 
            (MAX_MMT_SERVER_MSG_ID_LEN - 1)); 
        
        uiLen = strlen(pcSrvMsgId);

        if (uiLen > MAX_MMT_SERVER_MSG_ID_LEN) 
        {
            uiLen = MAX_MMT_SERVER_MSG_ID_LEN - 1; 
        } 


        
        iMsgIndex = fldrMgrSearchIndex(0, pcSrvMsgId, MMS_SUFFIX_SEND_REQ);
        if (0 <= iMsgIndex)
        {
           uiRet = stFsmFldrData.pstMasterTable->msg[iMsgIndex].msgId;    

           


           stFsmFldrData.pstMasterTable->msg[iMsgIndex].drRec++;
                        
           


           if (stFsmFldrData.pstMasterTable->msg[iMsgIndex].subject[0] != 0)
           {
               memset(stFsmFldrData.pstMasterTable->msg[i].subject,0,MAX_MMT_SUBJECT_LEN);
               strncpy(stFsmFldrData.pstMasterTable->msg[i].subject,
                    stFsmFldrData.pstMasterTable->msg[iMsgIndex].subject,MAX_MMT_SUBJECT_LEN-1);
           } 
        }
    }  
    return uiRet;    
} 










WE_UINT32 fldrUpdateMmtReadReport
(
	WE_UINT32 uiMsgId, 
  	const MmsReadOrigInd *pstMmsOrigInd,
  	WE_UINT32 uiSize
)
{   
    int i = Mms_GetIndexFromMasterTable(uiMsgId);
    unsigned int uiLen = 0; 
    WE_UINT32 uiRet = 0;
    int iMsgIndex = 0;
     
    if (i < 0)
    { 
        
       mmsNotifyError(MMS_RESULT_INSUFFICIENT_PERSISTENT_STORAGE); 

       WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
         "%s(%d): No space left to update the read report %d\n", 
          __FILE__, __LINE__, uiMsgId));
       return uiRet; 
    }  
   
     
    if (pstMmsOrigInd->from.address != NULL)
    {
       strncpy(stFsmFldrData.pstMasterTable->msg[i].address, pstMmsOrigInd->from.address,
           MAX_MMT_ADDRESS_LEN - 1); 
    } 

	
	if (uiSize != 0)
	{
		stFsmFldrData.pstMasterTable->msg[i].size = uiSize; 
	} 
   
     
    if (pstMmsOrigInd->date != 0) 
    {
        stFsmFldrData.pstMasterTable->msg[i].date = pstMmsOrigInd->date; 
    } 

     
     
    if (pstMmsOrigInd->serverMessageId != NULL) 
    {
        strncpy(stFsmFldrData.pstMasterTable->msg[i].serverMsgId, pstMmsOrigInd->serverMessageId,
            MAX_MMT_SERVER_MSG_ID_LEN -1);
        
        uiLen = strlen(pstMmsOrigInd->serverMessageId);

        if (uiLen > MAX_MMT_SERVER_MSG_ID_LEN) 
        {
            uiLen = MAX_MMT_SERVER_MSG_ID_LEN -1; 
        } 

        
        iMsgIndex = fldrMgrSearchIndex(0, pstMmsOrigInd->serverMessageId, MMS_SUFFIX_SEND_REQ);
        if (0 <= iMsgIndex)
        {
           uiRet = stFsmFldrData.pstMasterTable->msg[iMsgIndex].msgId;    

           if (pstMmsOrigInd->readStatus == MMS_READ_STATUS_READ)
           {
               


               stFsmFldrData.pstMasterTable->msg[iMsgIndex].rrRead++;
           }
           else if (pstMmsOrigInd->readStatus == MMS_READ_STATUS_DELETED_WITHOUT_BEING_READ)
           {
               


               stFsmFldrData.pstMasterTable->msg[iMsgIndex].rrDel++;
           }      
           
           


           if (stFsmFldrData.pstMasterTable->msg[iMsgIndex].subject[0] != 0)
           {
               memset(stFsmFldrData.pstMasterTable->msg[i].subject,0,MAX_MMT_SUBJECT_LEN);
             strncpy(stFsmFldrData.pstMasterTable->msg[i].subject,
                    stFsmFldrData.pstMasterTable->msg[iMsgIndex].subject,MAX_MMT_SUBJECT_LEN-1);
           } 
        }
    } 

    return uiRet; 
} 







WE_UINT32 fldrMgrSearchImmNotif(void)
{
    WE_UINT32 uiMsgFound = 0;
    int iMsgIndex = 0;
    
    for (iMsgIndex = 0; (iMsgIndex < MMS_MAX_NO_OF_MSG) && 
        (stFsmFldrData.pstMasterTable->msg[iMsgIndex].msgId != 0); iMsgIndex++)
    {
        if (stFsmFldrData.pstMasterTable->msg[iMsgIndex].suffix == MMS_SUFFIX_IMMEDIATE)
        {
            if (uiMsgFound == 0 ||
                stFsmFldrData.pstMasterTable->msg[iMsgIndex].msgId < uiMsgFound)
            {
                uiMsgFound = stFsmFldrData.pstMasterTable->msg[iMsgIndex].msgId;
            }
        }  
    }  
    
    return uiMsgFound;
} 






WE_UINT32 mmsCountNotifInQueue()
{
    WE_UINT32 uiNotifFound = 0;
    int iMsgIndex = 0;
    
    for (iMsgIndex = 0; (iMsgIndex < MMS_MAX_NO_OF_MSG) && 
        (stFsmFldrData.pstMasterTable->msg[iMsgIndex].msgId != 0); iMsgIndex++)
    {
        if (stFsmFldrData.pstMasterTable->msg[iMsgIndex].suffix == MMS_SUFFIX_IMMEDIATE)
        {
            uiNotifFound++;
        }  
    }  
    
    return uiNotifFound;
} 




void fldrMgrSetHasDcf(MmsMsgId uiMsgId)
{
    int i = Mms_GetIndexFromMasterTable(uiMsgId); 
    
    if (i < 0)
    {
        
        return; 
    }  
    
    stFsmFldrData.pstMasterTable->msg[i].drmStatus |= MMS_DRM_STATUS_DCF; 
}   
                      







void fldrMgrSetHeader
(
	MmsMsgId uiMsgId,
	const MmsGetHeader *pstMhead,
	WE_UINT32 uiFileSize,
	const char *pcMessageId
)
{   
    int i = Mms_GetIndexFromMasterTable(uiMsgId); 

    if (i < 0)
    {
       
       mmsNotifyError(MMS_RESULT_FILE_NOT_FOUND);
       return; 
    }  
     
    if (stFsmFldrData.eState == STATE_FOLDER_WRITING)
    {
        asyncOperationStop(&stFsmFldrData.pTmpAsyncOperation);
    }  

    if (pstMhead->date)
    {
      stFsmFldrData.pstMasterTable->msg[i].date = pstMhead->date; 
    }  

    stFsmFldrData.pstMasterTable->msg[i].drmStatus = (WE_UINT8) (((int) pstMhead->forwardLock << MMS_DRM_STATUS_FWDL_SHIFT) | 
                                                ((int)stFsmFldrData.pstMasterTable->msg[i].drmStatus) & MMS_DRM_STATUS_DCF); 
    stFsmFldrData.pstMasterTable->msg[i].priority = pstMhead->priority;
    
   


    memset(stFsmFldrData.pstMasterTable->msg[i].subject, 0, MAX_MMT_SUBJECT_LEN);
    if (pstMhead->subject.charset == MMS_UTF8 && pstMhead->subject.text != NULL)
    {   
        unsigned int uiSubjLen= strlen(pstMhead->subject.text); 
        
        if (uiSubjLen > MAX_MMT_SUBJECT_LEN - 1)
        {
            uiSubjLen = MAX_MMT_SUBJECT_LEN - 1;  
        } 
        memcpy(stFsmFldrData.pstMasterTable->msg[i].subject, pstMhead->subject.text, uiSubjLen);       
    }  

    



    if ((pstMhead->messageType == MMS_MSG_TYPE_SEND_REQ) && 
        (pstMhead->readReply == MMS_READ_REPLY_YES || 
        pstMhead->deliveryReport == MMS_DELIVERY_REPORT_YES)) 
    {
        WE_UINT8 noOfMsg = Mms_GetNoOfAddress(pstMhead); 
                   
        if (pstMhead->readReply == MMS_READ_REPLY_YES) 
        {
            stFsmFldrData.pstMasterTable->msg[i].rrReq = noOfMsg; 
        }  

        if (pstMhead->deliveryReport == MMS_DELIVERY_REPORT_YES) 
        { 
             stFsmFldrData.pstMasterTable->msg[i].drReq = noOfMsg; 
        }      
    } 

    stFsmFldrData.pstMasterTable->msg[i].size = uiFileSize;

    if (pstMhead->messageType == MMS_MSG_TYPE_SEND_REQ && pstMhead->to != NULL)
    {
    

        unsigned int uiAddrLen= strlen(pstMhead->to->current.address); 
        
        if (uiAddrLen > MAX_MMT_ADDRESS_LEN - 1)
        {
            uiAddrLen = MAX_MMT_ADDRESS_LEN - 1;  
        } 
        memset(stFsmFldrData.pstMasterTable->msg[i].address, 0, MAX_MMT_ADDRESS_LEN);
        memcpy(stFsmFldrData.pstMasterTable->msg[i].address, pstMhead->to->current.address, uiAddrLen);       
    }
    else if (pstMhead->messageType == MMS_MSG_TYPE_RETRIEVE_CONF && pstMhead->from.address != NULL)
    {
    

        unsigned int uiAddrLen= strlen(pstMhead->from.address); 
        
        if (uiAddrLen > MAX_MMT_ADDRESS_LEN - 1)
        {
            uiAddrLen = MAX_MMT_ADDRESS_LEN - 1;  
        } 
        memset(stFsmFldrData.pstMasterTable->msg[i].address, 0, MAX_MMT_ADDRESS_LEN);
        memcpy(stFsmFldrData.pstMasterTable->msg[i].address, pstMhead->from.address, uiAddrLen);        
    }

    if (pcMessageId != NULL)
    {
        strncpy(stFsmFldrData.pstMasterTable->msg[i].serverMsgId, pcMessageId, 
            MAX_MMT_SERVER_MSG_ID_LEN - 1);
    }  

    


#if (MMS_FLDRMGR_MSG_DATA_SET == 1)
    if (stFsmFldrData.eState != STATE_FOLDER_RESTORING)
    {
        M_SIGNAL_SENDTO( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_INT_WRITE_MMT);     
    } 
#endif 
} 









void fldrMgrSetNotify
(
	MmsMsgId uiMsgId,
	const MmsNotification *pstMmsNotification,
	WE_UINT32 uiFileSize
)
{
    int i = Mms_GetIndexFromMasterTable(uiMsgId); 

    if (i < 0)
    {
       
       mmsNotifyError(MMS_RESULT_INSUFFICIENT_PERSISTENT_STORAGE);
       return; 
    }  
    
    if (stFsmFldrData.eState == STATE_FOLDER_WRITING)
    {
        asyncOperationStop(&stFsmFldrData.pTmpAsyncOperation);
    }  

    



    stFsmFldrData.pstMasterTable->msg[i].date = mmsGetGMTtime(FALSE); 

   



    memset(stFsmFldrData.pstMasterTable->msg[i].subject, 0, MAX_MMT_SUBJECT_LEN); 
    if (pstMmsNotification->subject.charset == MMS_UTF8 &&
        pstMmsNotification->subject.text != NULL)
    {   
         
        strncpy(stFsmFldrData.pstMasterTable->msg[i].subject, 
            pstMmsNotification->subject.text, MAX_MMT_SUBJECT_LEN - 1);
    }  

    if (pstMmsNotification->from.address != NULL) 
    {
        strncpy(stFsmFldrData.pstMasterTable->msg[i].address, pstMmsNotification->from.address,
            MAX_MMT_ADDRESS_LEN - 1);
    }  

    stFsmFldrData.pstMasterTable->msg[i].size = uiFileSize;
    
    


    if (pstMmsNotification->deliveryReport == MMS_DELIVERY_REPORT_YES)
    {
        stFsmFldrData.pstMasterTable->msg[i].drReq = 1;
    }

} 







void fldrMgrSetReadMark
(
	MmsMsgId uiMsgId,
	WE_UINT8 uiValue
)
{    

    int i = Mms_GetIndexFromMasterTable(uiMsgId);

    if (i < 0)
    {
       return; 
    }  
        
    stFsmFldrData.pstMasterTable->msg[i].read = uiValue;
      
    if (stFsmFldrData.pstMasterTable->msg[i].folderId != MMS_HIDDEN)
    {
        WE_UINT32 uiNumOfMsg = 0;
        WE_UINT32 uiNumOfUnreadMsg = 0;
        WE_UINT32 uiNumOfNotif = 0;
        WE_UINT32 uiNumOfUnreadNotif = 0;
        WE_UINT32 uiNumOfRR = 0;
        WE_UINT32 uiNumOfUnreadRR = 0;
        WE_UINT32 uiNumOfDR = 0;
        WE_UINT32 uiNumOfUnreadDR = 0;

         
        fldrGetInboxStatus(&uiNumOfMsg, &uiNumOfUnreadMsg,
            &uiNumOfNotif, &uiNumOfUnreadNotif,
            &uiNumOfRR, &uiNumOfUnreadRR,
            &uiNumOfDR, &uiNumOfUnreadDR);
        MMSa_messageStatus(uiNumOfMsg, uiNumOfUnreadMsg,
            uiNumOfNotif, uiNumOfUnreadNotif,
            uiNumOfRR, uiNumOfUnreadRR,
            uiNumOfDR, uiNumOfUnreadDR); 
#ifdef MMS_UBS_IN_USE
        mmsNotifyUBSMsgChanged(uiMsgId);
        mmsNotifyUBSUnreadMsg(
            (WE_UINT16) (uiNumOfUnreadMsg+uiNumOfUnreadNotif+uiNumOfUnreadRR+uiNumOfUnreadDR));
#endif
    }
#if (MMS_FLDRMGR_READMARK == 1)
    M_SIGNAL_SENDTO( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_INT_WRITE_MMT);         
#endif
} 







MmsFileType fldrGetFileSuffix(MmsMsgId uiMsgId)
{ 
    int i = Mms_GetIndexFromMasterTable(uiMsgId);
  
    if (i < 0)
    {
       return MMS_SUFFIX_ERROR; 
    }  
    return stFsmFldrData.pstMasterTable->msg[i].suffix; 

} 










WE_UINT32 fldrGetMsgSize(MmsMsgId uiMsgId)
{
    int i = Mms_GetIndexFromMasterTable(uiMsgId);

    if (i < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): MsgId (%d) was not found in the MMT, get uiSize failed\n", 
            __FILE__, __LINE__, uiMsgId));
       return 0; 
    }  
    return stFsmFldrData.pstMasterTable->msg[i].size; 
} 









WE_BOOL Mms_FldrSetServerMsgId
(
	MmsMsgId uiMsgId,
	const char *pcServerMsgId
)
{
    int i = Mms_GetIndexFromMasterTable(uiMsgId);

    if (i < 0)
    { 
       
       mmsNotifyError(MMS_RESULT_INSUFFICIENT_PERSISTENT_STORAGE);
       return FALSE; 
    }  

    if (pcServerMsgId != NULL) 
    {
        memset(stFsmFldrData.pstMasterTable->msg[i].serverMsgId, 0,MAX_MMT_SERVER_MSG_ID_LEN);
        strncpy(stFsmFldrData.pstMasterTable->msg[i].serverMsgId, pcServerMsgId, 
            MAX_MMT_SERVER_MSG_ID_LEN - 1);
    } 

    return TRUE;
} 







unsigned char *fldrGetServerMsgId(MmsMsgId uiMsgId)
{ 
   int i = Mms_GetIndexFromMasterTable(uiMsgId); 
   
   if (i < 0)
   {
       return NULL; 
   }
   else 
   {
       return (unsigned char*)stFsmFldrData.pstMasterTable->msg[i].serverMsgId;
   }  

} 








WE_UINT32 fldrMgrSearchMsgId
(
	WE_UINT32 uiStartMsgId, 
	const char *pcSrvMsgId,
	MmsFileType eSuffix
)
{
    int iMsgIndex = 0;
    
    if (uiStartMsgId > 0)
    {
        iMsgIndex = Mms_GetIndexFromMasterTable(uiStartMsgId);

        if (iMsgIndex < 0)
        {
            


            return 0;
        }
    }   

    for (; (iMsgIndex < MMS_MAX_NO_OF_MSG) && (stFsmFldrData.pstMasterTable->msg[iMsgIndex].msgId != 0); iMsgIndex++)
    {
        if ((stFsmFldrData.pstMasterTable->msg[iMsgIndex].suffix == eSuffix) && 
            (stFsmFldrData.pstMasterTable->msg[iMsgIndex].serverMsgId[0] != 0))
        {
            if ( strcmp(pcSrvMsgId, stFsmFldrData.pstMasterTable->msg[iMsgIndex].serverMsgId) == 0)  
            {
                return stFsmFldrData.pstMasterTable->msg[iMsgIndex].msgId;
            }  
        }  
    }  
    
    return 0; 
} 









WE_INT32 fldrMgrSearchIndex
(
	WE_UINT32 uiStartMsgId, 
	const char *pcSrvMsgId,
	MmsFileType eSuffix
)
{
    int iMsgIndex = 0;
    
    if (uiStartMsgId > 0)
    {
        iMsgIndex = Mms_GetIndexFromMasterTable(uiStartMsgId);

        if (iMsgIndex < 0)
        {
            


            return 0;
        }
    }   

    for (; (iMsgIndex < MMS_MAX_NO_OF_MSG) && (stFsmFldrData.pstMasterTable->msg[iMsgIndex].msgId != 0); iMsgIndex++)
    {
        if ((stFsmFldrData.pstMasterTable->msg[iMsgIndex].suffix == eSuffix) && 
            (stFsmFldrData.pstMasterTable->msg[iMsgIndex].serverMsgId[0] != 0))
        {
            if ( strcmp(pcSrvMsgId, stFsmFldrData.pstMasterTable->msg[iMsgIndex].serverMsgId) == 0)  
            {
                return iMsgIndex;
            }  
        }  
    }  
    
    return -1; 
} 










static WE_BOOL Mms_FldrMgrFillInMsgProperties
(
	int iMsgIndex,
	MmsMessageProperties *pstProperties
)
{
    const char *pcSrvMsgId;
    int i;
    WE_UINT8   drNo = 0;
    WE_UINT8   rrNo = 0;
    
    if (iMsgIndex >= MMS_MAX_NO_OF_MSG ||
        stFsmFldrData.pstMasterTable->msg[iMsgIndex].msgId == 0)
    {
        return FALSE;
    }


    


    memcpy(&(pstProperties->msgFolderInfo),
           &(stFsmFldrData.pstMasterTable->msg[iMsgIndex]),
           sizeof(MmsMessageFolderInfo));

    pstProperties->rrListLength = 0;
    pstProperties->rrList = NULL;
    pstProperties->drListLength = 0;
    pstProperties->drList = NULL;

    


    if (stFsmFldrData.pstMasterTable->msg[iMsgIndex].serverMsgId[0] == 0)
    {
        return TRUE;
    }
    
    pcSrvMsgId = (const char *) stFsmFldrData.pstMasterTable->msg[iMsgIndex].serverMsgId;

    


    for (i = 0; (i < MMS_MAX_NO_OF_MSG) && (stFsmFldrData.pstMasterTable->msg[i].msgId != 0); i++)
    {
        if (((stFsmFldrData.pstMasterTable->msg[i].suffix == MMS_SUFFIX_READREPORT) ||
            (stFsmFldrData.pstMasterTable->msg[i].suffix == MMS_SUFFIX_DELIVERYREPORT)) &&
            (stFsmFldrData.pstMasterTable->msg[i].serverMsgId[0] != 0))
        {
            if ( strcmp(pcSrvMsgId, stFsmFldrData.pstMasterTable->msg[i].serverMsgId) == 0)  
            {
                if (stFsmFldrData.pstMasterTable->msg[i].suffix == MMS_SUFFIX_READREPORT)
                {
                    rrNo++;
                }
                else if (stFsmFldrData.pstMasterTable->msg[i].suffix == MMS_SUFFIX_DELIVERYREPORT)
                {
                    drNo++;
                }
            }  
        }  
    }  

    


    if (rrNo > 0)
    {
        pstProperties->rrList = M_CALLOC(sizeof(WE_UINT32)*rrNo);
    }
    else
    {
        pstProperties->rrList = NULL;
    }
    if (drNo > 0)
    {
        pstProperties->drList = M_CALLOC(sizeof(WE_UINT32)*drNo);
    }
    else
    {
        pstProperties->drList = NULL;
    }

    


    rrNo = 0;
    drNo = 0;
    for (i = 0; (i < MMS_MAX_NO_OF_MSG) && (stFsmFldrData.pstMasterTable->msg[i].msgId != 0); i++)
    {
        if (((stFsmFldrData.pstMasterTable->msg[i].suffix == MMS_SUFFIX_READREPORT) ||
            (stFsmFldrData.pstMasterTable->msg[i].suffix == MMS_SUFFIX_DELIVERYREPORT)) &&
            (stFsmFldrData.pstMasterTable->msg[i].serverMsgId[0] != 0))
        {
            if ( strcmp(pcSrvMsgId, stFsmFldrData.pstMasterTable->msg[i].serverMsgId) == 0)  
            {
                if (stFsmFldrData.pstMasterTable->msg[i].suffix == MMS_SUFFIX_READREPORT)
                {
                    pstProperties->rrList[rrNo] = stFsmFldrData.pstMasterTable->msg[i].msgId;
                    rrNo++;
                }
                else if (stFsmFldrData.pstMasterTable->msg[i].suffix == MMS_SUFFIX_DELIVERYREPORT)
                {
                    pstProperties->drList[drNo] = stFsmFldrData.pstMasterTable->msg[i].msgId;
                    drNo++;
                }
            }  
        }  
    }  

    return TRUE;
} 




void fldrMgrSetDRsent(WE_UINT32 uiMsgId)
{
    int iMsgIndex;

    for (iMsgIndex = 0; (iMsgIndex < MMS_MAX_NO_OF_MSG) && (stFsmFldrData.pstMasterTable->msg[iMsgIndex].msgId != 0); iMsgIndex++)
    {
        if (stFsmFldrData.pstMasterTable->msg[iMsgIndex].msgId == uiMsgId)
        {
            


            stFsmFldrData.pstMasterTable->msg[iMsgIndex].drReq = 1;
            stFsmFldrData.pstMasterTable->msg[iMsgIndex].drRec = 1;
            return ;
        }  
    }  
} 




void fldrMgrSetRRsent(WE_UINT32 uiMsgId)
{
    int iMsgIndex;

    for (iMsgIndex = 0; (iMsgIndex < MMS_MAX_NO_OF_MSG) && (stFsmFldrData.pstMasterTable->msg[iMsgIndex].msgId != 0); iMsgIndex++)
    {
        if (stFsmFldrData.pstMasterTable->msg[iMsgIndex].msgId == uiMsgId)
        {
            


            stFsmFldrData.pstMasterTable->msg[iMsgIndex].rrReq = 1;
			
			stFsmFldrData.pstMasterTable->msg[iMsgIndex].class = MMS_MESSAGE_CLASS_AUTO;
            return ;
        }  
    }  
} 





WE_UINT32 fldrGetNoOfFreeMsg(void)
{
    WE_UINT32 uiLoopInd = 0; 
    WE_UINT32 uiNumOfMsg = 0;

    for (uiLoopInd = 0; uiLoopInd < MMS_MAX_NO_OF_MSG && 
        (stFsmFldrData.pstMasterTable->msg[uiLoopInd].msgId != 0) ; uiLoopInd++)
    {
        uiNumOfMsg++; 
    } 
    return (MMS_MAX_NO_OF_MSG - uiNumOfMsg);
} 













void fldrGetFolderStatus
(
	WE_UINT32 *puiUsedMsgBytes,
	WE_UINT32 *puiNumOfMsg, 
       WE_UINT32 *puiTotalFolderSize,
       WE_UINT32 uiFolderMask
)
{
    int iFiles; 
    int i;
    MmsMessageFolderInfo *pstInfo = NULL;
    MmsMsgId uiMsgId;
    char acTmpName[MMS_PATH_LEN]; 
    long uiSize;
    int iType;
     
    *puiUsedMsgBytes = 0; 
    *puiNumOfMsg = 0;
    *puiTotalFolderSize = 0;

    *puiNumOfMsg = 0;

    for (i = 0; (i < MMS_MAX_NO_OF_MSG) && 
        (stFsmFldrData.pstMasterTable->msg[i].msgId != 0); i++)
    {
        if ((stFsmFldrData.pstMasterTable->msg[i].folderId & uiFolderMask) != 0)
        {
            *puiUsedMsgBytes += stFsmFldrData.pstMasterTable->msg[i].size;        
            *puiNumOfMsg = *puiNumOfMsg +1;
        }
    }  

     
    iFiles =  WE_FILE_GETSIZE_DIR (MMS_FOLDER);
    i = 0;

    if (iFiles >= 1) 
    {
       while  ((WE_FILE_READ_DIR(MMS_FOLDER, i, acTmpName,
            MMS_PATH_LEN, &iType, &uiSize) == WE_FILE_OK) && 
            (i < iFiles))
       {
           uiMsgId = Mms_FileNameToMsgId(acTmpName);

           if (uiMsgId > 0)
           {
               pstInfo = Mms_FldrMgrGetMsgInfo(uiMsgId);
               if (pstInfo != NULL && ((pstInfo->folderId & uiFolderMask) != 0))
               {
                   *puiTotalFolderSize = *puiTotalFolderSize + (WE_UINT32)uiSize; 
               }
           }
           i++; 
       }  
    }      
} 













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
)
{
    WE_UINT32 loopInd = 0; 
    WE_UINT32 isRead = 0;
    *puiNumOfMsg = 0;
    *puiNumOfUnreadMsg = 0;
    *puiNumOfNotif = 0;
    *puiNumOfUnreadNotif = 0;
    *puiNumOfRR = 0;
    *puiNumOfUnreadRR = 0;
    *puiNumOfDR = 0;
    *puiNumOfUnreadDR = 0;
    
    for (loopInd = 0; loopInd < MMS_MAX_NO_OF_MSG && 
        (stFsmFldrData.pstMasterTable->msg[loopInd].msgId != 0) ; loopInd++)
    {
        if (stFsmFldrData.pstMasterTable->msg[loopInd].msgId != 0)
        {
            
            if (stFsmFldrData.pstMasterTable->msg[loopInd].folderId != MMS_HIDDEN)
            {
                isRead = 1-stFsmFldrData.pstMasterTable->msg[loopInd].read;

                switch (stFsmFldrData.pstMasterTable->msg[loopInd].suffix)
                {
                case MMS_SUFFIX_MSG:
                    (*puiNumOfMsg)++;
                    (*puiNumOfUnreadMsg) += isRead;
                    break;
                case MMS_SUFFIX_NOTIFICATION:  
                    (*puiNumOfNotif)++;
                    (*puiNumOfUnreadNotif) += isRead;
                    break;
                case MMS_SUFFIX_READREPORT: 
                    (*puiNumOfRR)++;
                    (*puiNumOfUnreadRR) += isRead;
                    break;
                case MMS_SUFFIX_DELIVERYREPORT:
                    (*puiNumOfDR)++;
                    (*puiNumOfUnreadDR) += isRead;
                    break;
                case MMS_SUFFIX_IMMEDIATE:
                    
                    (*puiNumOfNotif)++;
                    (*puiNumOfUnreadNotif) += isRead;
                    break;
                case MMS_SUFFIX_SEND_REQ:
                case MMS_SUFFIX_INFO:
                case MMS_SUFFIX_TEMPLATE:
                case MMS_SUFFIX_ERROR:
                default:
                      break;
                }
            }
        } 
    } 
} 









void Mms_FldrTerminate
(
	MmsStateMachine eFsm,
	WE_UINT32 uiSignal,
	long lInstance
)
{
    stFsmFldrData.stMyCaller.eFsm = eFsm;
    stFsmFldrData.stMyCaller.uiSignal = uiSignal;
    stFsmFldrData.stMyCaller.lInstance = lInstance;

    M_SIGNAL_SENDTO( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_TERMINATE);
} 







MmsFileType getMsgType (WE_UINT32 uiMsgId)
{   
    WE_INT32 iIndex = 0; 

    if ((iIndex = Mms_GetIndexFromMasterTable(uiMsgId)) >= 0 ) 
    {
        return (MmsFileType)stFsmFldrData.pstMasterTable->msg[iIndex].suffix;
    }
    else 
    {
        return MMS_SUFFIX_ERROR;  
    }  
} 








WE_BOOL checkMsgId(WE_UINT32 uiMsgId)
{
    if (Mms_GetIndexFromMasterTable(uiMsgId) >= 0 ) 
    {

        return TRUE;   
    }
    else 
    {
        return FALSE;  
    }  

} 









MmsResult mmsFldrMgrSetFolderQuiet
(
	WE_UINT32 uiMsgId,
	WE_UINT32 uiFolder
)
{ 
    int iIndex = Mms_GetIndexFromMasterTable((WE_UINT32)uiMsgId);

    if (uiFolder >= MMS_ALL_FOLDERS) 
    {
       return MMS_RESULT_FOLDER_NOT_FOUND;
    }
    if (iIndex >= 0) 
    {
        stFsmFldrData.pstMasterTable->msg[iIndex].folderId = (MmsFolderType)uiFolder; 

        
 
        if (uiFolder == MMS_SENT) 
        {
          stFsmFldrData.pstMasterTable->msg[iIndex].sent = 1; 
        } 

         
        M_SIGNAL_SENDTO( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_INT_WRITE_MMT);
        
        return MMS_RESULT_OK;
    } 
    return MMS_RESULT_FILE_NOT_FOUND;
} 




void mmsShowAllImmediateNotif(void)
{
    WE_BOOL bChanged = FALSE;
    int i; 
    
    for (i = 0; (i < MMS_MAX_NO_OF_MSG) &&
        (stFsmFldrData.pstMasterTable->msg[i].msgId != 0) ; i++)
    {
        if (stFsmFldrData.pstMasterTable->msg[i].suffix == MMS_SUFFIX_IMMEDIATE &&
            stFsmFldrData.pstMasterTable->msg[i].folderId == MMS_HIDDEN)
        {
            stFsmFldrData.pstMasterTable->msg[i].folderId = MMS_NOTIFY;
            bChanged = TRUE;
        }
    } 
    
    if (bChanged == TRUE)
    {
#if (MMS_FLDRMGR_NOTIF == 1)
        

 
       M_SIGNAL_SENDTO( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_INT_WRITE_MMT);   
#endif
    }

} 







static MmsResult makeTemplate(WE_UINT32 uiMsgId)
{
    MmsResult eRes;
    int iIndex = Mms_GetIndexFromMasterTable(uiMsgId);

    if (iIndex >= 0) 
    {
        stFsmFldrData.pstMasterTable->msg[iIndex].folderId = MMS_TEMPLATES; 

        eRes = mmsChangeMsgType(uiMsgId, MMS_SUFFIX_TEMPLATE);

        if (eRes != MMS_RESULT_OK)
        {
            return eRes;
        }
            
#ifdef MMS_UBS_IN_USE
        


        mmsNotifyUBSMsgChanged(uiMsgId);
#endif            
        
#if (MMS_FLDRMGR_MOVE == 1)       
        


        M_SIGNAL_SENDTO( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_INT_WRITE_MMT);     
#endif        
        return MMS_RESULT_OK;
    } 

    return MMS_RESULT_FILE_NOT_FOUND;
} 








MmsResult moveMessage
(
	WE_UINT32 uiFolder,
	WE_UINT32 uiMsgId
)
{ 
    MmsResult eRes;
    int       iIndex = Mms_GetIndexFromMasterTable(uiMsgId);

    if (uiFolder == MMS_TEMPLATES)
    {
        if (stFsmFldrData.pstMasterTable->msg[iIndex].suffix == MMS_SUFFIX_SEND_REQ)
        {
            eRes = makeTemplate(uiMsgId);
            if (MMS_RESULT_OK != eRes)
            {
                return eRes;
            }
        }
        else
        {
            return MMS_RESULT_WRONG_FILE_TYPE;
        }
    }

    if (uiFolder >= MMS_ALL_FOLDERS) 
    {
       return MMS_RESULT_FOLDER_NOT_FOUND;
    }
    if (iIndex >= 0) 
    {
        MmsFolderType oldFolder = stFsmFldrData.pstMasterTable->msg[iIndex].folderId;
        
        stFsmFldrData.pstMasterTable->msg[iIndex].folderId = (MmsFolderType)uiFolder; 

        
 
        if (uiFolder == MMS_SENT) 
        {
          stFsmFldrData.pstMasterTable->msg[iIndex].sent = 1; 
        } 

        if (oldFolder != MMS_HIDDEN || uiFolder != MMS_HIDDEN)
        {
            WE_UINT32 uiNumOfMsg = 0;
            WE_UINT32 uiNumOfUnreadMsg = 0;
            WE_UINT32 uiNumOfNotif = 0;
            WE_UINT32 uiNumOfUnreadNotif = 0;
            WE_UINT32 uiNumOfRR = 0;
            WE_UINT32 uiNumOfUnreadRR = 0;
            WE_UINT32 uiNumOfDR = 0;
            WE_UINT32 uiNumOfUnreadDR = 0;          
            
             
            fldrGetInboxStatus(&uiNumOfMsg, &uiNumOfUnreadMsg,
                &uiNumOfNotif, &uiNumOfUnreadNotif,
                &uiNumOfRR, &uiNumOfUnreadRR,
                &uiNumOfDR, &uiNumOfUnreadDR);
            MMSa_messageStatus(uiNumOfMsg, uiNumOfUnreadMsg,
                uiNumOfNotif, uiNumOfUnreadNotif,
                uiNumOfRR, uiNumOfUnreadRR,
                uiNumOfDR, uiNumOfUnreadDR);
#ifdef MMS_UBS_IN_USE
            mmsNotifyUBSMsgChanged(uiMsgId);
            mmsNotifyUBSUnreadMsg(
                (WE_UINT16) (uiNumOfUnreadMsg+uiNumOfUnreadNotif+uiNumOfUnreadRR+uiNumOfUnreadDR));
#endif            
        }
        
#if (MMS_FLDRMGR_MOVE == 1)       
        M_SIGNAL_SENDTO( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_INT_WRITE_MMT);     
#endif        
        return MMS_RESULT_OK;
    } 
    return MMS_RESULT_FILE_NOT_FOUND;
} 






static MmsResult Mms_RemoveFile(const char *pcName)
{
    if (pcName == NULL) 
    {
        return MMS_RESULT_FILE_NOT_FOUND; 
    }  

    if( (WE_FILE_REMOVE(pcName) < 0)) 
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Warning: Failed to remove the file from disk.  %s!\n", 
            __FILE__, __LINE__, pcName));
        return MMS_RESULT_FILE_NOT_FOUND;
    } 
    
    return MMS_RESULT_OK;
} 













static void Mms_ClearFileIndex(unsigned int i)
{
    memset( &stFsmFldrData.pstMasterTable->msg[i], 0, sizeof(MmsMessageFolderInfo));
    stFsmFldrData.pstMasterTable->msg[i].msgId = 0; 

    if (i < (MMS_MAX_NO_OF_MSG - 1)) 
    {
        unsigned int uiNoToMove; 
        
        if (stFsmFldrData.pstMasterTable->msg[i+1].msgId == 0) 
        {
            return; 
        }
        for (uiNoToMove = 1;((uiNoToMove + i) < MMS_MAX_NO_OF_MSG) && 
            (stFsmFldrData.pstMasterTable->msg[i + uiNoToMove].msgId != 0); uiNoToMove++)
        {} 

        

        if (1 != uiNoToMove)
        {
            uiNoToMove--;
        }
        
        if ((uiNoToMove + i) < MMS_MAX_NO_OF_MSG)
        {
            memmove(&stFsmFldrData.pstMasterTable->msg[i], &stFsmFldrData.pstMasterTable->msg[i+1], 
                (sizeof(MmsMessageFolderInfo) * uiNoToMove));          
        } 
        
        
        if (uiNoToMove + i < MMS_MAX_NO_OF_MSG)
        {
            memset(&stFsmFldrData.pstMasterTable->msg[uiNoToMove + i], 0, 
                sizeof(MmsMessageFolderInfo));
        }
    } 
} 









static WE_INT32 Mms_CreateMessage
(
	const MmsFileType eSuffix,
	MmsFolderType eLocation,
       WE_UINT32 uiSize
)
{
    WE_UINT32 uiMsgId = 0;
    int iFileHandle; 
    int iIndex = Mms_GetNextFreeFileId(&uiMsgId);

    if (iIndex < 0) 
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Max number of MMS files reached!\n", 
            __FILE__, __LINE__));
    } 
    else 
    {
        stFsmFldrData.pstMasterTable->msg[iIndex].msgId = (WE_UINT32)uiMsgId;
        stFsmFldrData.pstMasterTable->msg[iIndex].folderId = eLocation;
        stFsmFldrData.pstMasterTable->msg[iIndex].suffix = eSuffix;

        
        if ((iFileHandle = WE_FILE_OPEN( WE_MODID_MMS, 
            Mms_FldrMsgIdToName((WE_UINT32)uiMsgId, eSuffix), 
            WE_FILE_SET_RDWR | WE_FILE_SET_CREATE, (long)uiSize)) < 0 )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Failed to create message file <%s>\n", 
                __FILE__, __LINE__, stFsmFldrData.acFileName));
            Mms_ClearFileIndex((WE_UINT32)iIndex); 
            return -1; 
        } 
        
       if (WE_FILE_CLOSE(iFileHandle) != WE_FILE_OK) 
       {
             WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Failed to close message file <%s>\n", 
                __FILE__, __LINE__, stFsmFldrData.acFileName));
             Mms_ClearFileIndex((WE_UINT32)iIndex); 
             return -1; 
       }    

#if (MMS_FLDRMGR_CREATE == 1)     
        M_SIGNAL_SENDTO( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_INT_WRITE_MMT);   
#endif
            
    } 

    return (WE_INT32)uiMsgId;
} 







static MmsResult deleteMessage
(
	WE_UINT32 uiMsgId,
	WE_BOOL bExternal
)
{
    MmsFileType eSuffix;
    MmsResult eRet = MMS_RESULT_FILE_NOT_FOUND;
    MmsFolderType eFolder;
    int iIndex = Mms_GetIndexFromMasterTable(uiMsgId); 
    MmsNotification      stNotif;
    MmsVersion eVer=0;

    if (iIndex >= 0) 
    {
        eSuffix = (MmsFileType)stFsmFldrData.pstMasterTable->msg[iIndex].suffix;
        
        memset(&stNotif,0,sizeof(stNotif));
        WE_UNUSED_PARAMETER(bExternal);
        WE_UNUSED_PARAMETER(eVer);
#if  MMS_EXTENDED_NOTIFYRESPIND_HANDLING==1
        
        if(eSuffix == MMS_SUFFIX_NOTIFICATION  && cfgGetInt(MMS_CFG_NETWORK_STATUS_BEARER) == MMS_BEARER_AVAILABLE && bExternal)
        {
            
            
            
            MmsResult eResult = MMS_RESULT_OK;
            MmsTransaction stMTHandle ;
            
            MmsMessage stMMHandle;
           
            
            stMMHandle.acFileName = M_ALLOC(MMS_PATH_LEN);
            if (sprintf( stMMHandle.acFileName, "%s%lx.%c", MMS_FOLDER, stFsmFldrData.pstMasterTable->msg[iIndex].msgId,eSuffix) > 
                (int) (MMS_PATH_LEN))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "deleteMessage. Error creating filename"));
                return MMS_RESULT_RESTART_NEEDED;
            } 
            
            stMMHandle.fileHandle = WE_FILE_OPEN( WE_MODID_MMS, stMMHandle.acFileName, WE_FILE_SET_RDWR, 0);
            
            
            
            
            stMTHandle.sizeOfBuf = WE_FILE_GETSIZE ( stMMHandle.acFileName);
            if ( stMTHandle.sizeOfBuf <= 0)
            {
                
                return MMS_RESULT_FILE_READ_ERROR;
            } 
            stMTHandle.buf = M_ALLOC( (WE_UINT32)stMTHandle.sizeOfBuf);
            if (stMTHandle.buf == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                    "Unable to allocate memory in: deleteMessage(msgId = %d)\n", stFsmFldrData.pstMasterTable->msg[iIndex].msgId));
                return MMS_RESULT_INSUFFICIENT_MEMORY;
            } 
            
            stMTHandle.bufferPos = stMTHandle.buf;
            stMTHandle.bytesLeft = (WE_UINT32)stMTHandle.sizeOfBuf;
            stMTHandle.fileHandle = stMMHandle.fileHandle;
            stMTHandle.isMessageFile = TRUE;
            stMTHandle.filePos = 0; 
            stMTHandle.state = 2;  
            stMTHandle.subState = 1;
            eResult = readDataFromFile( WE_MODID_MMS, 
                (unsigned char **)&stMTHandle.bufferPos, &stMTHandle.bytesLeft, 
                stMTHandle.fileHandle, &stMTHandle.filePos);
            if ( eResult != MMS_RESULT_OK )
            {
                
                return eResult;
            } 
            stMTHandle.state = 3;
            stMTHandle.bufferPos = stMTHandle.buf;
            
            eResult = parseMmsNotification( WE_MODID_MMS, stMTHandle.bufferPos,
                (WE_UINT32)stMTHandle.sizeOfBuf, &stNotif, &eVer);
            
            M_FREE(stMMHandle.acFileName);
            M_FREE(stMTHandle.buf);
            
        }
#endif 
        eRet = Mms_RemoveFile( Mms_FldrMsgIdToName(uiMsgId, eSuffix));
        
        if ((eSuffix == MMS_SUFFIX_MSG || eSuffix == MMS_SUFFIX_SEND_REQ ||
            eSuffix == MMS_SUFFIX_TEMPLATE) && 
            stFsmFldrData.pstMasterTable->msg[iIndex].size != 0 && eRet == MMS_RESULT_OK)
        {  
            
            eRet = Mms_RemoveFile( Mms_FldrMsgIdToName(uiMsgId, MMS_SUFFIX_INFO) );
            if (eRet == MMS_RESULT_FILE_NOT_FOUND)
            {
                eRet = MMS_RESULT_OK;
            }
        } 
     
#if     MMS_EXTENDED_NOTIFYRESPIND_HANDLING==1
        
        if(eSuffix == MMS_SUFFIX_NOTIFICATION && cfgGetInt(MMS_CFG_NETWORK_STATUS_BEARER) == MMS_BEARER_AVAILABLE && eRet == MMS_RESULT_OK && bExternal)
        {
            
            sendNotifyIndRsp(FALSE, stNotif.transactionId, MMS_STATUS_REJECTED, 
                stNotif.msgClass.classIdentifier, eVer);
            freeMmsNotification( WE_MODID_MMS, &stNotif);
            
        }
#endif
        


        if ((stFsmFldrData.pstMasterTable->msg[iIndex].drmStatus & MMS_DRM_STATUS_DCF) > 0)
        {
            eRet = fldrMsgDeleteDcf(uiMsgId, FALSE);
        }

        eFolder = stFsmFldrData.pstMasterTable->msg[iIndex].folderId;
        Mms_ClearFileIndex((WE_UINT32)iIndex);

        if (eRet == MMS_RESULT_OK)
        {            
            if (eFolder != MMS_HIDDEN)
            {
                WE_UINT32 uiNumOfMsg = 0;
                WE_UINT32 uiNumOfUnreadMsg = 0;
                WE_UINT32 uiNumOfNotif = 0;
                WE_UINT32 uiNumOfUnreadNotif = 0;
                WE_UINT32 uiNumOfRR = 0;
                WE_UINT32 uiNumOfUnreadRR = 0;
                WE_UINT32 uiNumOfDR = 0;
                WE_UINT32 uiNumOfUnreadDR = 0;

                 
                fldrGetInboxStatus(&uiNumOfMsg, &uiNumOfUnreadMsg,
                    &uiNumOfNotif, &uiNumOfUnreadNotif,
                    &uiNumOfRR, &uiNumOfUnreadRR,
                    &uiNumOfDR, &uiNumOfUnreadDR);
                MMSa_messageStatus(uiNumOfMsg, uiNumOfUnreadMsg,
                    uiNumOfNotif, uiNumOfUnreadNotif,
                    uiNumOfRR, uiNumOfUnreadRR,
                    uiNumOfDR, uiNumOfUnreadDR);
#ifdef MMS_UBS_IN_USE
                mmsNotifyUBSMsgDeleted(uiMsgId);
                mmsNotifyUBSUnreadMsg(
                    (WE_UINT16) (uiNumOfUnreadMsg+uiNumOfUnreadNotif+uiNumOfUnreadRR+uiNumOfUnreadDR));
#endif                
            }
        }
        
#if (MMS_FLDRMGR_DELETE == 1)
       M_SIGNAL_SENDTO( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_INT_WRITE_MMT);   
#endif 
        
    } 
    else  
    {        
         
         WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Requested to delete a file that was not found in MMT table <%d>\n", 
             __FILE__, __LINE__, uiMsgId));

         



         eRet = fldrMsgDeleteDcf(uiMsgId, TRUE);
    }  

    
    return eRet;

} 

MmsResult fldrMgrDeleteMessage
(
	WE_UINT32 uiMsgId,
	WE_BOOL bExternal
)
{
    return deleteMessage(uiMsgId,bExternal);
}









MmsResult mmsChangeMsgType
(
	WE_UINT32 uiMsgId,
	MmsFileType eNewSuffix
)
{
    MmsFileType eOldSuffix;
    char acOldFileName[MMS_PATH_LEN];
    
    int iIndex = Mms_GetIndexFromMasterTable(uiMsgId); 
    
    if (iIndex >= 0) 
    {
        eOldSuffix = (MmsFileType)stFsmFldrData.pstMasterTable->msg[iIndex].suffix;

        strcpy(acOldFileName, Mms_FldrMsgIdToName(uiMsgId, eOldSuffix));
        
        if (0 != WE_FILE_RENAME(acOldFileName, Mms_FldrMsgIdToName(uiMsgId, eNewSuffix)))
        {
            return MMS_RESULT_FILE_NOT_FOUND;
        }

        stFsmFldrData.pstMasterTable->msg[iIndex].suffix = eNewSuffix;
        return MMS_RESULT_OK;
    } 
    else
    {
        return MMS_RESULT_FILE_NOT_FOUND;
    }  
} 









static WE_UINT8 Mms_GetNoOfAddress(const MmsGetHeader *pstHead)
{  
    WE_UINT8 uiNoOfAddr = 0;
    MmsAddressList *pstTmpPtr; 

    
    pstTmpPtr = pstHead->to;
    if (pstTmpPtr!= NULL)
    {
        uiNoOfAddr ++;
        while (pstTmpPtr->next != NULL) 
        {
            uiNoOfAddr++; 
            pstTmpPtr = pstTmpPtr->next; 
        } 
    } 

     
    pstTmpPtr = pstHead->bcc;
    if (pstTmpPtr!= NULL)
    {
        uiNoOfAddr ++;
        while (pstTmpPtr->next != NULL) 
        {
            uiNoOfAddr++; 
            pstTmpPtr = pstTmpPtr->next; 
        } 
    } 

    
    pstTmpPtr = pstHead->cc;
    if (pstTmpPtr!= NULL)
    {
        uiNoOfAddr ++;
        while (pstTmpPtr->next != NULL) 
        {
            uiNoOfAddr++; 
            pstTmpPtr = pstTmpPtr->next; 
        } 
    } 

    return uiNoOfAddr; 
} 






static void finalTerminate(void)
{
 
    M_SIGNAL_SENDTO_I( stFsmFldrData.stMyCaller.eFsm, stFsmFldrData.stMyCaller.uiSignal, 
        stFsmFldrData.stMyCaller.lInstance);
    mSignalDeregister(M_FSM_FLDR_MANAGER);
    stFsmFldrData.eState = STATE_FOLDER_STOPPED;
    M_FREE(stFsmFldrData.pstMasterTable);
    stFsmFldrData.pstMasterTable = NULL;
    stFsmFldrData.uiMmtLen = 0; 
    stFsmFldrData.uiBytesToWrite = 0;
    stFsmFldrData.bNoBackupFiles = 0; 
} 

static void fldrSigCreateMessage
(
	WE_UINT8 uiOrderer,
	MmsFileType eFileType)
{
    WE_INT32 uiMsgId;
    MmsResult eResult; 
    
    if ((uiMsgId = Mms_CreateMessage(eFileType, MMS_HIDDEN, 0)) > 0) 
    {
        eResult = MMS_RESULT_OK; 
    }
    else 
    {
        eResult = MMS_RESULT_INSUFFICIENT_PERSISTENT_STORAGE; 
    } 
    
    replyCreateMessageResponse(uiOrderer, eResult, (MmsMsgId) uiMsgId); 
} 






static void Mms_FldrMain(MmsSignal *pstSig)
{
    WE_INT32 uiMsgId;
    MmsResult eReplyValue; 
    st_MmsFmgDeleteParam *pstDel;
    st_MmsFmgCreateParam *pstCreate;

     
    if (stFsmFldrData.eState == STATE_FOLDER_WRITING)
    { 
        





        switch (pstSig->type) 
        {
            case MMS_SIG_FMG_INT_WRITE_MMT:
#if (MMS_FLDRMGR_NOTIF == 1)
            case MMS_SIG_FMG_INT_NOTIF_WRITE_MMT:
#endif
#if (MMS_FLDRMGR_DELETE == 1)
            case MMS_SIG_FMG_CREATE_MSG: 
            case MMS_SIG_FMG_INT_CREATE_MSG:
#endif
#if (MMS_FLDRMGR_MOVE == 1)
            case MMS_SIG_FMG_MOVE_MSG: 
#endif
#if (MMS_FLDRMGR_DELETE == 1)
            case MMS_SIG_FMG_DELETE_MSG:
#endif
            case MMS_SIG_FMG_INT_CLEAN_UP:
                 asyncOperationStop(&stFsmFldrData.pTmpAsyncOperation);
                 stFsmFldrData.eState = STATE_FOLDER_IDLE;
                 break; 
            default:
                break;
        }
    } 

    switch (pstSig->type)
    {
    


    case M_FSM_MMT_OPERATION_FINISHED :
        Mms_HandleMmtOperationFinished( (MmsResult)pstSig->u_param1, pstSig->u_param2);
        break;
        
        


 
    case MMS_SIG_FMG_TERMINATE:    
         
        stFsmFldrData.iNoToRestore = -1; 
        stFsmFldrData.iIdxToRestore = -1; 

        if (stFsmFldrData.pstFilesToRestore != NULL) 
        {
            M_FREE(stFsmFldrData.pstFilesToRestore); 
            stFsmFldrData.pstFilesToRestore = NULL; 
        }  
       
        if (stFsmFldrData.eState == STATE_FOLDER_READING) 
        {
            
 
            asyncOperationStop(&stFsmFldrData.pAsyncOperation);
            finalTerminate();
        }
        else if (stFsmFldrData.eState == STATE_FOLDER_WRITING)
        {
            
            Mms_RestartWriteMmtFile();
        }
        else 
        {
            stFsmFldrData.eState = STATE_FOLDER_TERMINATING;
            Mms_StartWriteMmtFile();
        } 

        break; 
        
        


    case MMS_SIG_FMG_INT_NOTIF_WRITE_MMT:
            s_Mms_bNotificationSave = TRUE;
            
    case MMS_SIG_FMG_INT_WRITE_MMT:
        if (stFsmFldrData.eState != STATE_FOLDER_RESTORING)
        {            
            stFsmFldrData.eState = STATE_FOLDER_WRITING; 
            Mms_StartWriteMmtFile();
        }
        break;
        
        


 
    case MMS_SIG_FMG_CREATE_MSG:
        fldrSigCreateMessage((WE_UINT8) pstSig->u_param1, (MmsFileType) pstSig->i_param);
        break; 
        
    case MMS_SIG_FMG_INT_CREATE_MSG: 
        pstCreate = (st_MmsFmgCreateParam *)pstSig->p_param;

        if ((uiMsgId = Mms_CreateMessage( pstCreate->eSuffix, pstCreate->eLocation, 
            pstCreate->uiSize)) > 0) 
        {
            eReplyValue = MMS_RESULT_OK; 
        }
        else 
        {
            eReplyValue = MMS_RESULT_INSUFFICIENT_PERSISTENT_STORAGE; 
        } 

        M_SIGNAL_SENDTO_IUUP( (MmsStateMachine)pstSig->u_param1, pstSig->u_param2, 
            pstSig->i_param, eReplyValue, (WE_UINT32) uiMsgId, 
            we_cmmn_strdup( WE_MODID_MMS, stFsmFldrData.acFileName));
        M_FREE(pstSig->p_param);
        break; 
        
        

   
    case MMS_SIG_FMG_MOVE_MSG: 
        replyMoveMessageResponse((WE_UINT8) pstSig->u_param2, moveMessage((WE_UINT32)pstSig->i_param, pstSig->u_param1));
        break; 
        


    case MMS_SIG_FMG_GET_NO_MSG: 
        Mms_GetNumberOfMsg((WE_UINT8) pstSig->u_param1, (MmsFolderType)pstSig->i_param); 
        break;
        
        

 
    case MMS_SIG_FMG_DELETE_MSG: 
        eReplyValue = deleteMessage((WE_UINT32)pstSig->u_param1,FALSE);
        replyDeleteMessageResponse((WE_UINT8) pstSig->u_param2, eReplyValue);
        break; 
        
    case MMS_SIG_FMG_INT_DELETE_MSG: 
        pstDel = (st_MmsFmgDeleteParam *)pstSig->p_param;
        eReplyValue = deleteMessage(pstDel->uiMsgId,FALSE);
      
        
        if ((pstSig->u_param1 != 0) && (pstSig->u_param2 !=0))
        {
            M_SIGNAL_SENDTO_IU( (MmsStateMachine)pstSig->u_param1, pstSig->u_param2, 
                pstSig->i_param, eReplyValue); 
        }  

        if (pstDel != NULL) 
        {
            M_FREE(pstDel);
        } 
        break; 
        
        

 
    case MMS_SIG_FMG_GET_LIST:
        {
        




            WE_UINT32 uiFrom = pstSig->u_param1;
            WE_UINT32 uiTo = pstSig->u_param2;
            WE_UINT32 uiIndex; 
            unsigned int uiNumOfFiles = 0;
            MmsListOfProperties *pstProperties; 
            WE_UINT32 uiReciever;
            
             
            for (uiIndex = uiFrom; uiIndex < MMS_MAX_NO_OF_MSG && uiIndex <= uiTo; uiIndex++)
            { 
                if (stFsmFldrData.pstMasterTable->msg[uiIndex].msgId == 0)
                    break;
                if (stFsmFldrData.pstMasterTable->msg[uiIndex].folderId & (MmsFolderType)pstSig->i_param)
                {
                    ++uiNumOfFiles;
                } 
            } 

            if (uiNumOfFiles > 0)     
            {   
                pstProperties = M_ALLOCTYPE(MmsListOfProperties);
                pstProperties->msgPropertyList = M_CALLOC(sizeof(MmsMessageProperties)*uiNumOfFiles);
                pstProperties->noOfElements = uiNumOfFiles;
                
                uiNumOfFiles = 0;
                for (uiIndex = uiFrom; uiNumOfFiles < pstProperties->noOfElements && 
                    uiIndex < MMS_MAX_NO_OF_MSG && uiIndex <= uiTo; ++uiIndex)
                { 
                    if (stFsmFldrData.pstMasterTable->msg[uiIndex].msgId == 0)
                        break;
                    if (stFsmFldrData.pstMasterTable->msg[uiIndex].folderId & (MmsFolderType)pstSig->i_param)
                    {
                        


                        if (!Mms_FldrMgrFillInMsgProperties(uiIndex, &(pstProperties->msgPropertyList[uiNumOfFiles])))
                        {
                            break;
                        }

                        ++uiNumOfFiles;
                    } 
                } 
                pstProperties->noOfElements = uiNumOfFiles;
                uiReciever = (WE_UINT32) pstSig->p_param;
                
                replyFolderContent((WE_UINT8) uiReciever, pstProperties); 
                MMSif_freeMmsListOfProperties(pstProperties, WE_MODID_MMS);
                M_FREE(pstProperties);
            }
            else
            {  
                uiReciever = (WE_UINT32) pstSig->p_param;

                pstProperties = M_ALLOCTYPE(MmsListOfProperties);
                pstProperties->noOfElements = 0;
                replyFolderContent((WE_UINT8) uiReciever, pstProperties);
                MMSif_freeMmsListOfProperties(pstProperties, WE_MODID_MMS);
                M_FREE(pstProperties);
            }
            break;
        }

        

 
    case MMS_SIG_FMG_GET_MSG_INFO:
        {
            WE_UINT8  uiSource = (WE_UINT8) pstSig->u_param2;
            int iIndex; 
            MmsListOfProperties *pstProperties; 
            
            uiMsgId = (WE_INT32) pstSig->u_param1;
            

            iIndex = Mms_GetIndexFromMasterTable(uiMsgId);

            if (iIndex >= 0)     
            {   
                pstProperties = M_ALLOCTYPE(MmsListOfProperties);
                pstProperties->noOfElements = 1;
                pstProperties->msgPropertyList = M_ALLOCTYPE(MmsMessageProperties);
 
                


                Mms_FldrMgrFillInMsgProperties(iIndex, pstProperties->msgPropertyList);

                replyMsgInfo(uiSource, pstProperties); 
                
                MMSif_freeMmsListOfProperties(pstProperties, WE_MODID_MMS);
                M_FREE(pstProperties);
            }
            else
            {  
                pstProperties = M_ALLOCTYPE(MmsListOfProperties);
                pstProperties->noOfElements = 0;
                pstProperties->msgPropertyList = NULL;
                replyMsgInfo(uiSource, pstProperties);
                M_FREE(pstProperties);
            }
            break;
        }

        




 
    case MMS_SIG_FMG_INT_CLEAN_UP: 
        (void)Mms_DirectoryCleanup(1, MMS_FOLDER);
        stFsmFldrData.eState = STATE_FOLDER_IDLE;
        break;
      
        
    case MMS_SIG_FMG_INT_INF_DONE:
         Mms_RestoredFile((MmsResult)pstSig->u_param1); 
         break; 
    
         



    case MMS_SIG_FMG_INT_NOTIF_DONE: 
         
        {
            MmsNotification *pstMmsNotification = (MmsNotification*)pstSig->p_param; 

            if (pstSig->u_param1 == MMS_M_NOTIFICATION_IND && 
                 (MmsResult)pstSig->i_param == MMS_RESULT_OK &&
                 pstMmsNotification != NULL) 
            {
                         
                fldrMgrSetNotify(stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].uiMsgId, 
                    (MmsNotification*)pstSig->p_param, 
                    stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].uiSize);
            }
            else 
            {
                 WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                   "%s(%d): Failed to recover notification (%d)!\n",  __FILE__, __LINE__, 
                    stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].uiMsgId));
            }
 
              
            if (pstMmsNotification != NULL)
            {
                freeMmsNotification(WE_MODID_MMS, pstMmsNotification);
                M_FREE(pstSig->p_param); 
                pstSig->p_param = 0;
            } 
         } 

          
         Mms_RestoredFile((MmsResult)pstSig->i_param); 
         break; 
         
         


    case MMS_SIG_FMG_INT_RR_PDU_RESTORE: 
        if (pstSig->u_param1 == MMS_M_READ_ORIG_IND && 
                 (MmsResult)pstSig->i_param == MMS_RESULT_OK) 
        {
            (void)fldrUpdateMmtReadReport(
                stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].uiMsgId,
                (MmsReadOrigInd*)pstSig->p_param, 
                stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].uiSize);

            if ( pstSig->p_param)
            {
                freeMmsReadOrigInd( WE_MODID_MMS, 
                    (MmsReadOrigInd*)pstSig->p_param);
                M_FREE( pstSig->p_param);
                pstSig->p_param = NULL;
            } 
        } 
        else
        {              
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Failed to recover notification (%d)!\n",  
                __FILE__, __LINE__, 
                stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].uiMsgId));

            if(pstSig->p_param != NULL)
            {   
                freeMmsReadOrigInd( WE_MODID_MMS, (MmsReadOrigInd *)pstSig->p_param);
                M_FREE ( pstSig->p_param);
                pstSig->p_param = NULL;
            }  
        } 

        Mms_RestoredFile((MmsResult)pstSig->i_param); 
        break; 

        


    case MMS_SIG_FMG_INT_DR_RESTORE:
        {
            MmsDeliveryInd *pstMmsDelivery = (MmsDeliveryInd*)pstSig->p_param;

            if (pstSig->u_param1 == MMS_M_DELIVERY_IND && 
                (MmsResult)pstSig->i_param == MMS_RESULT_OK && 
                pstMmsDelivery != NULL) 
            {
                (void)fldrUpdateMmtDelReport (
                    stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].uiMsgId, 
                    pstMmsDelivery->messageId,pstMmsDelivery->to->address, pstMmsDelivery->date,
					stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].uiSize);               
            }
            else
            {              
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Failed to recover notification (%d)!\n",  __FILE__, __LINE__, 
                    stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].uiMsgId));
            } 
            
            if (pstMmsDelivery != NULL) 
            {
                freeMmsDeliveryInd( WE_MODID_MMS, pstMmsDelivery);
                M_FREE(pstMmsDelivery);
            }  
        
            Mms_RestoredFile((MmsResult)pstSig->i_param);  
        } 
        break; 

        


 
    case MMS_SIG_FMG_INT_MMT_RESTORE :
        stFsmFldrData.eState = STATE_FOLDER_RESTORING;

        
        memset( stFsmFldrData.pstMasterTable, 0, stFsmFldrData.uiMmtLen);
        stFsmFldrData.pstMasterTable->uiNextFreeMsgId = 1;
        
        Mms_StartRestoringFiles(MMS_FOLDER); 
        break; 

    default: 
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM FOLDER MGR: Received unknown uiSignal %d\n", pstSig->type));
        break;
    } 
    
    mSignalDelete(pstSig);
} 







static int Mms_GetIndexFromMasterTable(WE_UINT32 uiMsgId)
{
    int i; 
    
    for (i = 0; (i < MMS_MAX_NO_OF_MSG) &&
        (stFsmFldrData.pstMasterTable->msg[i].msgId != uiMsgId) ; i++)
    {
        
    } 

    if (i < MMS_MAX_NO_OF_MSG)
    {
        return i; 
    } 
    return -1;
} 








static int Mms_AddMsgIdToMmt(WE_UINT32 uiMsgId)
{
    unsigned int i; 
    unsigned int uiNoToMove;

     
    for (i = 0; (i < MMS_MAX_NO_OF_MSG) && (stFsmFldrData.pstMasterTable->msg[i].msgId < uiMsgId) 
           && (stFsmFldrData.pstMasterTable->msg[i].msgId != 0); i++) 
    {
            
    }  

    if (stFsmFldrData.pstMasterTable->msg[i].msgId == uiMsgId) 
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Warning: Duplicated messageId found during restore! %d \n", 
             __FILE__, __LINE__, stFsmFldrData.iNoToRestore));
        return -1; 
    }  

    if (i == (MMS_MAX_NO_OF_MSG - 1))
    {
         WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Warning:Low disk storage! only one msg can be stored %d \n", 
                __FILE__, __LINE__, stFsmFldrData.iNoToRestore));
        
    } 
    else if (i  >= MMS_MAX_NO_OF_MSG)
    {
         WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Error: To many files in the folder to restore! %d \n", 
            __FILE__, __LINE__, stFsmFldrData.iNoToRestore));
         mmsNotifyError(MMS_RESULT_INSUFFICIENT_PERSISTENT_STORAGE); 
         return -1; 
    } 

    if (i < (MMS_MAX_NO_OF_MSG - 1)) 
    {
        
        for (uiNoToMove = 1;   ((i + uiNoToMove) < MMS_MAX_NO_OF_MSG) 
            && (stFsmFldrData.pstMasterTable->msg[i + uiNoToMove].msgId != 0); uiNoToMove++)
        {} 

        
        if ((uiNoToMove + i) < MMS_MAX_NO_OF_MSG)
        {
            memmove(&stFsmFldrData.pstMasterTable->msg[i+1], &stFsmFldrData.pstMasterTable->msg[i], 
                sizeof(MmsMessageFolderInfo) * uiNoToMove);  
        }
    } 

    memset( &stFsmFldrData.pstMasterTable->msg[i], 0x00, sizeof(MmsMessageFolderInfo));
    stFsmFldrData.pstMasterTable->msg[i].msgId = uiMsgId; 
    return (int)i;
}  







static int Mms_GetNextFreeFileId(WE_UINT32 *puiMsgId)
{
    int i = 0;
    WE_BOOL bFound = TRUE;

    while (bFound)
    {
        *puiMsgId = stFsmFldrData.pstMasterTable->uiNextFreeMsgId;
        bFound = FALSE;
        for (i = 0; i < MMS_MAX_NO_OF_MSG && stFsmFldrData.pstMasterTable->msg[i].msgId != 0; ++i)
        {
            if (stFsmFldrData.pstMasterTable->msg[i].msgId == (*puiMsgId))
            {
                ++stFsmFldrData.pstMasterTable->uiNextFreeMsgId; 
                if (stFsmFldrData.pstMasterTable->uiNextFreeMsgId > 0xFFFF)
                {
                    stFsmFldrData.pstMasterTable->uiNextFreeMsgId = 1;
                }  
                bFound = TRUE;
                break;
            }
            
        } 
    }

    if (i >= MMS_MAX_NO_OF_MSG)
    {
        *puiMsgId = 0;
        return -1; 
    } 

    ++stFsmFldrData.pstMasterTable->uiNextFreeMsgId; 
    if (stFsmFldrData.pstMasterTable->uiNextFreeMsgId > 0xFFFF)
    {
        stFsmFldrData.pstMasterTable->uiNextFreeMsgId = 1;
    }  
    return i;
}  






static void Mms_GetNumberOfMsg
(
	WE_UINT8 uiOrderer,
	MmsFolderType eFolderId
)
{
    WE_UINT32 uiNum = 0; 
    int i;
    
    for (i = 0; i < MMS_MAX_NO_OF_MSG; i++)
    {
        if (stFsmFldrData.pstMasterTable->msg[i].msgId == 0)
            break;
        if ((eFolderId == MMS_ALL_FOLDERS)
            || (stFsmFldrData.pstMasterTable->msg[i].folderId == eFolderId))
        { 
            uiNum++;
        } 
    } 

    replyGetNumberOfMessagesResponse(uiOrderer, MMS_RESULT_OK, uiNum, eFolderId);
} 






void fldrMgrGetNumberOfMessages
(
	WE_UINT16 uiFolderId,
	WE_UINT16 *puiMsgs,
	WE_UINT16 *puiUnreadMsgs
)
{
    int i;
    (*puiMsgs) = 0; 
    (*puiUnreadMsgs) = 0; 
    
    for (i = 0; i < MMS_MAX_NO_OF_MSG; i++)
    {
        if (stFsmFldrData.pstMasterTable->msg[i].msgId == 0)
            break;
        if ((stFsmFldrData.pstMasterTable->msg[i].folderId & uiFolderId) != 0)
        { 
            if (stFsmFldrData.pstMasterTable->msg[i].read == 0)
            {
                (*puiUnreadMsgs)++;
            }
            (*puiMsgs)++;
        } 
    } 
} 







MmsMessageFolderInfo *Mms_FldrMgrGetMsgInfo(WE_UINT32 uiMsgId)
{
    int i;
    
    for (i = 0; i < MMS_MAX_NO_OF_MSG; i++)
    {
        if (stFsmFldrData.pstMasterTable->msg[i].msgId == 0)
        {
            return NULL;
        } 
        if (stFsmFldrData.pstMasterTable->msg[i].msgId == uiMsgId)
        {
            return &(stFsmFldrData.pstMasterTable->msg[i]);
        } 
    } 
    
    return NULL;    
} 

#ifdef MMS_UBS_IN_USE

static MmsStrFolderId      mmsFolderIdTable[] = MMS_STR_FOLDERS;








const char *fldMgrFolderToString(MmsFolderType eFolderType)
{
    int i = 0;
    while (mmsFolderIdTable[i].strFolderId != NULL)
    {
        if (mmsFolderIdTable[i].folderType == eFolderType)
        {
            return mmsFolderIdTable[i].strFolderId;
        }
        ++i;
    }

    return NULL;
} 

MmsFolderType fldMgrStringToFolder(const char *pcFolderId)
{
    int i = 0;
    while (mmsFolderIdTable[i].strFolderId != NULL)
    {
        if (we_cmmn_strcmp_nc(mmsFolderIdTable[i].strFolderId, pcFolderId) == 0)
        {
            return (MmsFolderType) mmsFolderIdTable[i].folderType;
        }
        ++i;
    }
    
    return MMS_ALL_FOLDERS; 
} 





WE_BOOL fldrMgrMsgCompareUbs
(
	WE_UINT16 uiFilterListSize,
       const ubs_key_value_t *pstFilterList,
       WE_UINT32 uiMsgId
)
{
    int i;
    MmsMessageFolderInfo *pstMsgInfo;
    WE_UINT32            uiValue;
    WE_UINT32            uiRValue;
    WE_BOOL              uiBValue;
    const char           *pcFolder;

    pstMsgInfo = Mms_FldrMgrGetMsgInfo(uiMsgId);

    


    if (pstMsgInfo == NULL || pstMsgInfo->folderId == MMS_HIDDEN)
    {
        return FALSE;
    }

    for(i=0; i<uiFilterListSize; i++)
    {
        switch (pstFilterList[i].key)
        {
        case UBS_MSG_KEY_FOLDER:                
            pcFolder = fldMgrFolderToString(pstMsgInfo->folderId);
            if (pcFolder != NULL && we_cmmn_strcmp_nc((char *)pstFilterList[i].value, pcFolder) != 0)
            {
                
                return FALSE;
            }
            break;
        case UBS_MSG_KEY_SUBTYPE:               
            uiValue = (WE_UINT32) *pstFilterList[i].value;
            if (uiValue != ((WE_UINT32) pstMsgInfo->suffix))
            {
                
                return FALSE;
            }
            break;
        case UBS_MSG_KEY_SUBJECT:               
            if (we_cmmn_strcmp_nc((char *)pstFilterList[i].value, pstMsgInfo->subject) != 0)
            {
                
                return FALSE;
            }            
            break;
        case UBS_MSG_KEY_FROM:                  
            if (we_cmmn_strcmp_nc((char *)pstFilterList[i].value, pstMsgInfo->address) != 0)
            {
                
                return FALSE;
            }            
            break;
        case UBS_MSG_KEY_TIMESTAMP:             
            uiValue = (WE_UINT32) *pstFilterList[i].value;
            if (uiValue != pstMsgInfo->date)
            {
                
                return FALSE;
            }
            break;
        case UBS_MSG_KEY_SIZE:                  
            uiValue = (WE_UINT32) *pstFilterList[i].value;
            if (uiValue != pstMsgInfo->size)
            {
                
                return FALSE;
            }
            break;
        case UBS_MSG_KEY_READ:                  
            uiBValue = (WE_BOOL) *pstFilterList[i].value;
            if (uiBValue != (WE_BOOL) (pstMsgInfo->read != 0))
            {
                
                return FALSE;
            }
            break;
        case UBS_MSG_KEY_FORWARD_LOCK:          
            uiBValue = (WE_BOOL) *pstFilterList[i].value;
            if (uiBValue != (WE_BOOL) ((pstMsgInfo->drmStatus & MMS_DRM_STATUS_FWDL) != 0))
            {
                
                return FALSE;
            }
            break;
        case UBS_MSG_KEY_PRIORITY:              
            uiValue = (WE_UINT32) *pstFilterList[i].value;
            if (pstMsgInfo->priority == MMS_PRIORITY_LOW &&
                uiValue != UBS_MSG_KEY_VALUE_PRIORITY_LOW)
            {
                return FALSE;
            } 
            else if (pstMsgInfo->priority == MMS_PRIORITY_NORMAL &&
                uiValue != UBS_MSG_KEY_VALUE_PRIORITY_NORMAL)
            {
                return FALSE;
            }
            else if (pstMsgInfo->priority == MMS_PRIORITY_HIGH &&
                uiValue != UBS_MSG_KEY_VALUE_PRIORITY_HIGH)
            {
                return FALSE;
            }
            break;
        case UBS_MSG_KEY_TO:                    
            if (we_cmmn_strcmp_nc((char *)pstFilterList[i].value, pstMsgInfo->address) != 0)
            {
                
                return FALSE;
            }            
            break;
        case UBS_MSG_KEY_REPORT_STATUS: 
            uiValue = (WE_UINT32) *pstFilterList[i].value;
            
            uiRValue = ((WE_UINT32) (pstMsgInfo->rrReq)) |
                (((WE_UINT32) (pstMsgInfo->rrDel)) << 8) |
                (((WE_UINT32) (pstMsgInfo->drReq)) << 16) |
                (((WE_UINT32) (pstMsgInfo->drRec)) << 24);
            


            if ((uiValue == 0) == (uiRValue == 0))
            {
                
                return FALSE;
            }
            break;
        
        case UBS_MSG_KEY_CREATED:               
        case UBS_MSG_KEY_EXPIRES:               
        case UBS_MSG_KEY_NBR_OF_ATTACHMENTS:    
        case UBS_MSG_KEY_URL:                   
        default:
            
            return FALSE;
        } 
    } 

    


    return TRUE;
} 







void fldrMgrGetNumberOfMessagesUbs
(
	WE_UINT16 uiFilterListSize,
	const ubs_key_value_t *pstFilterList,
	WE_UINT16 *puiMsgs,
	WE_UINT16 *puiUnreadMsgs
)
{
    int i;
    (*puiMsgs) = 0; 
    (*puiUnreadMsgs) = 0; 
    
    for (i = 0; i < MMS_MAX_NO_OF_MSG; i++)
    {
        if (stFsmFldrData.pstMasterTable->msg[i].msgId == 0)
            break;
        if (TRUE == fldrMgrMsgCompareUbs(uiFilterListSize, pstFilterList, stFsmFldrData.pstMasterTable->msg[i].msgId))
        { 
            if (stFsmFldrData.pstMasterTable->msg[i].read == 0)
            {
                (*puiUnreadMsgs)++;
            }
            (*puiMsgs)++;
        } 
    } 
} 


WE_UINT16 fldrMgrGetUbsData
(
	WE_UINT16 uiFilterListSize,
       const ubs_key_value_t *pstFilterList,
       WE_UINT16 uiMsgs,
       WE_UINT16 uiFirstMsg,
       ubs_msg_list_item_t *pstList
)
{
    int i;
    int iCount = 0;
    WE_UINT16 uiListindex = 0;

    if (pstList == NULL)
    {
        return 0;
    }
    
    for (i = 0; i < MMS_MAX_NO_OF_MSG; i++)
    {
        if (stFsmFldrData.pstMasterTable->msg[i].msgId == 0)
            break;
        if (TRUE == fldrMgrMsgCompareUbs(uiFilterListSize, pstFilterList, stFsmFldrData.pstMasterTable->msg[i].msgId))
        {
            if (iCount >= uiFirstMsg)
            {
                if (FALSE == WeMmsUbs_SetMessageData(&(pstList[uiListindex]), &(stFsmFldrData.pstMasterTable->msg[i])))
                {
                    


                    return uiListindex;
                }

                ++uiListindex;                
                if (uiListindex == uiMsgs)
                {
                    return uiListindex;
                }
            }
            ++iCount;
        } 
    } 

    return uiListindex;    
} 

#endif








static void Mms_HandleMmtOperationFinished
(
	MmsResult eResult,
	WE_UINT32 uiBytes
)
{
    switch (stFsmFldrData.eState)
    {
    case STATE_FOLDER_READING :
        if (eResult != MMS_RESULT_OK || 
            uiBytes != stFsmFldrData.uiMmtLen)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Failed to read the MMT file, eResult=%d, bytes=%d!\n",
                __FILE__, __LINE__, eResult, uiBytes));
        
            M_SIGNAL_SENDTO( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_INT_MMT_RESTORE);
            mmsNotifyError(MMS_RESULT_INDEX_FILE_ERROR); 
            return;
        } 
        

         
        asyncOperationStop(&stFsmFldrData.pAsyncOperation);

         
        if (!Mms_ValidateCheckSum())
        {
            if (Mms_GetBackUpFiles())
            {
            



                
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): None of the MMT-files could be used due to "
                    "checksum error or missing files "
                    "(folder error). \n", __FILE__, __LINE__));
                
                mmsNotifyError(MMS_RESULT_INDEX_FILE_ERROR);  
                
                memset( stFsmFldrData.pstMasterTable, 0, stFsmFldrData.uiMmtLen);
                stFsmFldrData.pstMasterTable->uiNextFreeMsgId = 1;            
                M_SIGNAL_SENDTO( M_FSM_FLDR_MANAGER, 
                    (int)MMS_SIG_FMG_INT_MMT_RESTORE);
            } 
        }  
        else 
        {  
            
            M_SIGNAL_SENDTO( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_INT_CLEAN_UP);
        }  
        break;
 
    case STATE_FOLDER_WRITING :
        if (stFsmFldrData.pTmpAsyncOperation != NULL)
        {
            asyncOperationStop(&stFsmFldrData.pTmpAsyncOperation);
        } 

        if (stFsmFldrData.pAsyncOperation != NULL)
        {
            asyncOperationStop(&stFsmFldrData.pAsyncOperation); 
        } 

        stFsmFldrData.eState = STATE_FOLDER_IDLE;


        if (TRUE == s_Mms_bNotificationSave)
        {
            M_SIGNAL_SENDTO( M_FSM_MSR_NOTIFY, (int)MMS_SIG_MSR_NOTIFY_MMT_RSP);
            s_Mms_bNotificationSave = FALSE;
        }

        if (eResult != MMS_RESULT_OK || 
            uiBytes != stFsmFldrData.uiBytesToWrite)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Failed to write the MMT file, eResult=%d, bytes=%d!\n",
                __FILE__, __LINE__, eResult, uiBytes));
            


            (void)Mms_RemoveFile(MMS_MMT_TMP_FILE);
            mmsNotifyError(MMS_RESULT_INDEX_FILE_ERROR); 
            return;
        } 

        




        stFsmFldrData.uiBytesToWrite = 0;

         
        if (!stFsmFldrData.bNoBackupFiles)
        {
            (void)Mms_RemoveFile(MMS_MMT_BACKUP_FILE);
        
            if (Mms_RenameFile(MMS_MMT_FILE, MMS_MMT_BACKUP_FILE))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Failed to rename  file %s to file  %s!\n", 
                    __FILE__, __LINE__, MMS_MMT_FILE, MMS_MMT_BACKUP_FILE)); 
            }  
        }  

        if (Mms_RenameFile(MMS_MMT_TMP_FILE, MMS_MMT_FILE) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                     "%s(%d): Failed to rename  file %s to file  %s!\n", 
                  __FILE__, __LINE__, MMS_MMT_FILE, MMS_MMT_BACKUP_FILE)); 
        } 

        stFsmFldrData.bNoBackupFiles = 0;
        stFsmFldrData.eState = STATE_FOLDER_IDLE;
        break;

    case STATE_FOLDER_TERMINATING :               
        if (stFsmFldrData.pTmpAsyncOperation != NULL)
        {
            asyncOperationStop(&stFsmFldrData.pTmpAsyncOperation);
        }
        if (stFsmFldrData.pAsyncOperation != NULL)
        {
            asyncOperationStop(&stFsmFldrData.pAsyncOperation); 
        }

        stFsmFldrData.eState = STATE_FOLDER_IDLE;

        if (eResult != MMS_RESULT_OK || 
            uiBytes != stFsmFldrData.uiBytesToWrite)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Failed to write the MMT file, eResult=%d, bytes=%d!\n",
                __FILE__, __LINE__, eResult, uiBytes));
        
            mmsNotifyError(MMS_RESULT_FILE_WRITE_ERROR); 
        } 
        else 
        {
            
            if (!stFsmFldrData.bNoBackupFiles)
            {
                (void)Mms_RemoveFile(MMS_MMT_BACKUP_FILE);
            
                if (Mms_RenameFile(MMS_MMT_FILE, MMS_MMT_BACKUP_FILE))
                {
                    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                        "%s(%d): Failed to rename  file %s to file  %s!\n", 
                        __FILE__, __LINE__, MMS_MMT_FILE, MMS_MMT_BACKUP_FILE)); 
                 }  
            }  
 
            if (Mms_RenameFile(MMS_MMT_TMP_FILE, MMS_MMT_FILE))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Failed to rename  file %s to file  %s!\n", 
                    __FILE__, __LINE__, MMS_MMT_FILE, MMS_MMT_BACKUP_FILE)); 
            } 
            
        }     
       
        

        finalTerminate();

        break;
    case STATE_FOLDER_RESTORING :
    case STATE_FOLDER_STOPPED :
    case STATE_FOLDER_IDLE :
    default :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Received MMT_OPERATION_FINISHED in wrong state %d\n",
            __FILE__, __LINE__, stFsmFldrData.eState));
        break;
    } 
} 











char *Mms_FldrMsgIdToName(WE_UINT32 uiMsgId, MmsFileType eSuffix)
{
    stFsmFldrData.acFileName[0] = '\0';    

    if (sprintf(stFsmFldrData.acFileName,"%s%lx.%c", MMS_FOLDER, uiMsgId, (char)eSuffix) >= 
        (int) sizeof(stFsmFldrData.acFileName))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Failed to create file name. Buffer overwritten.\n",  
            __FILE__, __LINE__));

        mmsNotifyError(MMS_RESULT_RESTART_NEEDED);
        return NULL;
    } 

    return stFsmFldrData.acFileName;
} 













char *fldrDcf2Name
(
	WE_UINT32 uiMsgId,
	WE_UINT32 uiBp,
	char *pStr,
	int iLen
)
{
    if (NULL == pStr)
    {
        return NULL;
    }
    pStr[0] = '\0';    

    if (sprintf(pStr, "%s%lx_%lx.dcf", MMS_FOLDER, uiMsgId, uiBp) >= iLen)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Failed to create DCF file name. Buffer overwritten.\n",  
            __FILE__, __LINE__));

        mmsNotifyError(MMS_RESULT_RESTART_NEEDED);
    } 

    return pStr;
} 






static int atox(const char *pcStr)        
{
    int iNum = 0;   
    int iNeg = 0;     

    while (*pcStr == '-') 
    {
        pcStr++;
        iNeg = !iNeg;
    } 
 
    while (*pcStr != '\0') 
    {
        if ( (*pcStr < '0') && (*pcStr > 'f') && ((*pcStr >= 'a') && (*pcStr <= '9')))                     
        {
            break;
        } 
                 
        if (*pcStr <= '9')
        {
            iNum = (16 * iNum) + (*pcStr - '0');
        }
        else
        {
            iNum = (16 * iNum) + (*pcStr - 87);
        }
        pcStr++;
    } 
 
    return (iNeg ? -iNum : iNum);
} 







static WE_UINT32 Mms_FileNameToMsgId(char *pcFName)
{
  char *pcDot; 
  char acTmpFileName[MMS_FILE_LEN]; 

  pcDot = strchr (pcFName,'.' );
  if (pcDot != NULL)
  {
      strncpy(acTmpFileName, pcFName, (unsigned int)pcDot - (unsigned int)pcFName);
      acTmpFileName[(int)pcDot - (int)pcFName] = '\0';
  
      return (WE_UINT32)atox(acTmpFileName);
  }
  else
  {
      return 0;
  } 
} 









static int Mms_DirectoryCleanup
(
	unsigned char ucCheck,
	const char *pcDirName
)
{
    
    int i = 0; 
    int j = 0; 
    int iNoOfItems = 0;
    int iType = 0;
    long uiSize = 0;
    st_FileInFolder *pstFolderFiles;
    char *pcFullName;
    int iNoOfFiles =  WE_FILE_GETSIZE_DIR (pcDirName);

    if (iNoOfFiles <= 0)
    {
        
        return 0; 
    }  
    
    


    for (iNoOfItems = 0; (iNoOfItems < MMS_MAX_NO_OF_MSG) &&
        (stFsmFldrData.pstMasterTable->msg[iNoOfItems].msgId != 0); iNoOfItems++)
    {
        
    }   

    if ((iNoOfFiles == (iNoOfItems + 2)) && !ucCheck) 
    {
       return 0; 
    }
    else if (!ucCheck) 
    {
       return 1; 
    }  
   
     
    


    pstFolderFiles = (st_FileInFolder*)M_CALLOC(sizeof(st_FileInFolder) *
        (unsigned int)iNoOfFiles);

    pcFullName = (char *)M_CALLOC(strlen(MMS_FOLDER) + MMS_FILE_LEN + 2);

    j = 0;
      
    for (i = 0; i < iNoOfFiles; i++)
    {
        if ( (WE_FILE_READ_DIR(pcDirName, i, pstFolderFiles[j].acFileName,
            MMS_FILE_LEN, &iType, &uiSize) == WE_FILE_OK) &&
            (we_cmmn_strcmp_nc(pstFolderFiles[j].acFileName, MMS_MMT) != 0) && 
            (we_cmmn_strcmp_nc(pstFolderFiles[j].acFileName, MMS_MMTBACK) != 0) &&
            (we_cmmn_strcmp_nc(pstFolderFiles[j].acFileName, MMS_MMTTMP) != 0)&&
            (strstr(pstFolderFiles[j].acFileName, ".dcf") == 0))
        {
           j++;   
        } 
    }  
     
    iNoOfFiles = j; 
    
     
    for (i = 0; i < iNoOfFiles; i++)
    {
        pstFolderFiles[i].uiMsgId = Mms_FileNameToMsgId(pstFolderFiles[i].acFileName);
            
        if (pstFolderFiles[i].uiMsgId > 0)
        {
            if (Mms_GetIndexFromMasterTable(pstFolderFiles[i].uiMsgId) == -1)                 
            {
                
                 *pcFullName = '\0';
                 strcat(pcFullName, MMS_FOLDER); 
                 strcat(pcFullName + strlen(MMS_FOLDER), pstFolderFiles[i].acFileName);
                 (void)Mms_RemoveFile (pcFullName);
                 pstFolderFiles[i].uiMsgId = 0;
            }        
        }  
        else 
        {    
            
             *pcFullName = '\0';
             strcat(pcFullName, MMS_FOLDER); 
             strcat(pcFullName + strlen(MMS_FOLDER), pstFolderFiles[i].acFileName);
             (void)Mms_RemoveFile (pcFullName);       
             pstFolderFiles[i].uiMsgId = 0;
        }          
    }  

    



    {
        WE_UINT32 thisId = stFsmFldrData.pstMasterTable->msg[0].msgId;

        for (i = 0; (i < MMS_MAX_NO_OF_MSG) && (i<iNoOfItems); )
        {
            int filesMatching = 0;
            int wantedFiles = 0;
            
            for (j = 0; (j < iNoOfFiles); j++)
            {
                if (pstFolderFiles[j].uiMsgId == thisId)
                {
                    filesMatching++;
                }
            }   

            switch (stFsmFldrData.pstMasterTable->msg[i].suffix)
            {
            case MMS_SUFFIX_SEND_REQ:
                



                if (stFsmFldrData.pstMasterTable->msg[i].folderId == MMS_HIDDEN)
                {
                    wantedFiles = 0;
                }
                else
                {
                    wantedFiles = 2;
                }
                break;
            case MMS_SUFFIX_TEMPLATE:
            case MMS_SUFFIX_MSG:
                wantedFiles = 2;
                break;
            case MMS_SUFFIX_ERROR:
            case MMS_SUFFIX_INFO:
                wantedFiles = 0;
                break;
            case MMS_SUFFIX_IMMEDIATE:
            case MMS_SUFFIX_NOTIFICATION:
            case MMS_SUFFIX_READREPORT:
            case MMS_SUFFIX_DELIVERYREPORT:
            default:
                wantedFiles = 1;
                break;
            }

            if (filesMatching != wantedFiles)  
            {   
                for (j = 0; (j < iNoOfFiles); j++)
                {
                    if (pstFolderFiles[j].uiMsgId == thisId)
                    {
                        
                        *pcFullName = '\0';
                        strcat(pcFullName, MMS_FOLDER); 
                        strcat(pcFullName + strlen(MMS_FOLDER), pstFolderFiles[j].acFileName);
                        (void)Mms_RemoveFile (pcFullName);
                        pstFolderFiles[i].uiMsgId = 0;
                    }
                }   

                Mms_ClearFileIndex((unsigned int) i);
                iNoOfItems--;
                thisId = stFsmFldrData.pstMasterTable->msg[i].msgId;
            } 
            else 
            {   
                i++; 
                thisId = stFsmFldrData.pstMasterTable->msg[i].msgId;
            } 
        } 
    }

    M_FREE(pstFolderFiles);
    M_FREE(pcFullName);
    return 1;
} 










static WE_INT32 getCheckSum
(
	const unsigned char *pcData,
	WE_UINT32 uiDataLength
)
{    
  WE_UINT32 uiSum = 0, i = 0;

  while (i < uiDataLength) {
    uiSum += *pcData++;
    i++;
  }
  return (uiSum % 256);
} 





static void Mms_SetCheckSum()
{ 
    stFsmFldrData.pstMasterTable->uiCheckSum = 
        getCheckSum( (unsigned char*) stFsmFldrData.pstMasterTable + sizeof(WE_INT32), 
           stFsmFldrData.uiMmtLen - sizeof(WE_INT32) ); 
} 





static WE_BOOL Mms_ValidateCheckSum()
{
    return (stFsmFldrData.pstMasterTable->uiCheckSum == 
        getCheckSum( (unsigned char*) stFsmFldrData.pstMasterTable + sizeof(WE_INT32), 
                      stFsmFldrData.uiMmtLen - sizeof(WE_INT32))); 
} 





static void Mms_StartFileOperation()
{
     stFsmFldrData.pAsyncOperation = asyncOperationStart( MMS_MMT_FILE, M_FSM_FLDR_MANAGER,
        0, M_FSM_MMT_OPERATION_FINISHED, ASYNC_MODE_READ_WRITE);

    if (stFsmFldrData.pAsyncOperation == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): asyncOperationStart Read operation failed "
            "Out of memory.\n", __FILE__, __LINE__));

        mmsNotifyError(MMS_RESULT_INSUFFICIENT_MEMORY);
    }
    else
    {
        stFsmFldrData.eState = STATE_FOLDER_READING;
        asyncRead( stFsmFldrData.pAsyncOperation, 0, stFsmFldrData.pstMasterTable, stFsmFldrData.uiMmtLen);
    } 
} 






static void Mms_StartWriteMmtFile()
{
    stFsmFldrData.pTmpAsyncOperation = asyncOperationStart( 
        MMS_MMT_TMP_FILE, M_FSM_FLDR_MANAGER,
        0, M_FSM_MMT_OPERATION_FINISHED, ASYNC_MODE_READ_WRITE);
        
    if (stFsmFldrData.pTmpAsyncOperation == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): asyncOperationStart Write operation failed. "
                "Increase available memory.\n", __FILE__, __LINE__));

        mmsNotifyError(MMS_RESULT_INSUFFICIENT_MEMORY);

     }
     else
     {
         
        Mms_SetCheckSum(); 
        
        stFsmFldrData.uiBytesToWrite = stFsmFldrData.uiMmtLen; 
        asyncWrite( stFsmFldrData.pTmpAsyncOperation, 0 , stFsmFldrData.pstMasterTable, 
            stFsmFldrData.uiBytesToWrite);
     } 
} 





static void Mms_RestartWriteMmtFile(void)
{
    asyncOperationStop(&stFsmFldrData.pTmpAsyncOperation);
    Mms_StartWriteMmtFile();
}









static int Mms_GetBackUpFiles()
{
     
    if (WE_FILE_GETSIZE (MMS_MMT_BACKUP_FILE ) > 0)
    {            
        if (Mms_RemoveFile(MMS_MMT_FILE) == MMS_RESULT_OK)
        {
            if (Mms_RenameFile(MMS_MMT_BACKUP_FILE, MMS_MMT_FILE) == 0)
            {             
                Mms_StartFileOperation();
                return 0 ; 
            } 
        }         
    } 

    if (WE_FILE_GETSIZE (MMS_MMT_TMP_FILE) > 0)
    {         
        if (Mms_RemoveFile(MMS_MMT_FILE) == MMS_RESULT_OK)
        {
            if (Mms_RenameFile(MMS_MMT_TMP_FILE, MMS_MMT_FILE) == 0)
            {
                Mms_StartFileOperation();
                return 0 ; 
            }           
        }  
        return 1; 
    }   
    else 
    {
        return 1;
    }
} 








static int Mms_RenameFile
(
	const char *pcSourceName, 
	const char *pcTargetName
)
{
    int iRet = WE_FILE_RENAME (pcSourceName, pcTargetName);
    
    switch (iRet)
    {
    case WE_FILE_ERROR_ACCESS:
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): File or directory specified in file rename operation could",
            " not be created.(access error). %s\n", __FILE__, __LINE__,pcTargetName));
        mmsNotifyError(MMS_RESULT_FILE_WRITE_ERROR);
        break;
    case WE_FILE_ERROR_PATH:
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): The path to the file or directory specified in file rename",
            "operation could not be found. (invalid path). %s\n",
            __FILE__, __LINE__,pcTargetName));
        mmsNotifyError(MMS_RESULT_FOLDER_NOT_FOUND); 
        break;
    case WE_FILE_ERROR_INVALID:
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): The specified filenames to FileRename operation is invalid . "
            "(invalid path). %s\n", __FILE__, __LINE__,pcTargetName));
        mmsNotifyError(MMS_RESULT_ERROR);  
        break;
    default: 
        break; 
    }
    return iRet; 
} 








static char* Mms_GetFilePath
(
	const char *pcName,
	char *pcPathBuf
) 
{
    strcpy(pcPathBuf, MMS_FOLDER); 
    strcat(pcPathBuf + strlen(MMS_FOLDER), pcName);
    return pcPathBuf;
} 






static void Mms_StartRestoringFiles(const char *pcDirName)
{  
    int i = 0;
    int j = 0;
    int iType = 0;
    long uiSize = 0;
    char cSuffix;
    char *ptr; 
    char acFullName[MMS_PATH_LEN + 2];
    char acFileName[MMS_FILE_LEN];

    stFsmFldrData.iIdxToRestore = -1;
    stFsmFldrData.iNoToRestore =  WE_FILE_GETSIZE_DIR (pcDirName);
    
    if (stFsmFldrData.iNoToRestore <= 0) 
    {
        stFsmFldrData.eState = STATE_FOLDER_IDLE;
        return; 
    }  

    stFsmFldrData.pstFilesToRestore = (st_FileInFolder*)M_CALLOC(sizeof(st_FileInFolder) * MMS_MAX_NO_OF_MSG);    
    
    if (stFsmFldrData.pstFilesToRestore != NULL) 
    {
      while (j < stFsmFldrData.iNoToRestore)
      {
          if (i >= MMS_MAX_NO_OF_MSG)
          {
              WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                  "%s(%d): Warning: To many files in the folder to restore! %d \n", 
                  __FILE__, __LINE__, stFsmFldrData.iNoToRestore));
              
              if (WE_FILE_READ_DIR(pcDirName, i, acFileName, MMS_FILE_LEN, &iType, &uiSize) == WE_FILE_OK)
              {
                  (void)Mms_RemoveFile(Mms_GetFilePath(acFileName, (char*)acFullName)); 
              }
          }
          else if (WE_FILE_READ_DIR(pcDirName, i, stFsmFldrData.pstFilesToRestore[i].acFileName, 
              MMS_FILE_LEN, &iType, &uiSize) != WE_FILE_OK)
          {
              WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                     "%s(%d): Warning: WE_FILE_READ_DIR Failed to read the", 
                     "MMS folder! %d \n", 
                     __FILE__, __LINE__, stFsmFldrData.iNoToRestore)); 
              break; 
          }
          else if (stFsmFldrData.pstFilesToRestore[i].acFileName[0] != 0) 
          {
               
              if ((ptr = strchr(stFsmFldrData.pstFilesToRestore[i].acFileName, '.')) != NULL)
              {
                  ptr++;
              }  
              
              if (ptr != NULL)
              {
                  cSuffix = *ptr;
                  if (cSuffix >= 'A' && cSuffix <= 'Z')
                  { 
                      cSuffix += 'a'-'A';
                  }
              }
              else
              {
                  cSuffix = MMS_SUFFIX_ERROR;
              }
              
              if ((ptr == NULL) || (cSuffix == MMS_SUFFIX_INFO) || (uiSize == 0))
              {
                  (void)Mms_RemoveFile(Mms_GetFilePath(stFsmFldrData.pstFilesToRestore[i].acFileName,
                      (char*)acFullName)); 
                  *stFsmFldrData.pstFilesToRestore[i].acFileName = '\0';
              }          
              else
              {
                  stFsmFldrData.pstFilesToRestore[i].uiMsgId = 
                      Mms_FileNameToMsgId(stFsmFldrData.pstFilesToRestore[i].acFileName);
                  stFsmFldrData.pstFilesToRestore[i].uiSize = (WE_UINT32)uiSize; 
                  i++; 
              }  
          }  

          j++;       
      }  

      stFsmFldrData.iNoToRestore = i;   
      stFsmFldrData.iIdxToRestore = 0;  
      Mms_RestoreFiles();    
    }  
} 





static void stopRestore()
{
    int i;
    
    stFsmFldrData.eState = STATE_FOLDER_IDLE;
    
      
    stFsmFldrData.iNoToRestore = -1; 
    stFsmFldrData.iIdxToRestore = -1; 
    if (stFsmFldrData.pstFilesToRestore != NULL) 
    {
        M_FREE(stFsmFldrData.pstFilesToRestore); 
        stFsmFldrData.pstFilesToRestore = NULL;
    }  
     
    stFsmFldrData.pstMasterTable->uiNextFreeMsgId = 0;
    
    for (i = 0;  (i < MMS_MAX_NO_OF_MSG) &&
        (stFsmFldrData.pstMasterTable->msg[i].msgId != 0); i++)
    {
        if (stFsmFldrData.pstMasterTable->msg[i].msgId > stFsmFldrData.pstMasterTable->uiNextFreeMsgId)
        {
            stFsmFldrData.pstMasterTable->uiNextFreeMsgId = stFsmFldrData.pstMasterTable->msg[i].msgId;
        } 
    }  
    
    
    stFsmFldrData.pstMasterTable->uiNextFreeMsgId = stFsmFldrData.pstMasterTable->uiNextFreeMsgId + 1; 
     
    M_SIGNAL_SENDTO( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_INT_WRITE_MMT);         
}






static void Mms_RestoreFiles()
{
    int iIndex = 0; 
    WE_UINT32 uiMsgId = 0;
    char cSuffix;
    char *ptr; 
    char acFullName[MMS_PATH_LEN + 2];
    WE_BOOL bLoopFlag = TRUE;  

    if ( (stFsmFldrData.iNoToRestore <= 0 || stFsmFldrData.iIdxToRestore < 0) 
        || (stFsmFldrData.iNoToRestore <= stFsmFldrData.iIdxToRestore))  
    {
        stopRestore();
        return; 
    } 
     
    do 
    {
         
        ptr = strchr(stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].acFileName, '.');
    
        if (ptr == NULL) 
        {
             
            (void)Mms_RemoveFile(Mms_GetFilePath(
                stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].acFileName,
                (char*)acFullName)); 

            stFsmFldrData.iIdxToRestore++;  
            bLoopFlag = TRUE; 
        }
        else 
        {
            ptr++;  
            cSuffix = *ptr;
            if (cSuffix >= 'A' && cSuffix <= 'Z')
            { 
                cSuffix += 'a'-'A';
            }

            switch (cSuffix)
            {  
                case MMS_SUFFIX_INFO:
                    




                    bLoopFlag = TRUE; 
                    stFsmFldrData.iIdxToRestore++;  
                    break;
               
                case MMS_SUFFIX_TEMPLATE:
                case MMS_SUFFIX_SEND_REQ:
                case MMS_SUFFIX_MSG:
                     
                    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                        "%s(%d): Restore a Send or Received message file - msgId: (%s)\n", 
                        __FILE__, __LINE__, stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].acFileName));

                    uiMsgId = stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].uiMsgId; 
                    
                    if (uiMsgId > 0) 
                    {
                        iIndex = Mms_AddMsgIdToMmt((WE_UINT32)uiMsgId);
                    } 

                    if ((iIndex < 0) || (uiMsgId == 0)) 
                    {
                        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                            "%s(%d): Failed to restore message !\n",  __FILE__, __LINE__));
                         
                       (void)Mms_RemoveFile(Mms_GetFilePath(
                           stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].acFileName, 
                           (char*)acFullName)); 

                       bLoopFlag = TRUE; 
                       stFsmFldrData.iIdxToRestore++;   
                    } 
                    else 
                    {
                        if (MMS_SUFFIX_TEMPLATE == cSuffix)
                        {
                            stFsmFldrData.pstMasterTable->msg[iIndex].folderId = MMS_TEMPLATES; 
                        }
                        else if (MMS_SUFFIX_SEND_REQ == cSuffix)
                        {
                            stFsmFldrData.pstMasterTable->msg[iIndex].folderId = MMS_DRAFTS; 
                        }
                        else
                        {
                            stFsmFldrData.pstMasterTable->msg[iIndex].folderId = RESTORE_FOLDER; 
                        }
                        stFsmFldrData.pstMasterTable->msg[iIndex].suffix = (MmsFileType) cSuffix;                        
                        


 
                        M_SIGNAL_SENDTO_IUU(M_FSM_MMH_HANDLER,
                            (int)M_FSM_MMH_INT_CREATE_INFO_LIST, M_FSM_FLDR_MANAGER
                            ,(WE_UINT32)uiMsgId, MMS_SIG_FMG_INT_INF_DONE);

                        bLoopFlag = FALSE; 
                    } 
                    break;  

                    


                case MMS_SUFFIX_NOTIFICATION:
                    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                        "%s(%d): Restore a Notification message file  - uiMsgId: (%s)\n", 
                        __FILE__, __LINE__, stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].acFileName));

                    uiMsgId = stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].uiMsgId; 
                    
                    if (uiMsgId > 0) 
                    {
                        iIndex = Mms_AddMsgIdToMmt(uiMsgId);
                    }  

                    if ((iIndex < 0) || (uiMsgId == 0)) 
                    {
                        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                            "%s(%d): Failed to restore notification  !\n",
                            __FILE__, __LINE__));
                        
                       (void)Mms_RemoveFile(Mms_GetFilePath(stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].acFileName,
                           (char*)acFullName)); 
                       bLoopFlag = TRUE; 
                       stFsmFldrData.iIdxToRestore++;   
                    } 
                    else 
                    {   
                        stFsmFldrData.pstMasterTable->msg[iIndex].folderId = MMS_NOTIFY; 
                        stFsmFldrData.pstMasterTable->msg[iIndex].suffix = MMS_SUFFIX_NOTIFICATION;
                        stFsmFldrData.pstMasterTable->msg[iIndex].size = 
                            stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].uiSize;

                        M_SIGNAL_SENDTO_IUU( M_FSM_MMH_HANDLER, (int)M_FSM_MMH_GET_HEADER, \
                                M_FSM_FLDR_MANAGER, (WE_UINT32)uiMsgId, MMS_SIG_FMG_INT_NOTIF_DONE);   
                        
                        bLoopFlag = FALSE;                    
                    }  
                    break; 
                    


                case MMS_SUFFIX_IMMEDIATE:
                    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                        "%s(%d): Restore a immediate retrieval notification message file  - uiMsgId: (%s)\n", 
                        __FILE__, __LINE__, stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].acFileName));

                    uiMsgId = stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].uiMsgId; 
                    
                    if (uiMsgId > 0) 
                    {
                        iIndex = Mms_AddMsgIdToMmt(uiMsgId);
                    }  

                    if ((iIndex < 0) || (uiMsgId == 0)) 
                    {
                        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                            "%s(%d): Failed to restore immediate notification  !\n",
                            __FILE__, __LINE__));
                        
                       (void)Mms_RemoveFile(Mms_GetFilePath(stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].acFileName,
                           (char*)acFullName)); 
                       bLoopFlag = TRUE; 
                       stFsmFldrData.iIdxToRestore++;   
                    } 
                    else 
                    {   
                        stFsmFldrData.pstMasterTable->msg[iIndex].folderId = MMS_NOTIFY; 
                        stFsmFldrData.pstMasterTable->msg[iIndex].suffix = MMS_SUFFIX_IMMEDIATE;
                        stFsmFldrData.pstMasterTable->msg[iIndex].size = 
                            stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].uiSize;

                        M_SIGNAL_SENDTO_IUU( M_FSM_MMH_HANDLER, (int)M_FSM_MMH_GET_HEADER, \
                                M_FSM_FLDR_MANAGER, (WE_UINT32)uiMsgId, MMS_SIG_FMG_INT_NOTIF_DONE);   
                        
                        bLoopFlag = FALSE;                    
                    }  
                    break; 

                case MMS_SUFFIX_READREPORT:
                    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                        "%s(%d): Restore a Read Report (PDU) message file  - uiMsgId: (%s)\n", 
                        __FILE__, __LINE__, stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].acFileName));

                    uiMsgId = stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].uiMsgId; 
                    
                    if (uiMsgId > 0) 
                    {
                        iIndex = Mms_AddMsgIdToMmt(uiMsgId);
                    }  

                    if ((iIndex < 0) || (uiMsgId == 0)) 
                    {
                        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                            "%s(%d): Failed to restore read report orig pdu  !\n",  __FILE__, __LINE__));
                        
                       (void)Mms_RemoveFile(Mms_GetFilePath(stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].acFileName,
                           (char*)acFullName)); 
                       bLoopFlag = TRUE; 
                       stFsmFldrData.iIdxToRestore++;   
                    } 
                    else 
                    {
                        stFsmFldrData.pstMasterTable->msg[iIndex].folderId = RESTORE_FOLDER; 
                        stFsmFldrData.pstMasterTable->msg[iIndex].suffix = MMS_SUFFIX_READREPORT;
                        stFsmFldrData.pstMasterTable->msg[iIndex].size = 
                            stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].uiSize;
                    
                        M_SIGNAL_SENDTO_IUU( M_FSM_MMH_HANDLER, (int)M_FSM_MMH_GET_HEADER, \
                             M_FSM_FLDR_MANAGER, (WE_UINT32)uiMsgId, MMS_SIG_FMG_INT_RR_PDU_RESTORE);   
                        
                        bLoopFlag = FALSE; 
                    } 
                    
                    break; 

                case MMS_SUFFIX_DELIVERYREPORT:
                    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                        "%s(%d): Restore a Delivery Report (PDU) message file  - uiMsgId: (%s)\n", 
                        __FILE__, __LINE__, stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].acFileName));

                    uiMsgId = stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].uiMsgId; 
                    
                    if (uiMsgId > 0) 
                    {
                        iIndex = Mms_AddMsgIdToMmt(uiMsgId);
                    }  

                    if ((iIndex < 0) || (uiMsgId == 0)) 
                    {
                        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                            "%s(%d): Failed to restore read delivery report!\n",  __FILE__, __LINE__));
                        
                       (void)Mms_RemoveFile(Mms_GetFilePath(
                           stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].acFileName, 
                           (char*)acFullName)); 
                       bLoopFlag = TRUE; 
                       stFsmFldrData.iIdxToRestore++;   
                    } 
                    else 
                    {
                        stFsmFldrData.pstMasterTable->msg[iIndex].folderId = RESTORE_FOLDER; 
                        stFsmFldrData.pstMasterTable->msg[iIndex].suffix = MMS_SUFFIX_DELIVERYREPORT;
                        stFsmFldrData.pstMasterTable->msg[iIndex].size = 
                            stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].uiSize;
                    
                        M_SIGNAL_SENDTO_IUU( M_FSM_MMH_HANDLER, (int)M_FSM_MMH_GET_HEADER, \
                                M_FSM_FLDR_MANAGER, uiMsgId, MMS_SIG_FMG_INT_DR_RESTORE);   
                        
                        bLoopFlag = FALSE; 
                    } 
                    
                    break; 

                default: 
                    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                        "%s(%d): Could not recover this file Not supported file type  - uiMsgId: (%s)\n", 
                        __FILE__, __LINE__, stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].acFileName));

                    
                    (void)Mms_RemoveFile(Mms_GetFilePath(stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].acFileName,
                        (char*)acFullName)); 
                    bLoopFlag = TRUE; 
                    stFsmFldrData.iIdxToRestore++;   
                    break;
            } 
        }   
    } while (bLoopFlag && (stFsmFldrData.iNoToRestore > stFsmFldrData.iIdxToRestore));
    
    if ( TRUE == bLoopFlag)  
    {
        stopRestore();
    } 
} 










static void Mms_RestoredFile(MmsResult eResult)
{
    
    if (eResult != MMS_RESULT_OK) 
    {
         WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Remove garbage file uiMsgId: %s!\n", 
                __FILE__, __LINE__, stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].acFileName));
           
         
         if (deleteMessage(
                stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].uiMsgId,FALSE)
                != MMS_RESULT_OK)
         {
              WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Failed to remove file uiMsgId: %s!\n", 
                __FILE__, __LINE__, stFsmFldrData.pstFilesToRestore[stFsmFldrData.iIdxToRestore].acFileName));
         }        
    }  

     
    stFsmFldrData.iIdxToRestore++; 
    Mms_RestoreFiles();   
} 






void createFolder( void)
{
    int iResult;
    
    if ( WE_FILE_GETSIZE_DIR( MMS_FOLDER) == WE_FILE_ERROR_PATH)
    {
        
        iResult = WE_FILE_MKDIR( MMS_FOLDER);
        switch (iResult)
        {
        case WE_FILE_ERROR_FULL:
        
        case WE_FILE_ERROR_PATH:
            
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
                "ERROR: Unable to create folder."));
            break;
        default:
            
            break;
        } 
    } 
} 

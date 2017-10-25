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
#include "We_File.h"   

#include "We_Core.h"   
#include "We_Dcvt.h"    
#include "We_Cmmn.h"    

#include "Mms_Cfg.h"    
#include "Mms_Def.h"    
#include "Mms_If.h"     
#include "Mmem.h"           
#include "Mmsrpl.h"     
#include "Msig.h"       
#include "Mhandler.h"   
#include "Mcpdu.h"      
#include "Mutils.h"     
#include "Fldmgr.h"     
#include "Masync.h"     

#include "Mltypes.h"    
#include "Mlfetch.h"    
#include "Mlpdup.h"     
#include "Mlfieldp.h"   
#include "Mreport.h"     
#include "Mms_Int.h"    
#include "Mmsui.h"      

#ifdef WE_MODID_DRS
#include "Drs_If.h"     
#endif


#define MMS_MAX_FSM_MMH_INSTANCES     2   
#define MMS_MAX_GET_HEADER_INSTANCE   2  
#define MMS_MAX_BYTES_FROM_BP_TO_SUBJECT (2*MAX_MMT_SUBJECT_LEN)


typedef struct
{
    MmsMsgId msgId;                 
    unsigned char *buffer;          
    unsigned char *bufferPos;       
    WE_UINT32 bufferSize;          
    unsigned char *infoBuffer;      
    WE_UINT32 counter;             
    WE_UINT32 infoBufferSize;      
    WE_UINT32 nrOfBytes;           
    WE_UINT32 mFilePos;            
    WE_INT32 mFileSize;            
    WE_BOOL lastIsMultipart;       
    AsyncOperationHandle asyncOper; 
    MmsGetHeader mHeader;
    MmsHeaderValue mServerID;       
    MmsFolderType mFolder;          
} CreateInfoList;

typedef struct
{
    
    WE_INT16  instance;            
    WE_UINT32 state;               
    WE_UINT32 callingFsm;          
    WE_UINT32 replySig;            
    WE_UINT8  orderer;             
    
    CreateInfoList createInfoList;
} MmhInstanceData; 



typedef enum
{
    STATE_HRD_STOPPED,   
    STATE_HRD_IDLE,      
    STATE_HRD_READING    
} FmHeaderState;



 
typedef struct 
{
    AsyncOperationHandle asyncOper; 
    FmHeaderState state;            
    unsigned char *buf;              
    WE_UINT32 callingFsm;          
    WE_UINT32 replySig;            
    WE_UINT32 size;                 
    WE_UINT32 msgId;               
} MmhHeaderInstance; 













static MmhInstanceData *fsmMmhInstance[MMS_MAX_FSM_MMH_INSTANCES];

      
static MmhHeaderInstance headerInstance[MMS_MAX_GET_HEADER_INSTANCE]; 


static void mmhMain(MmsSignal *sig);
static WE_INT16 selectInstance(void);
static void deleteInstance(WE_INT16 instance);
static MmsResult prepareForInfoList( MmsMsgId msgId, MmhInstanceData *fsm);
static WE_UINT8 isContentTypeDRM(const MmsContentType *contentType);
static MmsResult handleEntries( MmhInstanceData *fsm);
static MmsResult handleHeader ( MmhInstanceData *fsm);
static MmsResult writeInfoFile( MmhInstanceData *fsm);
static int handleReadData(WE_INT16 instance, WE_UINT32 bytes, MmsResult result);
static WE_INT16 getHeaderInstance(void);
static void reportHandler(const MmsSignal *sig);
static MmsMessageType MmsFileType2MmsMessageType(MmsFileType fileType); 
#ifdef WE_MODID_DRS
static void writingDCFdone(MmhInstanceData *fsm, WE_UINT16 result);
#endif

#if MMS_CREATE_ALT_SUBJECT_TEXT == 1
static MmsCharset getMmsCharset(MmsAllParams *params);
static void storeSubjectFromTextBodyPart(MmsGetHeader *header, 
    const MmsContentType *contentType, WE_UINT32 bytesToRead, 
    const unsigned char *textStart);
#endif


#ifdef WE_LOG_MODULE 



const char *fsmMMHSigName(WE_UINT32 sigType)
{
    switch (sigType)
    {
    case M_FSM_MMH_GET_HEADER:
        return "M_FSM_MMH_GET_HEADER";
    case M_FSM_MMH_INT_CREATE_INFO_LIST:
        return "M_FSM_MMH_INT_CREATE_INFO_LIST";
    case M_FSM_MMH_CREATE_INFO_LOOP:
        return "M_FSM_MMH_CREATE_INFO_LOOP";
    case M_FSM_MMH_CREATE_INFO_LIST:
        return "M_FSM_MMH_CREATE_INFO_LIST";
    case M_FSM_MMH_READ_HDR_FINISHED:
        return "M_FSM_MMH_READ_HDR_FINISHED";
    case M_FSM_MMH_CREATE_RR:
        return "M_FSM_MMH_CREATE_RR";
    case M_FSM_MMH_MSG_RR_CREATED:
        return "M_FSM_MMH_MSG_RR_CREATED";
    case M_FSM_MMH_RR_WRITE:
        return "M_FSM_MMH_RR_WRITE";
    case M_FSM_MMH_RR_INFO_FILE_CREATED:
        return "M_FSM_MMH_RR_INFO_FILE_CREATED";
    case M_FSM_MMH_CREATE_RR_GOT_HEADER:
        return "M_FSM_MMH_CREATE_RR_GOT_HEADER";
    case M_FSM_MMH_GOT_REPORT:
        return "M_FSM_MMH_GOT_REPORT";
    default:
        return 0;
    }
} 
#endif









void mmhEmergencyAbort(void)
{
    mSignalDeregister(M_FSM_MMH_HANDLER);
} 




void mmhInit(void)
{
    int i;
    
    for (i = 0; i < MMS_MAX_FSM_MMH_INSTANCES; ++i)
    {
        fsmMmhInstance[i] = NULL;
    } 

    readReportInitData(); 
    mSignalRegisterDst(M_FSM_MMH_HANDLER, mmhMain);
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM MMH UTILS: initialized\n"));
} 









static int startReadHeader(int instance, WE_UINT32 msgId)
{
    MmsFileType suffix = fldrGetFileSuffix(msgId);  
   
    headerInstance[instance].size = fldrGetMsgSize(msgId); 

    if (suffix == MMS_SUFFIX_ERROR) 
    {       
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): File not found !\n", __FILE__, __LINE__));
        return -1;
    }  
    
    if (headerInstance[instance].size == 0) 
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "The file to read header info from is empty!\n"));
        return -1;
    } 
     
    headerInstance[instance].asyncOper = asyncOperationStart(
        Mms_FldrMsgIdToName( msgId, suffix), M_FSM_MMH_HANDLER, instance, 
        M_FSM_MMH_READ_HDR_FINISHED, ASYNC_MODE_READ);

    if (headerInstance[instance].asyncOper == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Insufficient memory during reading message. "
            "Increase available memory.!\n", __FILE__, __LINE__));
        return -1;
    }
    else
    {   
        headerInstance[instance].state = STATE_HRD_READING;
        
        if (headerInstance[instance].size > MMS_MAX_CHUNK_SIZE) 
        {
             
           headerInstance[instance].size = MMS_MAX_CHUNK_SIZE;
        }
       
        headerInstance[instance].buf = M_CALLOC(headerInstance[instance].size);

        asyncRead( headerInstance[instance].asyncOper, 0, headerInstance[instance].buf,
            headerInstance[instance].size);
    } 

    return 0; 
} 




void mmhTerminate(void)
{
    WE_INT16 i;

    
    for (i = 0L; i < MMS_MAX_FSM_MMH_INSTANCES; ++i)
    {
        if (fsmMmhInstance[i] != NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "******* MMH FSM active at terminate ******* \n"));
            deleteInstance(i);
        } 
    } 
    
    deleteNewReadReport();  
    freeReportGlobalData();  
   
    mSignalDeregister(M_FSM_MMH_HANDLER);
    
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM MMH UTILS: terminated\n"));
} 






static void mmhMain(MmsSignal *sig)
{
    MmsResult result = MMS_RESULT_OK;
    MmsMsgId msgId = 0;
    MmsFolderType folder;
    WE_UINT8 orderer;
    WE_INT16 instance = -1;
    MmhInstanceData *fsm;
    
    switch (sig->type)
    {
 #ifdef WE_MODID_DRS
    case M_FSM_MMH_DCF_WRITE_COMPLETE:
        
        if (sig->i_param < 0 || sig->i_param >= MMS_MAX_FSM_MMH_INSTANCES ||
            fsmMmhInstance[sig->i_param] == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Illegal FSM, instance %d!\n", __FILE__, __LINE__,
                sig->i_param));
            mSignalDelete(sig);
            return;
        } 
        
        instance = (WE_INT16) sig->i_param;
        fsm = fsmMmhInstance[instance];
        writingDCFdone(fsm, (WE_UINT16) sig->u_param1);
        break;
#endif           
    case M_FSM_MMH_GET_HEADER:  
        instance = getHeaderInstance();
        if (instance == -1L)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "We're BUSY! Return!\n"));
            M_SIGNAL_SENDTO_IU( (MmsStateMachine)sig->i_param, sig->u_param2, 
                MMS_RESULT_BUSY, 0);
        }
        else 
        {   
             
            headerInstance[instance].callingFsm = (WE_UINT32)sig->i_param;
            headerInstance[instance].replySig   = (WE_UINT32)sig->u_param2;
            headerInstance[instance].msgId      = (WE_UINT32)sig->u_param1; 
            
            if (startReadHeader(instance, sig->u_param1) == -1)
            {
                MmsMessageType msgType = MmsFileType2MmsMessageType(getMsgType(sig->u_param1));    
                
                M_SIGNAL_SENDTO_IU( (MmsStateMachine)sig->i_param, 
                    sig->u_param2, MMS_RESULT_FILE_NOT_FOUND, msgType);
                headerInstance[instance].state = STATE_HRD_STOPPED;
            } 
        }  
        break;

    case M_FSM_MMH_READ_HDR_FINISHED:  
        (void)handleReadData((WE_INT16) sig->i_param, (WE_UINT32)sig->u_param2,(MmsResult) sig->u_param1); 
        
          
        headerInstance[sig->i_param].state = STATE_HRD_STOPPED;
        if (headerInstance[sig->i_param].buf != 0)
        {
          M_FREE(headerInstance[sig->i_param].buf); 
        } 

        asyncOperationStop(&headerInstance[sig->i_param].asyncOper);
        break; 

        



    case M_FSM_MMH_INT_CREATE_INFO_LIST: 
        instance = selectInstance();
       
        if (instance == -1L) 
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "No free instances to create info list! \n"));
           
            M_SIGNAL_SENDTO_IU( (MmsStateMachine)sig->i_param, sig->u_param2, 
                (WE_INT32)sig->u_param1, MMS_RESULT_BUSY);                   
        }
        else 
        {
            fsm = fsmMmhInstance[instance];
            fsm->callingFsm = (WE_UINT32)sig->i_param;
            fsm->replySig = sig->u_param2;
            fsm->createInfoList.msgId = sig->u_param1;
            
            result = mmhCreateInfoList(sig->u_param1, instance);
        
            if ( result == MMS_RESULT_DELAYED || result == MMS_RESULT_OK)
            {
                
            } 
            else 
            { 
                
               if ( fsm->createInfoList.asyncOper != NULL) 
               {
                    asyncOperationStop( &fsm->createInfoList.asyncOper);
               }  
                
                 
               M_SIGNAL_SENDTO_IU( (MmsStateMachine)fsm->callingFsm,
                   fsm->replySig, (WE_INT32) fsm->createInfoList.msgId, result);
              
               fsm->callingFsm = 0;
               fsm->replySig = 0;
               deleteInstance(instance);
            }  
        }  
        break; 

        


    case M_FSM_MMH_CREATE_INFO_LIST:
        msgId = sig->u_param1;
        folder = (MmsFolderType) sig->u_param2;
        instance = selectInstance();
        orderer = (WE_UINT8) sig->i_param;
        
        if (instance == -1L)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "We're BUSY! Return!\n"));
            replyCreateMessageDoneResponse(orderer, MMS_RESULT_ERROR, msgId);
        }
        else
        {   
            fsm = fsmMmhInstance[instance];
            fsm->callingFsm = 0;
            fsm->replySig = 0;
            fsm->createInfoList.mFolder = folder;
            fsm->orderer = orderer;

            result = mmhCreateInfoList( msgId, instance);

            if (result == MMS_RESULT_DELAYED || result == MMS_RESULT_OK)
            {
                
            } 
            else 
            {
                
                
               if (fsm->createInfoList.asyncOper != NULL) 
               {
                    asyncOperationStop( &fsm->createInfoList.asyncOper);
               } 

               deleteInstance(instance);   
               replyCreateMessageDoneResponse(orderer, result, msgId);
            }
        } 
        break;
    


    case M_FSM_MMH_CREATE_INFO_LOOP :
        {
            if (sig->i_param < 0 || sig->i_param >= MMS_MAX_FSM_MMH_INSTANCES ||
                fsmMmhInstance[sig->i_param] == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Illegal FSM, instance %d!\n", __FILE__, __LINE__,
                    sig->i_param));
                mSignalDelete(sig);
                return;
            } 

            instance = (WE_INT16) sig->i_param;
            result = (MmsResult)sig->u_param1;
            fsm = fsmMmhInstance[instance];

            if (result == MMS_RESULT_OK)
            {
                
                result = mmhCreateInfoList( 0, instance); 
                
                if ( result == MMS_RESULT_DELAYED)
                {
                   
                   break;
                } 
            }
            else
            {
                if (WE_FILE_REMOVE(Mms_FldrMsgIdToName( fsm->createInfoList.msgId, MMS_SUFFIX_INFO)) 
                    != WE_FILE_OK) 
                {
                    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                        "%s(%d): Failed to remove file (%s).\n", __FILE__, __LINE__,
                        Mms_FldrMsgIdToName( fsm->createInfoList.msgId, MMS_SUFFIX_INFO)));
                }
            } 

            


            if (result == MMS_RESULT_OK)
            {
                if (fsm->createInfoList.mFolder != MMS_NO_FOLDER)
                {
                    result = mmsFldrMgrSetFolderQuiet(fsm->createInfoList.msgId,
                        fsm->createInfoList.mFolder);
                    if (MMS_RESULT_OK != result)
                    {
                        (void) fldrMgrDeleteMessage(fsm->createInfoList.msgId,FALSE);
                    }
                }
                fsm->createInfoList.mFolder = MMS_NO_FOLDER;
            }

            if (fsm->callingFsm == 0) 
            {
                if (result == MMS_RESULT_OK)
                {
#ifdef MMS_UBS_IN_USE
                    mmsNotifyUBSMsgNew(fsm->createInfoList.msgId);
#endif
                }
                
                replyCreateMessageDoneResponse(fsm->orderer, result, fsm->createInfoList.msgId);
            }
            else   
            {              
                M_SIGNAL_SENDTO_IU( (MmsStateMachine)fsm->callingFsm, 
                    fsm->replySig, (WE_INT32) fsm->createInfoList.msgId, result);
            }  
 
            
            if (fsm->createInfoList.asyncOper != NULL)  
            {
                asyncOperationStop( &fsm->createInfoList.asyncOper);
            } 
            deleteInstance( instance);
        }
        break;

    case M_FSM_MMH_CREATE_RR:
        startReadReportCreation(sig);
        break; 

    case M_FSM_MMH_CREATE_RR_GOT_HEADER: 
        createReadReport(sig); 
        break;

    case M_FSM_MMH_MSG_RR_CREATED:
         
        create10report(sig);
        break; 

    case  M_FSM_MMH_RR_WRITE:
        createInfoFileForRR (sig);
         
        break; 

    case M_FSM_MMH_RR_INFO_FILE_CREATED: 
        
        readReportDone(sig);
        break;
    
    
    case M_FSM_MMH_GOT_REPORT: 
        

        reportHandler(sig);
        break; 

    default:
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM MMH: received unknown signal\n"));
        break;
    } 

    mSignalDelete(sig);
} 








 void sendReportWithError(MmsFileType fileType, MmsResult res) 
 {
    if (fileType ==  MMS_SUFFIX_READREPORT)
    {
        MmsReadOrigInd *mmsReadInd = (MmsReadOrigInd*) M_CALLOC(sizeof(MmsReadOrigInd)); 
        mmsReadInd->readStatus = MMS_READ_STATUS_DELETED_WITHOUT_BEING_READ;
        replyReadReportIndication(mmsReadInd, 0, 0);
        M_FREE(mmsReadInd);
    }
    else if (fileType ==  MMS_SUFFIX_DELIVERYREPORT)
    {
        MmsDeliveryInd *mmsDelivery = M_CALLOC(sizeof(MmsDeliveryInd));
        if (mmsDelivery == NULL)
        {
            return;
        }
        mmsDelivery->messageId = NULL;
        mmsDelivery->to = (MmsAddress*) M_CALLOC (sizeof(MmsAddress));
        mmsDelivery->date = 0;
        mmsDelivery->status = MMS_STATUS_UNRECOGNIZED;
        replyDeliveryReportReceived(mmsDelivery->messageId, mmsDelivery->to, 
                          mmsDelivery->date, mmsDelivery->status, 0, 0);
        M_FREE(mmsDelivery->to);
        M_FREE(mmsDelivery);
    } 
    mmsNotifyError(res);
 }  






 
static MmsMessageType MmsFileType2MmsMessageType(MmsFileType fileType) 
{
     switch (fileType)
     {
        case MMS_SUFFIX_TEMPLATE:
            return MMS_M_SEND_REQ; 
        case MMS_SUFFIX_SEND_REQ:
            return MMS_M_SEND_REQ; 
        case MMS_SUFFIX_IMMEDIATE:
            return MMS_M_NOTIFICATION_IND;
        case MMS_SUFFIX_NOTIFICATION:
            return MMS_M_NOTIFICATION_IND;
        case MMS_SUFFIX_MSG:
            return MMS_M_RETRIEVE_CONF;
        case MMS_SUFFIX_READREPORT:
            return MMS_M_READ_ORIG_IND; 
        case MMS_SUFFIX_DELIVERYREPORT:
            return MMS_M_DELIVERY_IND;

        case MMS_SUFFIX_INFO:
        case MMS_SUFFIX_ERROR:
        default: 
            return (MmsMessageType)0; 
     } 
} 






static void reportHandler(const MmsSignal *sig)
{
     
    WE_UINT32      origMsgId = 0;
    MmsReadOrigInd *mmsOrigInd = NULL;
    MmsDeliveryInd *mmsDelivery= NULL;
   
   
    if ((MmsResult)sig->i_param != MMS_RESULT_OK || 
        sig->p_param == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "MMS FSM MMH: Failed to parse requested report (PDU)\n")); 
       
        if ((MmsMessageType)sig->u_param1 == MMS_M_DELIVERY_IND)
        {
          sendReportWithError(MMS_SUFFIX_DELIVERYREPORT,(MmsResult)sig->i_param);
        }
        else 
        {
          sendReportWithError(MMS_SUFFIX_READREPORT,(MmsResult)sig->i_param);
        }
        return;
    } 
    else if ((MmsMessageType)sig->u_param1 == MMS_M_DELIVERY_IND)
    {
        mmsDelivery = (MmsDeliveryInd*) sig->p_param; 
        
        if ( !mmsDelivery)
        {
            return;
        } 
        
        origMsgId = fldrMgrSearchMsgId(0, mmsDelivery->messageId, 
            MMS_SUFFIX_SEND_REQ); 
        
        if ((mmsDelivery->messageId != NULL) && (mmsDelivery->to != NULL))
        {
              replyDeliveryReportReceived(mmsDelivery->messageId, mmsDelivery->to, 
                          mmsDelivery->date, mmsDelivery->status, origMsgId, (WE_UINT32)sig->u_param2);
        }
        else 
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
              "MMS FSM MMH: The report has no to or serverMsgId (PDU)\n"));
            sendReportWithError(MMS_SUFFIX_DELIVERYREPORT,MMS_RESULT_MESSAGE_CORRUPT);
            
        } 

        freeMmsDeliveryInd(WE_MODID_MMS, mmsDelivery);
        M_FREE(mmsDelivery);
    }
    else if  ((MmsMessageType)sig->u_param1 == MMS_M_READ_ORIG_IND)
    {
        mmsOrigInd = (MmsReadOrigInd*) sig->p_param;  
           
        origMsgId = fldrMgrSearchMsgId(0, mmsOrigInd->serverMessageId, 
            MMS_SUFFIX_SEND_REQ); 

        if (  (mmsOrigInd->from.address != NULL) && 
            (mmsOrigInd->serverMessageId != NULL))
        {
           replyReadReportIndication(mmsOrigInd, origMsgId, (WE_UINT32)sig->u_param2);
        }
        else  
        {
           WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
              "MMS FSM MMH: The report has no address or serverMsgId (PDU)\n"));
           sendReportWithError(MMS_SUFFIX_READREPORT, MMS_RESULT_MESSAGE_CORRUPT);
        }  

        if ( mmsOrigInd)
        {
            freeMmsReadOrigInd( WE_MODID_MMS, mmsOrigInd);             
            M_FREE( mmsOrigInd);
            mmsOrigInd = NULL;
        } 
        
    }   
    else    
    { 
         WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
          "MMS FSM MMH: Failed to parse unknown message type Internal error (PDU)\n"));   
          mmsNotifyError(MMS_RESULT_WRONG_FILE_TYPE);    
    }  
} 










static int handleReadData(WE_INT16 instance, WE_UINT32 bytes, MmsResult result) 
{
    unsigned char *headerData = NULL;
    MmsNotification *notifyPart = NULL;
    MmsGetHeader *msgHeader = NULL; 
    MmsHeaderValue headerValue;
    MmsResult mmsResult = MMS_RESULT_OK;
    MmsVersion version = MMS_VERSION_10;
    unsigned char *ptr; 
    WE_UINT32 headerSize; 

    if (headerInstance[instance].state == STATE_HRD_READING)
    {
        if (result != MMS_RESULT_OK || 
             bytes != headerInstance[instance].size)
        {
          WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                 "%s(%d): Failed to read the file for header info, result=%d, bytes=%d!\n",
                    __FILE__, __LINE__, result, bytes));  
          M_SIGNAL_SENDTO_IUUP( 
              (MmsStateMachine)headerInstance[instance].callingFsm, 
              headerInstance[instance].replySig, 
              MMS_RESULT_FILE_READ_ERROR, MMS_M_RETRIEVE_CONF,
              version, NULL);
          return -1; 
        } 
        else  
        { 
            headerData = headerInstance[instance].buf;
        } 

       

 
       
         
       if (mmsPduGet( headerData, bytes, X_MMS_MESSAGE_TYPE, &headerValue) == 0) 
       {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                 "%s(%d): Failed to read the file for header info, Message type missing!\n",
                    __FILE__, __LINE__));  
            M_SIGNAL_SENDTO_IUUP( 
                (MmsStateMachine)headerInstance[instance].callingFsm, 
                headerInstance[instance].replySig, 
                MMS_RESULT_MESSAGE_CORRUPT, MMS_M_RETRIEVE_CONF,
                version, NULL);
            return -1;
       }  
       
       switch (headerValue.messageType) 
       {
       case  MMS_M_NOTIFICATION_IND:  
          
           notifyPart = M_CALLOC(sizeof(MmsNotification));
           mmsResult = parseMmsNotification(WE_MODID_MMS, headerData, bytes, 
                    notifyPart, &version);

           if (mmsResult != MMS_RESULT_OK)
           {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                 "%s(%d): Failed to parse the file for header info for notification msg!\n",
                    __FILE__, __LINE__));  
                if ( notifyPart)
                {
                    freeMmsNotification(WE_MODID_MMS, notifyPart);
                    M_FREE(notifyPart);
                    notifyPart = NULL;
                } 
                M_SIGNAL_SENDTO_IUUP( 
                    (MmsStateMachine)headerInstance[instance].callingFsm, 
                    headerInstance[instance].replySig, 
                    MMS_RESULT_INVALID_APPLICATION_HEADER, 
                    MMS_M_NOTIFICATION_IND, version, NULL);
                return -1;  
           }  

           M_SIGNAL_SENDTO_IUUP( 
               (MmsStateMachine)headerInstance[instance].callingFsm, 
               headerInstance[instance].replySig, MMS_RESULT_OK, 
               MMS_M_NOTIFICATION_IND, version, notifyPart);
           break; 

       case  MMS_M_RETRIEVE_CONF: 
            
            
           ptr = (unsigned char *)mmsGetBody( headerData, bytes); 
           headerSize = (WE_UINT32)ptr - (WE_UINT32)headerData;   

           if (headerSize >= bytes)
           {
               headerSize = bytes;  
           } 
           
           msgHeader = M_CALLOC(sizeof(MmsGetHeader)); 


           msgHeader->version = version;
           mmsResult = parseMmsHeader(WE_MODID_MMS, headerData, headerSize, 
                   msgHeader); 

           if (mmsResult != MMS_RESULT_OK)
           {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                     "%s(%d): Failed to parse the file for header info for msg!\n",
                        __FILE__, __LINE__));  
                if ( msgHeader)
                {
                    freeMmsGetHeader(WE_MODID_MMS, msgHeader);
                    M_FREE(msgHeader);
                    msgHeader = NULL;
                } 
                M_SIGNAL_SENDTO_IUUP( 
                    (MmsStateMachine)headerInstance[instance].callingFsm, 
                    headerInstance[instance].replySig, 
                    MMS_RESULT_INVALID_APPLICATION_HEADER, 
                    MMS_M_RETRIEVE_CONF, version, NULL);
                 return -1;
           }   

           version = msgHeader->version;

           M_SIGNAL_SENDTO_IUUP( 
               (MmsStateMachine)headerInstance[instance].callingFsm, 
               headerInstance[instance].replySig, MMS_RESULT_OK, 
               MMS_M_RETRIEVE_CONF, version, msgHeader);
           break; 

       case MMS_M_DELIVERY_IND:  
           {
               MmsDeliveryInd *mmsDelivery;
                

 
               mmsDelivery = (MmsDeliveryInd *)M_CALLOC(sizeof(MmsDeliveryInd));
               if ( parseDeliveryReport( WE_MODID_MMS, 
                   headerData, bytes, mmsDelivery) == FALSE)
               {
                   result = MMS_RESULT_ERROR;
                   freeMmsDeliveryInd(WE_MODID_MMS, mmsDelivery);
                   M_FREE( mmsDelivery);
                   mmsDelivery = NULL;
               }
               else
               {
                   result = MMS_RESULT_OK;
               } 
                
               M_SIGNAL_SENDTO_IUUP(
                   (MmsStateMachine)headerInstance[instance].callingFsm, 
                   headerInstance[instance].replySig, result, 
                   MMS_M_DELIVERY_IND, headerInstance[instance].msgId, mmsDelivery);
           }
           break; 

       case MMS_M_READ_ORIG_IND: 
           {
                MmsReadOrigInd *mmsOrigInd = NULL;  
                MmsResult parsResult; 
                
                
                mmsOrigInd = parseReadReport( WE_MODID_MMS, headerData, bytes); 
                
                
                if (mmsOrigInd == NULL) 
                {
                            
                     WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                     "%s(%d): Failed to parse the read report 1.1 pdu, corrupt data!\n",
                        __FILE__, __LINE__));  
                    parsResult = MMS_RESULT_MESSAGE_CORRUPT;
                }     
                else 
                {
                    parsResult = MMS_RESULT_OK;    
                } 

                M_SIGNAL_SENDTO_IUUP(
                    (MmsStateMachine)headerInstance[instance].callingFsm, 
                    headerInstance[instance].replySig, parsResult, 
                    MMS_M_READ_ORIG_IND, headerInstance[instance].msgId, mmsOrigInd); 
           }
           break; 

       default: 
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                 "%s(%d): Can't read the header from this type of message type! message_type = %x",
                    __FILE__, __LINE__, headerValue.messageType)); 

            M_SIGNAL_SENDTO_IUUP(
                (MmsStateMachine)headerInstance[instance].callingFsm, 
                headerInstance[instance].replySig, MMS_RESULT_OK, 
                MMS_M_RETRIEVE_CONF, version, msgHeader);

            return -1;
       } 

    }
    else
    {
       WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Received M_FSM_MMH_READ_HDR_FINISHED in wrong state, internal error!! %d\n",
            __FILE__, __LINE__, headerInstance[instance].state));
       return -1; 
    }     
    return 0;       
} 







static void deleteInstance(WE_INT16 instance)
{
    MmhInstanceData *fsm;   

    fsm = fsmMmhInstance[instance];

    if (fsm == NULL)
    {
        return;
    } 

    if ( fsm->createInfoList.buffer != NULL)
    {
        M_FREE( fsm->createInfoList.buffer);
        fsm->createInfoList.buffer = NULL;
        fsm->createInfoList.bufferPos = NULL;
    } 
  
    if (fsm->createInfoList.infoBuffer != NULL) 
    {
        M_FREE(fsm->createInfoList.infoBuffer); 
        fsm->createInfoList.infoBuffer = NULL;
    }

    if (fsm->createInfoList.asyncOper != NULL)
    {
        asyncOperationStop(&fsm->createInfoList.asyncOper);
    } 

    M_FREE(fsmMmhInstance[instance]);
    fsmMmhInstance[instance] = NULL;
} 







static WE_INT16 selectInstance(void)
{
    WE_UINT16 i;
    
    for (i = 0; i < MMS_MAX_FSM_MMH_INSTANCES; i++) 
    {
        if ( fsmMmhInstance[i] == NULL)
        {
            
            fsmMmhInstance[i] = M_ALLOC(sizeof( MmhInstanceData));
            memset( fsmMmhInstance[i], 0, sizeof(MmhInstanceData));
            fsmMmhInstance[i]->instance = i; 
            fsmMmhInstance[i]->state = 1; 
            return i;
        } 
    } 

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "%s(%d):MMH FSM instance data could not be allocated\n",
        __FILE__, __LINE__));

    return -1;
} 







static WE_INT16 getHeaderInstance(void)
{
    WE_INT16  i;
    
    for (i = 0; i < MMS_MAX_GET_HEADER_INSTANCE; i++) 
    {
        if ( headerInstance[i].state == STATE_HRD_STOPPED)
        {
            
            memset( &headerInstance[i], 0, sizeof(MmhHeaderInstance));
            headerInstance[i].state = STATE_HRD_IDLE; 
            return i;
        } 
    } 
    return -1;
} 







MmsResult mmhCreateInfoList( WE_UINT32 msgId, long instance)
{   
    MmsResult result = MMS_RESULT_ERROR;
    MmhInstanceData *fsm = fsmMmhInstance[instance];

    if (instance < 0 || instance >= MMS_MAX_FSM_MMH_INSTANCES)
    {
              
        return MMS_RESULT_ERROR;
    } 
    else if (fsmMmhInstance[instance] == NULL)
    {
         
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d):MMH FSM instance data is not set\n",
            __FILE__, __LINE__));
       return MMS_RESULT_ERROR;
    } 
        
    
    switch( fsm->state)
    {
        case 1:
            result = prepareForInfoList( msgId, fsm);
            if ( result == MMS_RESULT_OK)
            {
                
                result = MMS_RESULT_DELAYED;
                fsm->state = 2; 
                M_SIGNAL_SENDTO_IU( M_FSM_MMH_HANDLER, (int)M_FSM_MMH_CREATE_INFO_LOOP, 
                    (long)fsm->instance, MMS_RESULT_OK);
            } 
            break;
        case 2:
            
            asyncRead( fsm->createInfoList.asyncOper, 0, fsm->createInfoList.buffer, fsm->createInfoList.nrOfBytes);
            result = MMS_RESULT_DELAYED;
            fsm->state = 3; 
            
            break;            
        case 3: 
            result = handleHeader ( fsm);
            if ( result == MMS_RESULT_OK)
            {
                
                fsm->state = 5; 
                result = MMS_RESULT_DELAYED;
                
                
                M_SIGNAL_SENDTO_IU( M_FSM_MMH_HANDLER, (int)M_FSM_MMH_CREATE_INFO_LOOP, 
                    (long)fsm->instance, MMS_RESULT_OK);
            }
            else if ( result == MMS_RESULT_DELAYED)
            {
                fsm->state = 4;     
                
            } 
            break;            
        case 4:
            fsm->createInfoList.counter++;     
            result = handleEntries( fsm);
            if ( result == MMS_RESULT_OK)
            {
                
                fsm->state = 5; 
                result = MMS_RESULT_DELAYED;
                
                M_SIGNAL_SENDTO_IU( M_FSM_MMH_HANDLER, (int)M_FSM_MMH_CREATE_INFO_LOOP, 
                    (long)fsm->instance, MMS_RESULT_OK);
            }
            else if ( result == MMS_RESULT_DELAYED)            
            {
                fsm->state = 4; 
                
            } 
            else
            {
                freeMmsGetHeader( WE_MODID_MMS, &fsm->createInfoList.mHeader);                
            }
            break;
        case 5:
            fldrMgrSetHeader(fsm->createInfoList.msgId, &fsm->createInfoList.mHeader,
                (WE_UINT32) fsm->createInfoList.mFileSize, fsm->createInfoList.mServerID.messageId);

            
            freeMmsGetHeader( WE_MODID_MMS, &fsm->createInfoList.mHeader);

            
            asyncOperationStop( &fsm->createInfoList.asyncOper);
            result = writeInfoFile( fsm); 
            if ( result == MMS_RESULT_DELAYED)
            {
                fsm->state = 6;
                
            } 
            break;
        case 6:
            
            asyncOperationStop( &fsm->createInfoList.asyncOper);
            return MMS_RESULT_OK;
    } 
    
    return result;
} 








static MmsResult writeInfoFile( MmhInstanceData *fsm)
{
    MmsResult result = MMS_RESULT_DELAYED;  
    WE_UINT32 infoSize = 0;

    
    fsm->createInfoList.asyncOper = asyncOperationStart( 
        Mms_FldrMsgIdToName( fsm->createInfoList.msgId, MMS_SUFFIX_INFO), 
        M_FSM_MMH_HANDLER, 
        (long)fsm->instance,
        M_FSM_MMH_CREATE_INFO_LOOP,
        ASYNC_MODE_WRITE);
    
    if ( fsm->createInfoList.asyncOper == NULL)
    {
       WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
         "%s(%d):MMH FSM Failed to start asynch file operation, write mode\n",
          __FILE__, __LINE__));
        return MMS_RESULT_ERROR;
    }

    
    setNumberOfParts( fsm->createInfoList.infoBuffer, fsm->createInfoList.counter);

    infoSize = ( ((int)POS_INT_MSG_NUMBER + (int)POS_INT_PART_NUMBER + 
        ((int)POS_INT_PART_NUMBER * fsm->createInfoList.counter)) * sizeof( WE_UINT32) ); 

    if ( infoSize > fsm->createInfoList.infoBufferSize)
    {
        
        result = MMS_RESULT_ERROR;
        
        asyncOperationStop(&fsm->createInfoList.asyncOper);
    } 
    else
    {
        asyncWrite( fsm->createInfoList.asyncOper, 0, fsm->createInfoList.infoBuffer, infoSize);
        result = MMS_RESULT_DELAYED;
    } 
    
    return result;
} 








static MmsResult prepareForInfoList( MmsMsgId msgId, MmhInstanceData *fsm)
{
    MmsResult result = MMS_RESULT_OK;  
    MmsFileType msgType = fldrGetFileSuffix(msgId);

    
    fsm->createInfoList.asyncOper = asyncOperationStart( 
        Mms_FldrMsgIdToName( msgId, msgType), 
        M_FSM_MMH_HANDLER, 
        (WE_INT32)fsm->instance,
        M_FSM_MMH_CREATE_INFO_LOOP,
        ASYNC_MODE_READ);

    if ( fsm->createInfoList.asyncOper == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
         "%s(%d):MMH FSM Failed to start asynch file operation, read mode\n",
          __FILE__, __LINE__));
        return MMS_RESULT_ERROR;
    }

    
    fsm->createInfoList.msgId = msgId;
    
    
    fsm->createInfoList.mFileSize = WE_FILE_GETSIZE ( Mms_FldrMsgIdToName( msgId, msgType));

    if ( fsm->createInfoList.mFileSize < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
         "%s(%d):MMH FSM Failed to get the size of the file: msgId:%d\n",
          __FILE__, __LINE__, msgId));
        return MMS_RESULT_ERROR;
    } 

    fsm->createInfoList.mServerID.messageId = 0;
    
    
    





    fsm->createInfoList.infoBuffer = M_ALLOC( MMS_MAX_CHUNK_SIZE);
    fsm->createInfoList.infoBufferSize = MMS_MAX_CHUNK_SIZE;

    
    if ( (WE_UINT32)fsm->createInfoList.mFileSize > MMS_MAX_CHUNK_SIZE)
    {
        fsm->createInfoList.bufferSize = MMS_MAX_CHUNK_SIZE;
        fsm->createInfoList.nrOfBytes = MMS_MAX_CHUNK_SIZE;
    }
    else
    {
        fsm->createInfoList.bufferSize = (WE_UINT32)fsm->createInfoList.mFileSize;
        fsm->createInfoList.nrOfBytes = (WE_UINT32)fsm->createInfoList.mFileSize;
    } 
    fsm->createInfoList.buffer = M_CALLOC( fsm->createInfoList.bufferSize);

    return result;

} 







#if MMS_CREATE_ALT_SUBJECT_TEXT == 1
static MmsCharset getMmsCharset(MmsAllParams *params)
{
    MmsAllParams *tmpParams = params;

    if (tmpParams == NULL)
    {
        return MMS_UNKNOWN_CHARSET;
    }

    while (tmpParams)
    {
        if (tmpParams->param == MMS_CHARSET)
        {
            if (tmpParams->type == MMS_PARAM_STRING)
            {
                return MMS_UNKNOWN_CHARSET;
            }
            else
            {
                switch(tmpParams->value.integer)
                {
              
                case MMS_US_ASCII:
                case MMS_UTF8:
                case MMS_UCS2:
                case MMS_UTF16BE:
                case MMS_UTF16LE:                    
                case MMS_UTF16:
                case MMS_ISO_8859_1:
                    return (MmsCharset)tmpParams->value.integer;
                case MMS_UNKNOWN_CHARSET:                    
                default:
                    return MMS_UNKNOWN_CHARSET;
                }
            }

        }
        tmpParams = tmpParams->next;
    }
    return MMS_UNKNOWN_CHARSET;
}









static void storeSubjectFromTextBodyPart(MmsGetHeader *header, 
    const MmsContentType *contentType, WE_UINT32 bytesToRead, 
    const unsigned char *textStart)
{
    MmsCharset    mmsCharset;
    char         *utf8String   = NULL; 
    char         *lastUtf8Sign = NULL;
    int           utf8Length;
    int           numberOfUtf8Signs;
    int           bytes;
    int           charset;
     
    if (header == NULL || contentType == NULL  || textStart == NULL || bytesToRead == 0)
    {
        return;
    }

    mmsCharset = getMmsCharset(contentType->params);
    
    bytes = (int)bytesToRead;
    charset = (int)mmsCharset;
    
    utf8String = 
        (char *)mmsCvtBuf(WE_MODID_MMS, &charset, textStart, &bytes, 
        &utf8Length);


    if (utf8String == NULL)
    {
        return;
    }

    header->subject.charset = MMS_UTF8;

    if (utf8Length < (MAX_MMT_SUBJECT_LEN - 1))
    {
        header->subject.text = utf8String;
    }
    else
    {
        
        numberOfUtf8Signs = 
            we_cmmn_utf8_strnlen (utf8String, (MAX_MMT_SUBJECT_LEN - 1));

        
        lastUtf8Sign = we_cmmn_utf8_str_incr (utf8String, numberOfUtf8Signs);

        header->subject.text = M_CALLOC(MAX_MMT_SUBJECT_LEN);

        memcpy(header->subject.text, utf8String, lastUtf8Sign - utf8String);
        M_FREE(utf8String);
    }
}
#endif 

#ifdef WE_MODID_DRS
static void startWritingDCF(MmhInstanceData *fsm, WE_UINT32 dataStart, WE_UINT32 dataSize, MmsContentType *ct, WE_INT32 rightsExpected)
{
    char tmpStr[32];
    drs_content_data_t object;
    WE_INT16 opId = fsm->instance;

    object.boundary = NULL;
    object.contentDataLength = dataSize;
    object.mimeType = (char*)ct->strValue;
    object.rightsExpected = rightsExpected;
    object.contentDataType = WE_CONTENT_DATA_FILE;
    object.contentSource = Mms_FldrMsgIdToName(fsm->createInfoList.msgId, fldrGetFileSuffix(fsm->createInfoList.msgId));
    object.contentData = NULL;
    object.contentOffset = dataStart;

    


    fldrMgrSetHasDcf(fsm->createInfoList.msgId);
        
    


    DRSif_storeObject (WE_MODID_MMS,
                   opId,
                   &object,
                   fldrDcf2Name(fsm->createInfoList.msgId, fsm->createInfoList.counter, (char *)tmpStr, 32),
                   DRS_STOREINSTR_BIT_RIGHTS_NOTIFY |
                   DRS_STOREINSTR_BIT_SILENT_RO);
}

static void writingDCFdone(MmhInstanceData *fsm, WE_UINT16 result)
{
    if (result != DRS_DCF_OK)
    {
        



        
    }

    


    asyncRead( fsm->createInfoList.asyncOper, (WE_INT32)fsm->createInfoList.mFilePos, fsm->createInfoList.buffer, fsm->createInfoList.nrOfBytes);
    

} 
#endif






static MmsResult handleHeader( MmhInstanceData *fsm)
{
    MmsResult result = MMS_RESULT_OK;
    unsigned char *dataPtr = NULL;
    WE_UINT32 headerSize = 0;
    WE_BOOL hasDRM = FALSE;
    
    memset( &fsm->createInfoList.mHeader, 0x00, sizeof(MmsGetHeader));

    
    dataPtr = mmsGetBody( fsm->createInfoList.buffer, fsm->createInfoList.nrOfBytes);    
    if ( dataPtr == NULL )
    {
        
        freeMmsGetHeader( WE_MODID_MMS, &fsm->createInfoList.mHeader);
        return MMS_RESULT_MESSAGE_CORRUPT;
    } 
    
    
    headerSize = (WE_UINT32) (dataPtr - fsm->createInfoList.buffer);
    setHeaderSize( fsm->createInfoList.infoBuffer, headerSize);    
    
    result = parseMmsHeader(  WE_MODID_MMS, fsm->createInfoList.buffer, headerSize, 
                                &fsm->createInfoList.mHeader);

    if ( result != MMS_RESULT_OK)
    {
        
        freeMmsGetHeader( WE_MODID_MMS, &fsm->createInfoList.mHeader);
        return MMS_RESULT_MESSAGE_CORRUPT;
    } 
    
    
    setHeaderStart( fsm->createInfoList.infoBuffer);
    setDataSize( fsm->createInfoList.infoBuffer, 0, ((WE_UINT32) fsm->createInfoList.mFileSize - headerSize));
    setDataStart( fsm->createInfoList.infoBuffer, 0, headerSize);
    setEntryStart( fsm->createInfoList.infoBuffer, 0, 0);
    setEntrySize( fsm->createInfoList.infoBuffer, 0, headerSize);
    setEncodingType(fsm->createInfoList.infoBuffer, fsm->createInfoList.counter, ENCODE_VALUE_BINARY);

    
    (void) mmsPduGet( fsm->createInfoList.buffer, headerSize, MMS_MESSAGE_ID, &fsm->createInfoList.mServerID);

    


    if (isContentTypeDRM(fsm->createInfoList.mHeader.contentType) > 0)
    {
        fsm->createInfoList.mHeader.forwardLock = TRUE;
        hasDRM = FALSE;
#ifndef WE_MODID_DRS
        return MMS_RESULT_DRM_NOT_SUPPORTED;
#endif
    }
    setDrmStatus( fsm->createInfoList.infoBuffer, 0, (WE_UINT32) hasDRM);
    
    




#if MMS_CREATE_ALT_SUBJECT_TEXT == 1
    if (fsm->createInfoList.mHeader.messageType == MMS_MSG_TYPE_RETRIEVE_CONF && 
        fsm->createInfoList.mHeader.contentType != NULL && 
        fsm->createInfoList.mHeader.contentType->knownValue == MMS_TYPE_TEXT_PLAIN &&
        fsm->createInfoList.mHeader.subject.text == NULL)
    {
        WE_UINT32 sizeToRead = fsm->createInfoList.nrOfBytes - headerSize;
        if ( sizeToRead > MMS_MAX_BYTES_FROM_BP_TO_SUBJECT)
        {
            sizeToRead = MMS_MAX_BYTES_FROM_BP_TO_SUBJECT;
        }

        storeSubjectFromTextBodyPart(&fsm->createInfoList.mHeader, 
            fsm->createInfoList.mHeader.contentType, sizeToRead, 
            &fsm->createInfoList.buffer[headerSize]);
    }
#endif

    
    if ( mmsIsMultiPart( fsm->createInfoList.mHeader.contentType->knownValue) == FALSE)
    {
        
        
        
        setNumberOfSubParts( fsm->createInfoList.infoBuffer, 0, 0); 
        return MMS_RESULT_OK;
    } 
    
    
    fsm->createInfoList.lastIsMultipart = TRUE;
    
    if ( fsm->createInfoList.mFilePos < (WE_UINT32)fsm->createInfoList.mFileSize)
    {
        
        
        fsm->createInfoList.mFilePos += (WE_UINT32)( headerSize);
        if ( ((WE_UINT32)fsm->createInfoList.mFileSize - fsm->createInfoList.mFilePos) > MMS_MAX_CHUNK_SIZE)
        {
            fsm->createInfoList.nrOfBytes = MMS_MAX_CHUNK_SIZE;
        }
        else
        {
            fsm->createInfoList.nrOfBytes = ((WE_UINT32)fsm->createInfoList.mFileSize - fsm->createInfoList.mFilePos);
        } 
        result = MMS_RESULT_DELAYED;
        asyncRead( fsm->createInfoList.asyncOper, (WE_INT32)fsm->createInfoList.mFilePos, fsm->createInfoList.buffer, fsm->createInfoList.nrOfBytes);
        
    } 
    else
    {
        
        result = MMS_RESULT_DELAYED;
        M_SIGNAL_SENDTO_IU( M_FSM_MMH_HANDLER, (int)M_FSM_MMH_CREATE_INFO_LOOP, 
            (long)fsm->instance, MMS_RESULT_OK);
    }
    return result;
} 







static MmsResult handleEntries( MmhInstanceData *fsm)
{
    MmsResult result;
    unsigned char *dataPtr = NULL;
    WE_UINT32 value = 0;
    WE_UINT32 headersLen = 0;
    WE_UINT32 entrySize = 0;
    WE_UINT32 dataSize = 0;
    WE_UINT32 dataStart = 0;
    WE_UINT32 entryStart = 0;
    MmsContentType *contentType = 0;
    WE_BOOL   hasBody = TRUE;
    WE_BOOL   hasDRM = FALSE;
    MmsBodyInfoList bodyList;
    MmsEntryHeader *entryHeaderPtr = NULL;

    
    if ( fsm->createInfoList.lastIsMultipart == TRUE)
    {
        
        dataPtr = (unsigned char *)cnvUintvarToUint32(
            fsm->createInfoList.buffer, &value, fsm->createInfoList.nrOfBytes);           

        if ( dataPtr == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d):MMH FSM Message corrupt, failed to read entries \n",
                __FILE__, __LINE__));
            return MMS_RESULT_MESSAGE_CORRUPT;
        } 

        setNumberOfSubParts( fsm->createInfoList.infoBuffer, 
            fsm->createInfoList.counter - 1,                
            value);                                         

        if (value == 0)
        {
            
            hasBody = FALSE;
        }
    } 
    else
    {
        dataPtr = fsm->createInfoList.buffer;
    }
    
    if (hasBody == TRUE)
    {
      
      dataPtr = (unsigned char *)cnvUintvarToUint32(
          dataPtr, &headersLen, fsm->createInfoList.nrOfBytes);           

      if ( dataPtr == NULL)
      {
          WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
              "%s(%d):MMH FSM Message corrupt, header length \n",
              __FILE__, __LINE__));
          return MMS_RESULT_MESSAGE_CORRUPT;
      } 
    
    
      
      dataPtr = (unsigned char *)cnvUintvarToUint32(
          dataPtr, &dataSize, fsm->createInfoList.nrOfBytes);           

      if ( dataPtr == NULL)
      {
          WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
              "%s(%d):MMH FSM Message corrupt, data length \n",
              __FILE__, __LINE__));
          return MMS_RESULT_MESSAGE_CORRUPT;
      } 
    
      
      entryStart = fsm->createInfoList.mFilePos;
      entrySize = (WE_UINT32)(dataPtr - fsm->createInfoList.buffer) + headersLen;
      dataStart = fsm->createInfoList.mFilePos + entrySize; 
      setEntryStart( fsm->createInfoList.infoBuffer, fsm->createInfoList.counter, entryStart);
      setEntrySize( fsm->createInfoList.infoBuffer, fsm->createInfoList.counter, entrySize);
      setDataStart( fsm->createInfoList.infoBuffer, fsm->createInfoList.counter, dataStart);
      setDataSize( fsm->createInfoList.infoBuffer, fsm->createInfoList.counter, dataSize);
    
      dataPtr = parseContentType( WE_MODID_MMS, &contentType, dataPtr, 
          (fsm->createInfoList.nrOfBytes - (WE_UINT32) (dataPtr - fsm->createInfoList.buffer)));
      if ( dataPtr == NULL)
      {
          WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
              "%s(%d):MMH FSM Message corrupt, Content Type \n",
              __FILE__, __LINE__));
          return MMS_RESULT_MESSAGE_CORRUPT;
      }  
      if (isContentTypeDRM(contentType) > 0)
      {
          fsm->createInfoList.mHeader.forwardLock = TRUE;
          hasDRM = TRUE;
#ifndef WE_MODID_DRS
        return MMS_RESULT_DRM_NOT_SUPPORTED;
#endif
      }
    
      


      dataPtr = parseEntryHeader( WE_MODID_MMS, dataPtr,
          (entrySize - (WE_UINT32) (dataPtr - fsm->createInfoList.buffer)), 
          &bodyList);
      
      if ( dataPtr == NULL)
      {
          
          result = MMS_RESULT_MESSAGE_CORRUPT;
      }
      else
      {
          setEncodingType(fsm->createInfoList.infoBuffer, fsm->createInfoList.counter, ENCODE_VALUE_BINARY);
          entryHeaderPtr = bodyList.entryHeader;
          while (entryHeaderPtr)
          {
              if (entryHeaderPtr->headerType == MMS_WELL_KNOWN_CONTENT_ENCODING)
              {
                  setEncodingType(fsm->createInfoList.infoBuffer, fsm->createInfoList.counter, 
                      (MmsContentEncodingValue) entryHeaderPtr->value.shortCutShiftDelimiter);
                  break;
              }
              entryHeaderPtr = entryHeaderPtr->next;
          }
          freeMmsEntryHeader(WE_MODID_MMS, bodyList.entryHeader);
          M_FREE(bodyList.entryHeader);
          bodyList.entryHeader=NULL;
      } 
      
      setDrmStatus( fsm->createInfoList.infoBuffer, fsm->createInfoList.counter, (WE_UINT32) hasDRM);

      setEncodingType( fsm->createInfoList.infoBuffer, fsm->createInfoList.counter, (WE_UINT32) 0);
      
      




#if MMS_CREATE_ALT_SUBJECT_TEXT == 1
      if (fsm->createInfoList.mHeader.messageType == MMS_MSG_TYPE_RETRIEVE_CONF && 
          contentType->knownValue == MMS_TYPE_TEXT_PLAIN &&
          fsm->createInfoList.mHeader.subject.text == NULL)
      {
          WE_UINT32 sizeToRead = fsm->createInfoList.nrOfBytes -
              (dataStart - entryStart);
          
          if ( sizeToRead > MMS_MAX_BYTES_FROM_BP_TO_SUBJECT)
          {
              sizeToRead = MMS_MAX_BYTES_FROM_BP_TO_SUBJECT;
          }
          
          storeSubjectFromTextBodyPart(&fsm->createInfoList.mHeader, contentType,
              sizeToRead, &fsm->createInfoList.buffer[dataStart - entryStart]);
      }
#endif

      fsm->createInfoList.lastIsMultipart = FALSE;   
      if ( mmsIsMultiPart( contentType->knownValue))
      {
          fsm->createInfoList.lastIsMultipart = TRUE;
                    
          fsm->createInfoList.mFilePos = dataStart;
      } 
      else
      {
          
          fsm->createInfoList.mFilePos = dataStart + dataSize;
          
          setNumberOfSubParts( fsm->createInfoList.infoBuffer, fsm->createInfoList.counter, 0);
      } 

    }
    else
    {
        
        
        fsm->createInfoList.counter--;
        dataSize = 1;
        fsm->createInfoList.mFilePos += dataSize;
    }
        
    
    if ( fsm->createInfoList.mFilePos > (WE_UINT32)fsm->createInfoList.mFileSize)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d):MMH FSM Message corrupt, Body-part continues beyond message size.\n",
            __FILE__, __LINE__));
        result = MMS_RESULT_MESSAGE_CORRUPT;        
    }
    
    else if ( fsm->createInfoList.mFilePos < (WE_UINT32)fsm->createInfoList.mFileSize)
    {
        
        if ( ((WE_UINT32)fsm->createInfoList.mFileSize - fsm->createInfoList.mFilePos) > MMS_MAX_CHUNK_SIZE)
        {
            fsm->createInfoList.nrOfBytes = MMS_MAX_CHUNK_SIZE;
        }
        else
        {
            fsm->createInfoList.nrOfBytes = ((WE_UINT32)fsm->createInfoList.mFileSize - fsm->createInfoList.mFilePos);
        } 

        result = MMS_RESULT_DELAYED;
        if (hasDRM == TRUE)
        {
#ifdef WE_MODID_DRS
            startWritingDCF(fsm, dataStart, dataSize, contentType, -2);
            
#endif
        }
        else
        {
            asyncRead( fsm->createInfoList.asyncOper, (WE_INT32)fsm->createInfoList.mFilePos, fsm->createInfoList.buffer, fsm->createInfoList.nrOfBytes);
            
        }
    } 
    else
    {
        
        
        
        if ( fsm->createInfoList.nrOfBytes <= ( entrySize + dataSize))
        {
            
            result = MMS_RESULT_OK;
        } 
        else
        {
            fsm->createInfoList.bufferPos = fsm->createInfoList.buffer + dataSize;
            result = MMS_RESULT_DELAYED;
            M_SIGNAL_SENDTO_IU( M_FSM_MMH_HANDLER, (int)M_FSM_MMH_CREATE_INFO_LOOP, 
                (long)fsm->instance, MMS_RESULT_OK);
        } 
    }

    
    if ( contentType)
    {
        freeMmsContentType( WE_MODID_MMS, contentType);
        M_FREE( contentType);
        contentType = NULL;
    } 
    
    return result;
} 







static WE_UINT8 isContentTypeDRM(const MmsContentType *contentType)
{
    if (contentType->knownValue == MMS_TYPE_APP_VND_OMA_DRM_MESSAGE)
    {
        return 1;
    }
    else if (contentType->knownValue == MMS_TYPE_APP_VND_OMA_DRM_CONTENT)
    {
        


        return 0;
    }
    else if (contentType->knownValue == MMS_TYPE_APP_VND_OMA_DRM_R_XML)
    {
        return 2; 
    }
    else if (contentType->knownValue == MMS_TYPE_APP_VND_OMA_DRM_R_WBXML)
    {
        return 2; 
    }
    else if (contentType->knownValue == MMS_VALUE_AS_STRING)
    {
        if (we_cmmn_strcmp_nc((const char *)contentType->strValue, MMS_MEDIA_TYPE_STRING_FORWARD_LOCKED) == 0
            
            || we_cmmn_strcmp_nc((const char *)contentType->strValue, MMS_MEDIA_TYPE_STRING_FORWARD_LOCKED_OMA) == 0
            
            )
        {
            return 1;
        }
    }
    
    return 0;
} 









#ifdef WE_MODID_DRS
void mmsMMHhandleDrmStoreFinished(void *sigData)
{
    drs_store_finished_t *reply = sigData;
    
    M_SIGNAL_SENDTO_IU(M_FSM_MMH_HANDLER, M_FSM_MMH_DCF_WRITE_COMPLETE,
        reply->opId, reply->returnCode);
} 

#endif

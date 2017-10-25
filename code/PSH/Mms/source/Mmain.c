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
#include "We_Log.h"     

#include "We_Def.h"     
#include "We_Core.h"    
#include "We_Dcvt.h"    
#include "We_Mem.h"    
#include "We_Cfg.h"    
#include "We_File.h"   
#include "We_Pipe.h"   
#include "We_Errc.h"   

#include "Mms_Cfg.h"    
#include "Mms_Def.h"    
#include "Mms_If.h"     
#include "Mms_Int.h"    

#include "Msig.h"       
#include "Mtimer.h"     
#include "Mcpdu.h"      

#include "Msend.h"      
#include "Mreceive.h"   
#include "Mnotify.h"    
#include "Mcsend.h"     
#include "Mcget.h"      
#include "Mcnotif.h"    
#include "Mcpost.h"     
#include "Mconfig.h"    
#include "Fldmgr.h"     
#include "Mhandler.h"   
#include "Mms_Cont.h"   

#include "Mcwap.h"      
#include "Mmain.h"      
#include "Masync.h"     
#include "Msend.h"      
#include "Msgread.h"    

#include "Mmsui.h"      

#ifdef WE_MODID_DRS
#include "Drs_If.h"     
#endif


#define MMS_CONTENT_PATH            "/frw/rt_db/MMS"
#define MMS_CONTENT_MIME_TYPE_TAG   "MimeType"
#define MMS_CONTENT_APP_ID_TAG      "PAppId_s"
#define MMS_CONTENT_APP_NO_TAG      "PAppId_n"


typedef enum
{
    STATE_STOPPED = 0,
    STATE_STARTING,
    STATE_STARTED,
    STATE_STOPPING
} ModuleStates;




typedef struct
{
    ModuleStates    state;      

    WE_BOOL        useCallback;

    MmsStateMachine fsm;        
    unsigned int    signal;     
    long            i_param;    
} ModuleInfo;




static void mmsRegisterWithContentRouter(void);













static WE_BOOL executeCommandReceived;
static ModuleStates myState;


static ModuleInfo    drsModInfo; 
static ModuleInfo    stkModInfo; 


static void coreMain(MmsSignal *sig);
static void handleStart(long level);
static void handleTerminate(long level);
static void readyCheck(void);


#ifdef WE_LOG_MODULE 



const char *fsmCoreMainSigName(WE_UINT32 sigType)
{
    switch (sigType)
    {
    case MMS_SIG_MAIN_START:
        return "MMS_SIG_MAIN_START";
    case MMS_SIG_MAIN_TERMINATE:
        return "MMS_SIG_MAIN_TERMINATE";
    case MMS_SIG_MAIN_EXECUTE_COMMAND:
        return "MMS_SIG_MAIN_EXECUTE_COMMAND";
    default:
        return 0;
    }
} 
#endif




static void coreMain(MmsSignal *sig)
{
    WE_UINT32 type;
    int iParam;

    




    type = sig->type;
    iParam = sig->i_param;
    mSignalDelete(sig);

    switch (type)
    {
    case MMS_SIG_MAIN_EXECUTE_COMMAND :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS Main received EXECUTE_COMMAND.\n"));
        executeCommandReceived = TRUE;
        readyCheck();
        break;
    case MMS_SIG_MAIN_START: 
        handleStart(iParam);
        break;
    case MMS_SIG_MAIN_TERMINATE:  
        handleTerminate(iParam);
        break;
    default :
        break;
    } 
} 







void coreMainEmergencyAbort(void)
{
    myState = STATE_STOPPED;

    WE_ERROR( WE_MODID_MMS, WE_ERR_MODULE_OUT_OF_MEMORY);
    WE_SIGNAL_DEREGISTER_QUEUE(WE_MODID_MMS);
    msrSendEmergencyAbort();
    msrReceiveEmergencyAbort();
    msrNotifyEmergencyAbort();
    mmsMREmergencyAbort();
    cohSendEmergencyAbort();
    cohGetEmergencyAbort();
    cohPushReceiveEmergencyAbort();
    cohPostEmergencyAbort();
    cfgEmergencyAbort();
    mmsWapEmergencyAbort();
    mmhEmergencyAbort();
    Mms_FldrEmergencyAbort();
    asyncEmergencyAbort();
    mTimerEmergencyAbort();
    contEmergencyAbort();
    mSignalEmergencyAbort();
    WE_PIPE_CLOSE_ALL(WE_MODID_MMS);
    WE_FILE_CLOSE_ALL(WE_MODID_MMS);
    WE_MEM_FREE_ALL(WE_MODID_MMS);
    WE_MODULE_IS_TERMINATED(WE_MODID_MMS);
    WE_KILL_TASK(WE_MODID_MMS);
} 




void coreMainInit(void)
{
    myState = STATE_STOPPED;
    executeCommandReceived = FALSE;

    mSignalRegisterDst(M_FSM_CORE_MAIN, coreMain);
   
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM CORE MAIN: initialized\n"));
} 




void coreMainTerminate(void)
{
    executeCommandReceived = FALSE;
    mSignalDeregister(M_FSM_CORE_MAIN);
   
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM CORE MAIN: terminated\n"));
} 









void mmsCoreModuleStart(WE_UINT32 modId, WE_BOOL useCallback, 
                    MmsStateMachine fsm, unsigned int signal, WE_UINT32 i_param)
{
    ModuleInfo  *moduleInfo = NULL;
    WE_BOOL    immediateReply = FALSE;

    switch(modId)
    {
#ifdef WE_MODID_DRS
    case WE_MODID_DRS:
        if (STATE_STOPPED == drsModInfo.state)
        {
            WE_MODULE_START(WE_MODID_MMS, WE_MODID_DRS, NULL, NULL, NULL);
            drsModInfo.state = STATE_STARTING;
            moduleInfo = &drsModInfo;
        }
        break;
#endif
    case WE_MODID_STK:
        if (STATE_STOPPED == stkModInfo.state)
        {
            WE_MODULE_START(WE_MODID_MMS, WE_MODID_STK, NULL, NULL, NULL);
            stkModInfo.state = STATE_STARTING;
            moduleInfo = &stkModInfo;
        }
        else if(STATE_STARTED == stkModInfo.state)
        {
            moduleInfo = &stkModInfo;
            immediateReply = TRUE;
        }
        break;
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MMS,
            "(%s) (%d) Unsupported modId %d\n", __FILE__, __LINE__, modId));
        break;
    }

    if (moduleInfo != NULL)
    {
        moduleInfo->fsm = fsm;
        moduleInfo->signal = signal;
        moduleInfo->useCallback = useCallback;
        moduleInfo->i_param = i_param;

        if(immediateReply)
        {
            if (moduleInfo->useCallback)
            {
                M_SIGNAL_SENDTO_I(moduleInfo->fsm, moduleInfo->signal, 
                moduleInfo->i_param);
            }
        }
    }
}






void mmsCoreModuleStarted(WE_UINT32 modId)
{
    ModuleInfo        *moduleInfo = NULL;

    switch (modId)
    {
#ifdef WE_MODID_DRS
    case WE_MODID_DRS:
        if (drsModInfo.state == STATE_STARTING)
        {
            moduleInfo = &drsModInfo;
            DRSif_startInterface();
        }
        break;
#endif
    case WE_MODID_STK:
        if (stkModInfo.state == STATE_STARTING)
        {
            moduleInfo = &stkModInfo;
        }
        break;
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MMS,
            "(%s) (%d) Unsupported modId %d\n", __FILE__, __LINE__, modId));
        break;
    }

    if (NULL != moduleInfo)
    {
        
        moduleInfo->state = STATE_STARTED;
        
        if (moduleInfo->useCallback)
        {
            M_SIGNAL_SENDTO_I(moduleInfo->fsm, moduleInfo->signal, moduleInfo->i_param);
        }
    }
}






void mmsCoreModuleStop(WE_UINT32 modId)
{
    ModuleInfo *modInfo = NULL;

    switch (modId)
    {
#ifdef WE_MODID_DRS
    case WE_MODID_DRS:
        modInfo = &drsModInfo;
        break;
#endif
    case WE_MODID_STK:
        modInfo = &stkModInfo;
        break;
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MMS,
            "(%s) (%d) Unsupported modId %d\n", __FILE__, __LINE__, modId));
        return;
    }

    if ((modInfo->state != STATE_STOPPING) && (modInfo->state != STATE_STOPPED))
    {
        modInfo->state = STATE_STOPPING;
        
        WE_MODULE_TERMINATE(WE_MODID_MMS, (WE_UINT8)modId);
    }
}






void mmsCoreModuleStopped(WE_UINT32 modId)
{
    switch (modId)
    {
#ifdef WE_MODID_DRS
    case WE_MODID_DRS:
        drsModInfo.state = STATE_STOPPED;
        break;
#endif
    case WE_MODID_STK:
        stkModInfo.state = STATE_STOPPED;
        M_SIGNAL_SENDTO_IUU( M_FSM_COH_WAP, (int)MMS_SIG_COH_WAP_STOP_RESPONSE,
            0, 0, MMS_RESULT_OK);
        break;
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MMS,
            "(%s) (%d) Unsupported modId %d\n", __FILE__, __LINE__, modId));
        break;
    }
}




static void handleStart(long level)
{
    switch (level)
    {
    case 1:
        myState = STATE_STARTING;
        memset(&drsModInfo,0,sizeof(ModuleInfo));
        memset(&stkModInfo,0,sizeof(ModuleInfo));
        mTimerInit();
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_START, level + 1);
        break;
    case 2:
        if (asyncInit())
        {
            M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_START, level + 1);
        }
        else
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "MMS FSM CORE MAIN: ASYNC failed. Terminating...\n"));
            WE_ERROR( WE_MODID_MMS, WE_ERR_MODULE_OUT_OF_MEMORY);

            coreMainEmergencyAbort();
        } 
        break;
		

	case 3:
        cfgInit();
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_START, level + 1);
        break;
    case 4:
        


        mmhInit();
#ifdef WE_MODID_DRS      
        mmsCoreModuleStart(WE_MODID_DRS, TRUE, M_FSM_CORE_MAIN,MMS_SIG_MAIN_START, level + 1);
#else
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_START, level + 1);
#endif
            break;
    case 5:
        Mms_FldrInit(); 
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_START, level + 1);
        break;
    case 6:
        mmsWapInit();
        mmsMRInit();

        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_START, level + 1);
        break;
    case 7:
        msrSendInit();
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_START, level + 1);
        break;
    case 8:
        msrReceiveInit();
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_START, level + 1);
        break;
    case 9:
        msrNotifyInit();
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_START, level + 1);
        break;
    case 10:
        cohSendInit();
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_START, level + 1);
        break;
    case 11:
        cohGetInit();
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_START, level + 1);
        break;
    case 12:
        cohPushReceiveInit();
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_START, level + 1);
        break;
    case 13:
        cohPostInit();
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_START, level + 1);
        break;
    case 14: 
        
 
      
        if (Mms_FldrGetWorkingState() == STATE_FOLDER_IDLE)
        {
             
            M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_START, 
                level + 1); 
        } 
        else   
        {
            
            M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_START, 
                level); 
        } 
        break;
    case 15:
        M_SIGNAL_SENDTO_I( M_FSM_CORE_CONFIG, (int)MMS_SIG_CORE_READ_REGISTRY, 
            level + 1);
        break;
        
    case 16:
#ifdef MMS_UBS_IN_USE
        mmsHandleUBSStart();
#endif
        contInit();
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_START, level + 1);
        break;

    case 17:
        mmsRegisterWithContentRouter();
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_START, level + 1);
        break;

    case 18:
        {
            WE_UINT32 numOfMsg = 0;
            WE_UINT32 numOfUnreadMsg = 0;
            WE_UINT32 numOfNotif = 0;
            WE_UINT32 numOfUnreadNotif = 0;
            WE_UINT32 numOfRR = 0;
            WE_UINT32 numOfUnreadRR = 0;
            WE_UINT32 numOfDR = 0;
            WE_UINT32 numOfUnreadDR = 0;              

            myState = STATE_STARTED;
            readyCheck();

            
             
            fldrGetInboxStatus(&numOfMsg, &numOfUnreadMsg,
                &numOfNotif, &numOfUnreadNotif,
                &numOfRR, &numOfUnreadRR,
                &numOfDR, &numOfUnreadDR);
            MMSa_messageStatus(numOfMsg, numOfUnreadMsg,
                numOfNotif, numOfUnreadNotif,
                numOfRR, numOfUnreadRR,
                numOfDR, numOfUnreadDR);
#ifdef MMS_UBS_IN_USE            
            mmsNotifyUBSUnreadMsg(
                (WE_UINT16) (numOfUnreadMsg+numOfUnreadNotif+numOfUnreadRR+numOfUnreadDR));
#endif

            


            mmsImmediateRetrievalGet();
        }
        break;
    default:
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Received unexpected level %d\n",
            __FILE__, __LINE__, level));
        break;
    } 
} 




static void handleTerminate(long level)
{
    switch (level)
    {
    case 1:
        myState = STATE_STOPPING;
        msrSendTerminate();
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_TERMINATE, level + 1);
        break;
    case 2:
        msrReceiveTerminate();
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_TERMINATE, level + 1);
        break;
    case 3:
        msrNotifyTerminate();
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_TERMINATE, level + 1);
        break;
    case 4:
        mmsMRTerminate( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_TERMINATE, level + 1);
        break;
    case 5:
        cohSendTerminate();
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_TERMINATE, level + 1);
        break;
    case 6:
        cohGetTerminate();
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_TERMINATE, level + 1);
        break;
    case 7:
        cohPushReceiveTerminate();
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_TERMINATE, level + 1);
        break;
    case 8:
        cohPostTerminate();
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_TERMINATE, level + 1);
        break;
    case 9:
        cfgTerminate();
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_TERMINATE, level + 1);
        break;
    case 10:
        mmsWapTerminate();      
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_TERMINATE, level + 1);
        break;
    case 11: 
        mmhTerminate();
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_TERMINATE, level + 1);
        break;
    case 12: 
        

        Mms_FldrTerminate(M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_TERMINATE, level + 1);
        break;
    case 13: 
        

        asyncTerminate( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_TERMINATE, level + 1);
        break;
    case 14:
        mTimerTerminate();      
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_TERMINATE, level + 1);
        break;
    case 15:
        contTerminate();
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_TERMINATE, level + 1);
        break;
    case 16:
        
        coreMainTerminate();    
        mSignalTerminate();     

        WE_PIPE_CLOSE_ALL (WE_MODID_MMS);
        WE_FILE_CLOSE_ALL (WE_MODID_MMS);
        
        WE_SIGNAL_DEREGISTER_QUEUE(WE_MODID_MMS);
        WE_MEM_FREE_ALL(WE_MODID_MMS);
        WE_MODULE_IS_TERMINATED(WE_MODID_MMS);
        WE_KILL_TASK(WE_MODID_MMS);
        myState = STATE_STOPPED;
        break;
    } 
} 




static void mmsRegisterWithContentRouter(void)
{
    void* handle;
    
    handle = WE_REGISTRY_SET_INIT(WE_MODID_MMS);
    
    WE_REGISTRY_SET_PATH( handle, (char *)MMS_CONTENT_PATH);
    WE_REGISTRY_SET_ADD_KEY_STR( handle, (char *)MMS_CONTENT_MIME_TYPE_TAG, 
        (unsigned char*)MMS_CONTENT, (WE_UINT16)(strlen(MMS_CONTENT) + 1));
    WE_REGISTRY_SET_ADD_KEY_STR( handle, (char *)MMS_CONTENT_APP_ID_TAG, 
        (unsigned char*)MMS_PUSH_APPLICATION_ID, 
        (WE_UINT16)(strlen(MMS_PUSH_APPLICATION_ID) + 1));
    WE_REGISTRY_SET_ADD_KEY_STR( handle, (char *)MMS_CONTENT_APP_NO_TAG, 
        (unsigned char*)MMS_PUSH_APPLICATION_NO, 
        (WE_UINT16)(strlen(MMS_PUSH_APPLICATION_NO) + 1));
    WE_REGISTRY_SET_COMMIT(handle);
} 




static void readyCheck(void)
{
    if (myState == STATE_STARTED && executeCommandReceived)
    {
         
        WE_MODULE_IS_ACTIVE((WE_UINT8)WE_MODID_MMS);
    } 
} 

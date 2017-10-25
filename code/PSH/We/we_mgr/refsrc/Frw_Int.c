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
 *
 *
 * Frw_Int.c
 *
 * Stub implementation of FRWa_
 *
 * 
 *
 */

#include "Frw_Int.h"

void FRWa_createTask (WE_UINT8 uiModId)
{
}

void FRWa_moduleStatus 
(
    WE_UINT8 uiModId, 
    int iStatus
)
{
}

void FRWa_terminated (void)
{
}
   
void FRWa_setTimer (WE_UINT32 uiTimeInterval)
{

}

void FRWa_resetTimer (void)
{
}


void FRWa_registryResponse (WE_UINT8 wid)
{
}

void FRWa_externalGet 
(
    WE_INT32 wid, 
    const char* pcPath, 
    const char* pcKey
)
{
}

int 
FRWa_setPersistentTimer (WE_UINT8 uiModId, WE_UINT32 uiTime)
{
    return 0;
}

int 
FRWa_resetPersistentTimer (void)
{
    return 0;
}

void
integration_set_registry (void)
{
    
}

WE_BOOL wap_modules_run(void)
{
#if 0
    WE_UINT8 modId;
    WE_UINT8 signalNumBeforeRun, signalNumAfterRun;
    WE_BOOL  ret = WE_FALSE;
    int iMms = 0, iBrs = 0, iStk = 0;
    
    for (modId = WE_MODID_MSM; modId < WE_NUMBER_OF_MODULES; modId++)
    {
        if (get_moduleStatus(modId) > MSM_MODULE_STATUS_STANDBY &&
            get_moduleStatus(modId) < MSM_MODULE_STATUS_TERMINATED)
        {
            if (module_wantstorun_func[modId] != WE_NULL &&
                module_run_func[modId] != WE_NULL)
            {
                /* get current signal num */
                signalNumAfterRun = module_wantstorun_func[modId]();
                if (signalNumAfterRun > 0)
                {
                    iMms = 0;
                    iBrs = 0; 
                    iStk = 0;
                    do
                    {
                        if(WE_MODID_MMS == modId)
                        {
                            iMms ++;
                        }
                        else if(WE_MODID_WBS == modId)
                        {
                            iBrs ++;
                        }
                        else if(WE_MODID_STK == modId)
                        {
                            iStk ++;
                        }
                        signalNumBeforeRun = signalNumAfterRun;
                        module_run_func[modId]();

#if 0
                        /*panym add*/
                        if(WE_MODID_RMA== modId&&RMA_HTTP_START_JAR == rma_http_get_state())
                        {
                            ret = WE_FALSE;   /* so as to react more quickly when rma is downloading jar file*/
                        }
                        else
#endif                        
                        {
                            ret = TRUE;   /* module_run_func has been called */
                        }
                        
                        //ret = WE_TRUE;   /* module_run_func has been called */
                        signalNumAfterRun = module_wantstorun_func[modId]();
                    } while ((signalNumAfterRun > 0) && 
                        (signalNumBeforeRun <= signalNumAfterRun) && 
                        iMms < 6 && iBrs < 6 && iStk < 6);
                     /* 
                       * if (signalNumBeforeRun <= signalNumAfterRun), it means this module send 
                       * a signal to itself, so we process this new sinal immediately to speed up the module running 
                       */
                }
                
            }
        }
        
        if (get_moduleStatus(modId) == MSM_MODULE_STATUS_TERMINATED)
        {
            module_status[modId] = MSM_MODULE_STATUS_NULL;
#ifdef WE_CONFIG_INTERNAL_MALLOC
            int_release_internal_mem( modId);
#endif

#ifdef JAVA_DOWNLOAD_MODULE
            if (modId == WE_MODID_WBA )
#else
            if (modId == WE_MODID_WBA )
#endif
            {
                WE_UINT8 *pParam;
                pParam = op_new(WE_UINT8);
                *pParam = modId;
#if 0				
                EM_Add_Event(OPUS_EVENT_GROUP_INTERNAL, OPUS_INTERNAL_WAP_TERMINATE, (void*)pParam);                
#endif
            }
            if (modId == WE_MODID_PRS)
            {
                WE_UINT8 *pParam;
                pParam = op_new(WE_UINT8);
                *pParam = modId;
#if 0				
                EM_Add_Event(OPUS_EVENT_GROUP_INTERNAL, OPUS_INTERNAL_WAP_TERMINATE_CNF, (void*)pParam);                
#endif
        
            }
        }
    }
    return ret;
#endif 
   return 0;
}


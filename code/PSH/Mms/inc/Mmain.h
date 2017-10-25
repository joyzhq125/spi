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
 




#ifndef MMAIN_H
#define MMAIN_H


#define MMS_MAIN_FIRST_START_ELEMENT        1
#define MMS_MAIN_FIRST_TERMINATE_ELEMENT    1







typedef enum
{
    






    MMS_SIG_MAIN_START,

    






    MMS_SIG_MAIN_TERMINATE,

    





    MMS_SIG_MAIN_EXECUTE_COMMAND
} MmsCoreMainSignalId;










void coreMainEmergencyAbort(void);
void coreMainInit(void);
void coreMainTerminate(void);
void mmsCoreModuleStarted(WE_UINT32 modId);
void mmsCoreModuleStopped(WE_UINT32 modId);
void mmsCoreModuleStop(WE_UINT32 modId);
void mmsCoreModuleStart(WE_UINT32 modId, WE_BOOL useCallback, 
                        MmsStateMachine fsm, unsigned int signal, WE_UINT32 i_param);
                        

#endif 

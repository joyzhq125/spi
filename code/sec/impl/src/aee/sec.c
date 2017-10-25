/*==================================================================================================
    HEADER NAME : sec.c
    MODULE NAME : SEC

    PRE-INCLUDE FILES DESCRIPTION

    GENERAL DESCRIPTION
        In this file,define the interface function prototype of sec module.

    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History

    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-07   Kevin Yang        None         Draft
    
==================================================================================================*/

/*=====================================================================================
*   Include File Section
*=====================================================================================*/
#include "sec_comm.h"
#include "oem_secmgr.h"

#include "isecb.h"
#include "isecw.h"
#include "isignText.h"

#include "secwap.bid"
#include "secbrs.bid"
#include "secsigntext.bid"

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
extern WE_INT32 ISignText_New(WE_UINT16 sSize, IShell *pIShell, IModule* pIModule, ISignText ** ppMod);
extern WE_INT32 ISecB_New(WE_UINT16 sSize, IShell *pIShell, IModule* pIModule, ISecB ** ppMod);
extern WE_INT32 ISecW_New(WE_UINT16 sSize, IShell *pIShell, IModule* pIModule, ISecW ** ppMod);


/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*===========================================================================
FUNCTION: ISec_ClsCreateInstance

    DESCRIPTION
    This function is invoked while the app is being loaded. All Modules must provide this 
    function. Ensure to retain the same name and parameters for this function.
    In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
    that has been provided in AEEAppGen.c. 

    After invoking AEEApplet_New(), this function can do app specific initialization. In this
    example, a generic structure is provided so that app developers need not change app specific
    initialization section every time except for a call to IDisplay_InitAppData(). 
    This is done as follows: InitAppData() is called to initialize AppletData 
    instance. It is app developers responsibility to fill-in app data initialization 
    code of InitAppData(). App developer is also responsible to release memory 
    allocated for data contained in AppletData -- this can be done in 
    IDisplay_FreeAppData().

    PROTOTYPE:
    int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMe,void ** ppObj)

    PARAMETERS:
    clsID: [in]: Specifies the ClassID of the applet which is being loaded

    pIShell: [in]: Contains pointer to the IShell object. 

    pIModule: pin]: Contains pointer to the IModule object to the current module to which
    this app belongs

    ppObj: [out]: On return, *ppObj must point to a valid IApplet structure. Allocation
    of memory for this structure and initializing the base data members is done by AEEApplet_New().

    DEPENDENCIES
    none

    RETURN VALUE
    AEE_SUCCESS: If the app needs to be loaded and if AEEApplet_New() invocation was
    successful
    EFAILED: If the app does not need to be loaded or if errors occurred in 
    AEEApplet_New(). If this function returns FALSE, the app will not be loaded.

    SIDE EFFECTS
    none
===========================================================================*/
WE_INT32 ISec_ClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *pMe, void **ppObj)
{
   if(NULL == pIShell || NULL == pMe || NULL == ppObj)
   {
      return EBADPARM;
   }

   *ppObj = NULL;
   switch(ClsId)
   {
      case AEECLSID_SECWAP:
         {
            return ISecW_New((WE_UINT16)sizeof(ISecW), pIShell,pMe,(ISecW**)ppObj);
         }
      case AEECLSID_SECBRS:
         {
            return ISecB_New((WE_UINT16)sizeof(ISecB), pIShell,pMe,(ISecB**)ppObj);
         }
      case AEECLSID_SECSIGNTEXT:
         {
            return ISignText_New((WE_UINT16)sizeof(ISignText), pIShell,pMe,(ISignText**)ppObj);
         }
      default:
         return ECLASSNOTSUPPORT;
   }
   
}




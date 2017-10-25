/*==================================================================================================
    FILE NAME : sec_mod.c
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
    
    GENERAL DESCRIPTION
        this file is the module file for sec extension.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-07   Kevin Yang        None         Draft
    
==================================================================================================*/

/*==================================================================================================
*   Include File 
*=================================================================================================*/
#include "aeeshell.h"
#include "sec_comm.h"
#include "sec_mod.h"
#include "oem_secmgr.h"

/***************************************************************************************************
*   Prototype Declare Section
***************************************************************************************************/
/* || Functions to be provided by the module developers. */
static WE_UINT32   SecMod_AddRef(IModule *po);
static WE_UINT32   SecMod_Release(IModule *po);
static WE_INT      SecMod_CreateInstance(IModule *po,IShell *pIShell,
                                        AEECLSID ClsId, void **ppObj);
static void SecMod_FreeResources(IModule *po, IHeap *ph, IFileMgr *pfm);

extern WE_INT32 ISec_ClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj);
/*-------------------------------------------------------------------
            Global Constant Definitions
-------------------------------------------------------------------*/


/*-------------------------------------------------------------------
            Global Data Definitions
-------------------------------------------------------------------*/
#ifdef AEE_SIMULATOR
   /* IMPORTANT NOTE: g_pvtAEEStdLibEntry global variable is defined for 
      SDK ONLY! This variable should NOT BE:
   
         (1) overwritten 
         (2) USED DIRECTLY by BREW SDK users. 
   
     g_pvtAEEStdLibEntry is used as an entry point to AEEStdLib,*/
AEEHelperFuncs *g_pvtAEEStdLibEntry;
#endif

/*-------------------------------------------------------------------
            Static variable Definitions
-------------------------------------------------------------------*/


/* =========================================================================
                     FUNCTION DEFINITIONS
   ========================================================================= */

/* =========================================================================
Function:  AEEMod_Load()

Description:
   This is the module load function. It is invoked when the module is being
   loaded. It creates an instance of the AEEMod structure and initializes
   the data members appropriately.

   If running under the AEE Simulator, the function is prefixed with the tag
   __declspec(dllexport) so that this function is exported by the App DLL.
   This function shall be invoked from within the AEE.
   
Prototype:
   boolean AEEMod_Load(IShell *pIShell, char *psz, IModule **ppMod)

Parameters:
   piShell: Pointer to IShell interface
   psz:     String containing name of the resource file for module
   ppMod: [ou]: On return, *ppMod contains a valid pointer to the AEEMod 
               structure

Return Value:
   SUCCESS: If successful
   Error code: IF failed

Comments:
   This function must be the first function in the first module for 
   it to work on the handset!

Side Effects: 
   None

============================================================================*/

#ifdef AEE_LOAD_DLL
__declspec(dllexport) WE_INT AEEMod_Load(IShell *pIShell, void *ph, IModule **ppMod)
#else
#if defined(BREW_MODULE) || defined(FLAT_BREW)
extern WE_INT module_main(IShell *pIShell, void *ph, IModule **ppMod);
WE_INT module_main(IShell *pIShell, void *ph, IModule **ppMod)
#else
WE_INT AEEMod_Load(IShell *pIShell, void *ph, IModule **ppMod)
#endif
#endif
{
    /*Invoke helper function to do the actual loading.*/
    return SecStaticMod_New((WE_INT16)sizeof(SecMod),pIShell,ph,ppMod,NULL,NULL);
}

/*===========================================================================

Function:  AEEStaticMod_New()

Description:
   This is the module load function for static modules. It is directly 
   invoked by static modules when the static module is being loaded. 
   It also serves as a helper function for dynamic modules. 
   It creates an instance of the AEEMod structure and initializes the data
   members appropriately. The only difference between this function and the
   one used for dynamic mods is that this function takes an additional 
   parameter that specifies the CreateInstance function of the static module.

Prototype:
   boolean AEEStaticMod_New(int nSize, IShell *pIShell, void *ph, 
                            IModule **ppMod,PFNMODCREATEINST pfn)

Parameters:
   nSize: Specifies the size (in bytes) of the structure to be allocated for 
           the module.  Typically, this is the size of the AEEMod structure
   piShell: Pointer to IShell interface
   ph:  Not used on phone. In SDK, this is a pointer to the AEEHelperFuncs 
           structure. 
   ppMod: [ou]: On return, *ppMod contains a valid pointer to the AEEMod 
           structure
   pfn: Pointer to the modules' CreateInstance() function

Return Value:
   SUCCESS: If successful
   Error code: IF failed

Comments:  None

Side Effects: None

==============================================================================*/
WE_INT SecStaticMod_New(WE_INT16 nSize, IShell *pIShell, void *ph, IModule **ppMod,
                        PFNMODCREATEINST pfnMC,PFNFREEMODDATA pfnMF)
{
   SecMod *pMe = NULL;
   VTBL(IModule) *modFuncs;

   if (!ppMod || !pIShell) 
   {
      return EFAILED;
   }

   if (nSize < 0) 
   {
      return EBADPARM;
   }
   *ppMod = NULL;
  
#ifdef AEE_SIMULATOR
   if (!ph) 
   {
      return EFAILED;
   } 
   else 
   {
      g_pvtAEEStdLibEntry = (AEEHelperFuncs *)ph;
   }
#endif

   /*Allocate memory for the AEEMod object*/
   if (nSize < (WE_INT16)sizeof(SecMod)) 
   {
      nSize += (WE_INT16)sizeof(SecMod);
   }

   if (NULL == (pMe = (SecMod *)MALLOC((WE_ULONG)((WE_UINT16)nSize + (WE_UINT16)sizeof(IModuleVtbl)))))
   {
      return ENOMEMORY;
   }
   (void)MEMSET(pMe,0,(WE_ULONG)((WE_UINT16)nSize + (WE_UINT16)sizeof(IModuleVtbl)));
   /* Allocate the vtbl and initialize it. Note that the modules and apps 
      must not have any static data. Hence, we need to allocate the vtbl as 
      well.*/

   modFuncs = (IModuleVtbl *)((byte *)pMe + nSize);

   /* Initialize individual entries in the VTBL*/
   modFuncs->AddRef         = SecMod_AddRef;
   modFuncs->Release        = SecMod_Release;
   modFuncs->CreateInstance = SecMod_CreateInstance;
   modFuncs->FreeResources  = SecMod_FreeResources;

   /* initialize the vtable*/
   INIT_VTBL(pMe, IModule, *modFuncs);

/*
    if(NULL == (pMe->pvSecVtbl = MALLOC(sizeof(VTBL(ISec)))))
    {
        FREEIF(pMe->pvGlobalData);
        FREEIF(pMe);
        return ENOMEMORY;
    }
*/ 
   /* initialize the data members*/   
   (void)MEMSET(pMe->astPubUserCertKeyPair,0,(M_SEC_USER_CERT_MAX_SIZE*sizeof(St_WimUCertKeyPairInfo)));
   (void)MEMSET(&(pMe->stInfo),0,sizeof(St_SecInfo));
   
   /* Store address of Module's CreateInstance function*/
   pMe->pfnModCrInst = pfnMC;

   /* Store Address of Module's FreeData function*/
   pMe->pfnModFreeData = pfnMF;

   pMe->m_nRefs = 1;
   pMe->m_pIShell = pIShell;

   /*initial reference of sec*/
   pMe->uiSecBrsRefs = 0;
   pMe->uiSecWapRefs = 0;
   pMe->uiSecSignTextRefs = 0;

   /* have load user_priv file or not. */
   pMe->cHaveLoad = 0;

   (void)ISHELL_AddRef(pIShell);

   *ppMod = (IModule*)pMe;

   ph = ph;
   return SUCCESS;
}

/*===========================================================================

Function:  AEEMod_AddRef()

Description:
   This function increases the referecne count for the AEEMod object

Prototype:
   uint32 AEEMod_AddRef(IModule *po)

Parameters:
   po: Pointer to IModule interface whos reference count is to be incremented

Return Value:
   The updated reference count

Comments:  None

Side Effects: None

==============================================================================*/
static WE_UINT32 SecMod_AddRef(IModule *po)
{
   return (++((SecMod *)po)->m_nRefs);
}


/*===========================================================================

Function:  AEEMod_Release()

Description:
   This function decreases the referecne count for the IModule object. If
   the reference count reaches zero, it does the required cleanup

Prototype:
   uint32 AEEMod_Release(IModule *po)

Parameters:
   po: Pointer to the IModule interface whose reference count needs to be
     decremented.

Return Value:
   The updated reference count

Comments:  None

Side Effects: None

==============================================================================*/
static WE_UINT32 SecMod_Release(IModule *po)
{
   SecMod *pMe = NULL;

   if (NULL == po)
   {
      return EBADPARM;
   }
   pMe = (SecMod *)po;

   if(--pMe->m_nRefs != 0) 
   {
      return pMe->m_nRefs;
   }

   /* Ref count is zero. So, release memory associated with this object.*/

   /*Free the global variable*/

   /* FREEIF(pMe->pvSecVtbl); */
   /* First, release user-specific data if any
      Invoke User's FreeData function if they have registered*/
   if(pMe->pfnModFreeData) 
   {
      pMe->pfnModFreeData(po);
   }
   if(pMe->m_pIShell)
   {
      (void)ISHELL_Release(pMe->m_pIShell);
      pMe->m_pIShell = NULL;
   }

   /*Free the object itself*/
   FREE_VTBL(pMe, IModule);
   FREE(pMe);

   return AEE_SUCCESS;
}


/*===========================================================================

Function:  SecMod_CreateInstance()

Description:
   This function is invoked so as to create the individual classes belonging
   to this module. It is invoked once for each class this module contains. 
   This function invokes the AEEClsCreateInstance() function that the module
   implementer must provide.

Prototype:
   int AEEMod_CreateInstance(IModule *pIModule,IShell *pIShell,AEECLSID ClsId,void **ppObj)

Parameters:
   pIModule: [in]: Pointer to this module
   pIShell: [in]: Pointer to the AEE Shell interface
   ClsID:   [in]: ClassId of the class that needs to be created. 
   ppobj:   [out]: If ClsID points to an applet, then *ppobj must point to a 
            valid structure that implemetns the IApplet interface. If ClsID 
            points to a class that is not an applet, *ppobj must point to a 
            valid IBase*.

Return Value:
   SUCCESS: If successful
   Error code: If failed

Comments:  None

Side Effects: None

==============================================================================*/
static WE_INT SecMod_CreateInstance(IModule *pIModule,IShell *pIShell,
                                    AEECLSID ClsId,void **ppObj)
{
   SecMod    *pme = NULL;
   WE_INT32  nErr = EFAILED;
   if(NULL == pIModule)
   {
      return EFAILED;
   }
   pme = (SecMod *)pIModule;

   /* For a dynamic module, they must supply the AEEClsCreateInstance() 
      function. Hence, invoke it. For a static app, they will have 
      registered the create Instance function. Invoke it.*/
   if (pme->pfnModCrInst) 
   {
      nErr = pme->pfnModCrInst(ClsId, pIShell, pIModule, ppObj);
#if !defined(AEE_STATIC)
   } 
   else 
   {       
      nErr = ISec_ClsCreateInstance(ClsId, pIShell, pIModule, ppObj);

#endif
   }

   return nErr;
}

/*===========================================================================

Function:  AEEMod_FreeResources()

Description:
   This function is invoked so that the module can free any of the resources
   that it has loaded.

Prototype:
   void AEEMod_FreeResources(IModule *po, IHeap *ph, IFileMgr *pfm)

Parameters:
   po: [in]: Pointer to this module
   ph: [in]: Pointer to the AEE IHeap interface
   pfm: Pointer to the resource file

Return Value:
   None

Comments:  None

Side Effects: None

==============================================================================*/
static WE_VOID SecMod_FreeResources(IModule *po, IHeap *ph, IFileMgr *pfm)
{
   (void)po,(void)ph,(void)pfm; /* all unused */
}

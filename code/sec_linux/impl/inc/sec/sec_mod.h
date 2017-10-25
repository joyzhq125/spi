/*==================================================================================================
    HEADER NAME : sec_mod.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
    
    GENERAL DESCRIPTION
        In this file,define the sec module structure.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-07   Kevin Yang        None         Draft
    
==================================================================================================*/

/***************************************************************************************************
*   multi-Include-Prevent Section
***************************************************************************************************/
#ifndef _SEC_MOD_H_
#define _SEC_MOD_H_


/***************************************************************************************************
*   Type Define Section
***************************************************************************************************/
/*Address of IMODULE_CreateInstance() function*/
typedef int (*PFNMODCREATEINST)(AEECLSID, IShell*, IModule*, void**);

/*Address of Freedata function for the module*/
typedef void (*PFNFREEMODDATA)(IModule*);

/*Structure that implements the IModule interface*/
typedef struct _SecMod                                     
{
    DECLARE_VTBL(IModule)    /* Virtual Table with pointers to IModule functions*/
        
    uint32          m_nRefs;          /* Reference count for this module*/
    IShell *        m_pIShell;        /* Pointer to IShell*/
    
    /*Address of CreateInstance function of the module. This is needed for
     static modules*/
    PFNMODCREATEINST    pfnModCrInst; 
    
    /*Address of the function to free the module data. This is needed for static
      modules that define their own data.*/
    PFNFREEMODDATA      pfnModFreeData;   
    
    /*public data*/
    St_WimUCertKeyPairInfo   astPubUserCertKeyPair[M_SEC_USER_CERT_MAX_SIZE]; 

    /*added by bird 070126*/
    St_SecInfo                       stInfo;

    /*added by bird 070202*/
    St_SecSessionRec            astSessionCache[M_SEC_SESSION_CACHE_SIZE];    
    St_SecPeerRec                 astPeerCache[M_SEC_PEER_CACHE_SIZE];    
    /*for Sec wap interface vtbl*/
    uint32  uiSecWapRefs;
    void* pvSecWapVtbl;

    /*for Sec brs interface vtbl */
    uint32  uiSecBrsRefs;
    void* pvSecBrsVtbl;

    /*for Sec SignText vtbl */
    uint32 uiSecSignTextRefs;
    void * pvSecSignTextVtbl;

    /* add for user_priv file load number */
    char cHaveLoad;
    
}SecMod;


/*#define AEE_SIMULATOR
****************************************************************************************************
*   Prototype Declare Section
***************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
    
#ifndef  AEE_LOAD_DLL
#  if ((defined(AEE_SIMULATOR) || defined(WIN32)) && !defined(_AEEWIN)) || ( defined(_AEEWIN) && defined(_M_IX86))
#     define  AEE_LOAD_DLL
#  endif
#endif
    
    /* Prototype for the standard Module Load function. When a module is loaded,
      this is the first function that is invoked.*/
#ifdef AEE_LOAD_DLL
    
    /* Need to prefix this function with __declspec(dllexport) so that this function
       is exported from the DLL. */
    __declspec(dllexport) int  AEEMod_Load(IShell *ps, void * ph, IModule ** pMod);
    
#else
    
    int  AEEMod_Load(IShell *ps, void * ph, IModule ** pMod);
    
#endif   
    
/*Module load function for static modules*/
int      SecStaticMod_New(int16 nSize, IShell *pIShell, void * ph, IModule ** ppMod,PFNMODCREATEINST pfnMC,PFNFREEMODDATA pfnMF);


#ifdef __cplusplus
}
#endif

#endif

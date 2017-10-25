/*===========================================================================

FILE: secapp.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions


#include "AEEFile.h"            // File interface definitions
#include "AEEDB.h"              // Database interface definitions
#include "AEENet.h"             // Socket interface definitions
#include "AEESound.h"           // Sound Interface definitions
#include "AEETapi.h"            // TAPI Interface definitions
#include "AEEHTMLViewer.h"      // IHTML Interface definitions


#include "aeestdlib.h"


#include "sec_comm.h" 

#include "isecw.h"
#include "isecb.h"
#include "isigntext.h"


#include "secwap.bid"
#include "secapp.bid"
#include "secbrs.bid"
#include "secsigntext.bid"




 
#define MAIN_PAGE                   "test_secmain.htm"
#define FILE_NAME_LEN   40
#define BRX_FILE_DIR        "bar/*.bar"

#define ISIGNTESTREPORT         "report/isignresult.txt"

#define ISECWAPTESTREPORT         "report/isecwresult.txt"
#define ISECERRORREPORT         "report/isecerror.txt"

#define ISECBROWSERTESTREPORT         "report/isecbresult.txt"
#define ISECBROWSERERRORREPORT        "report/isecberror.txt"
#define USERCERTREQ                   "user_cert_req"




/*the height of font, displayed in screen */
#define LINE_HEIGHT   20   // hack
#define TOP_LINE      LINE_HEIGHT


#define IFFREE(pi)           { if(pi) {FREE(pi) ; (pi) = NULL;}}



/*-------------------------------------------------------------------
Applet structure. All variables in here are reference via "pMe->"
-------------------------------------------------------------------*/

typedef struct Tag_TestInfo{
        
    char           BrxFileInfo[FILE_NAME_LEN];
}St_TestInfo;



// create an applet structure that's passed around. All variables in
// here will be able to be referenced as static.
typedef struct _secapp {
    AEEApplet      a ;         // First element of this structure must be AEEApplet
    AEEDeviceInfo  DeviceInfo; // always have access to the hardware device information

    // add your own variables here...
    AEERect         m_rc;                       // Device Screen Rect  
    IFileMgr       *m_pIFileMgr;                // File Mgr used to open file
    IHtmlViewer    *m_pIHtmlViewer;             // HTML Viewer to display test menu and result                  // FIFO
    INetMgr        *m_pINetMgr;
    IWebUtil       *m_pIWebUtil;
    ISecW          *pstSecWHandle;
    ISecB          *pstSecBHandle;
    ISignText      *pstSignTextHandle;    
    St_TestInfo    stTestInfo; 
    WE_INT32        iFlag;   

} secapp;

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static  boolean secapp_HandleEvent(secapp* pMe, AEEEvent eCode, 
                                             uint16 wParam, uint32 dwParam);
boolean secapp_InitAppData(secapp* pMe);
void    secapp_FreeAppData(secapp* pMe);


static void NotifyWapCallback
( 
    WE_INT32 iEvent,
    void *hSecAppEvent,        
    void *pvPrivData
);

static void NotifyBrowCallback
( 
    WE_INT32 iEvent,
    void *hSecAppEvent,        
    void *pvPrivData
);

static void NotifySignTextCallback
( 
    WE_INT32 iEvent,
    void *hSecAppEvent,        
    void *pvPrivData
);



static void APP_LoadFile( secapp *pMe, const char *pszFileName);


static void APP_HTMLViewerNotifyCallback( void* pvUser, HViewNotify* pNotify );
static void APP_GotoURL( secapp* pMe, const char *pszURL);

static void APP_DoTest( secapp *pMe, const char *pszContent);






/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*===========================================================================
FUNCTION: AEEClsCreateInstance

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
   int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)

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
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
    *ppObj = NULL;

    if( ClsId == AEECLSID_SECAPP )
    {
        // Create the applet and make room for the applet structure
        if( AEEApplet_New(sizeof(secapp),
                          ClsId,
                          pIShell,
                          po,
                          (IApplet**)ppObj,
                          (AEEHANDLER)secapp_HandleEvent,
                          (PFNFREEAPPDATA)secapp_FreeAppData) ) // the FreeAppData function is called after sending EVT_APP_STOP to the HandleEvent function
                          
        {
            //Initialize applet data, this is called before sending EVT_APP_START
            // to the HandleEvent function
            if(secapp_InitAppData((secapp*)*ppObj))
            {
                //Data initialized successfully
                return(AEE_SUCCESS);
            }
            else
            {
                //Release the applet. This will free the memory allocated for the applet when
                // AEEApplet_New was called.
                IAPPLET_Release((IApplet*)*ppObj);
                return EFAILED;
            }

        } // end AEEApplet_New

    }

    return(EFAILED);
}


/*===========================================================================
FUNCTION SampleAppWizard_HandleEvent

DESCRIPTION
    This is the EventHandler for this app. All events to this app are handled in this
    function. All APPs must supply an Event Handler.

PROTOTYPE:
    boolean SampleAppWizard_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
    pi: Pointer to the AEEApplet structure. This structure contains information specific
    to this applet. It was initialized during the AEEClsCreateInstance() function.

    ecode: Specifies the Event sent to this applet

   wParam, dwParam: Event specific data.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the app has processed the event
  FALSE: If the app did not process the event

SIDE EFFECTS
  none
===========================================================================*/
static boolean secapp_HandleEvent(secapp* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{ 
    
    if (IHTMLVIEWER_HandleEvent(pMe->m_pIHtmlViewer, eCode, wParam, dwParam))
        return TRUE;
    switch (eCode) 
    {
        // App is told it is starting up
        case EVT_APP_START:                        
            // Add your code here...
            APP_LoadFile( pMe, MAIN_PAGE );
            return(TRUE);


        // App is told it is exiting
        case EVT_APP_STOP:
            // Add your code here...

            return(TRUE);


        // App is being suspended 
        case EVT_APP_SUSPEND:
            // Add your code here...

            return(TRUE);


        // App is being resumed
        case EVT_APP_RESUME:
            // Add your code here...

            return(TRUE);


        // An SMS message has arrived for this app. Message is in the dwParam above as (char *)
        // sender simply uses this format "//BREW:ClassId:Message", example //BREW:0x00000001:Hello World
        case EVT_APP_MESSAGE:
            // Add your code here...

            return(TRUE);

        // A key was pressed. Look at the wParam above to see which key was pressed. The key
        // codes are in AEEVCodes.h. Example "AVK_1" means that the "1" key was pressed.
        case EVT_KEY:
            // Add your code here...
              switch (wParam)
            {
                case AVK_CLR:
                    APP_LoadFile(pMe, MAIN_PAGE);
                break;           
                
            }
            return(TRUE);


        // If nothing fits up to this point then we'll just break out
        default:
            break;
   }

   return FALSE;
}


// this function is called when your application is starting up
boolean secapp_InitAppData(secapp* pMe)
{
    // Get the device information for this handset.
    // Reference all the data by looking at the pMe->DeviceInfo structure
    // Check the API reference guide for all the handy device info you can get
    
    pMe->DeviceInfo.wStructSize = sizeof(pMe->DeviceInfo);
    ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&pMe->DeviceInfo);
    
    

    // Insert your code here for initializing or allocating resources...

    if(SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SECWAP, (void **)&(pMe->pstSecWHandle)))
    {
        
        return FALSE;
    }
    ISecW_RegWapEventCB( pMe->pstSecWHandle,pMe,NotifyWapCallback );

    if(SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SECBRS, (void **)&(pMe->pstSecBHandle)))
    {
        
        return FALSE;
    }
    ISecB_RegSecClientEvtCB( pMe->pstSecBHandle,pMe,NotifyBrowCallback );

    if(SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SECSIGNTEXT, (void **)&(pMe->pstSignTextHandle)))
    {
        
        return FALSE;
    }
    ISignText_RegSecClientEvtCB( pMe->pstSignTextHandle,pMe,NotifySignTextCallback );
        




    if (SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WEBUTIL,(void **)&(pMe->m_pIWebUtil)))
    {
        ISecW_Release( pMe->pstSecWHandle );
        return FALSE;
    }

    if(SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_HTML,(void **)&(pMe->m_pIHtmlViewer)))
    {
        ISecW_Release( pMe->pstSecWHandle );
        IWEBUTIL_Release(pMe->m_pIWebUtil);
        return FALSE;
    }
    
    if(SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR ,(void **)&(pMe->m_pIFileMgr)))
    {
        ISecW_Release( pMe->pstSecWHandle );
        IWEBUTIL_Release(pMe->m_pIWebUtil);
        IHTMLVIEWER_Release(pMe->m_pIHtmlViewer);
        return FALSE;
    }
    
    IFILEMGR_Remove( pMe->m_pIFileMgr,"report/isignresult.txt");
    IFILEMGR_Remove( pMe->m_pIFileMgr,"report/isecwresult.txt");
    IFILEMGR_Remove( pMe->m_pIFileMgr,"report/isecerror.txt");
    IFILEMGR_Remove( pMe->m_pIFileMgr,"report/isecbresult.txt");
    IFILEMGR_Remove( pMe->m_pIFileMgr,"report/isecberror.txt");
    IFILEMGR_RmDir( pMe->m_pIFileMgr,"report");
        // Set callback for HTML viewer
    IHTMLVIEWER_SetNotifyFn(pMe->m_pIHtmlViewer, (PFNHVIEWNOTIFY)APP_HTMLViewerNotifyCallback, pMe);
    IHTMLVIEWER_SetProperties(pMe->m_pIHtmlViewer, HVP_SCROLLBAR);
    IHTMLVIEWER_SetLinkColor(pMe->m_pIHtmlViewer, MAKE_RGB(0,0,0));
    SETAEERECT(&pMe->m_rc, 0, 0, pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen);
    // if there have been no failures up to this point then return success
    return TRUE;
}

// this function is called when your application is exiting
void secapp_FreeAppData(secapp* pMe)
{
    // insert your code here for freeing any resources you have allocated...

    // example to use for releasing each interface:
    // if ( pMe->pIMenuCtl != NULL )         // check for NULL first
    // {
    //    IMENUCTL_Release(pMe->pIMenuCtl)   // release the interface
    //    pMe->pIMenuCtl = NULL;             // set to NULL so no problems trying to free later
    // }
    //
    ISecW_Release( pMe->pstSecWHandle );
    ISecB_Release( pMe->pstSecBHandle );
    ISignText_Release( pMe->pstSignTextHandle );
    
    IWEBUTIL_Release(pMe->m_pIWebUtil);
    IHTMLVIEWER_Release(pMe->m_pIHtmlViewer);
    IFILEMGR_Release(pMe->m_pIFileMgr);
}

//base function, this function append write buf into file 
static void WriteResult(secapp *pMe, char * pszFileName,char *pcBuffer, int32 iBufferLength )
{
    
    int32    nBytes = -1;
    IFile  * pFile;
    
    if(NULL == pMe->m_pIFileMgr)
    {           
        return;     
    }
    
    //if file exists, then append , otherwise create it
    pFile = IFILEMGR_OpenFile(pMe->m_pIFileMgr, pszFileName, _OFM_CREATE);

    if(NULL == pFile)
    {
        if(EFILEEXISTS == IFILEMGR_GetLastError(pMe->m_pIFileMgr))
        {
            pFile = IFILEMGR_OpenFile(pMe->m_pIFileMgr, pszFileName, _OFM_APPEND);
        }    
        if(NULL == pFile)
        {
            return;
        }
    }
    
    //move file pointer to file end
    IFILE_Seek(pFile, _SEEK_END, 0);
    if ( pcBuffer && iBufferLength > 0 )
    {    
        nBytes = IFILE_Write( pFile, pcBuffer, iBufferLength );
        if( nBytes != iBufferLength )
        {
            DBGPRINTF("ERROR - Append is dropping stuff!");
        }
    }   
    
    if(pFile)
    {
        IFILE_Release(pFile);    
    }
    
}

static void IntToString( char *pszString,WE_INT32 iConvet )
{
    char acResult[12] = {0,};
    WE_INT32 i = 0;
    WE_INT32 j = 0;
    
    if ( iConvet < 0 )
    {
        pszString[j++] = '-';
        iConvet = -iConvet;
        
    }
    if ( iConvet >= 0 && iConvet <= 9 )
    {
        pszString[j++] = iConvet + 0x30;
        
        pszString[j++] = 0;
        return;
    }
    while (1)
    {
        
        if ( iConvet < 10 )
        {
            acResult[i] = iConvet + 0x30;
            break;
        }
        
        acResult[i++] = iConvet%10 + 0x30;
        iConvet = iConvet/10;        
    }
    while ( i )
    {
        
        pszString[j++] = acResult[i--];
    }
    pszString[j++] = acResult[i];
    
    pszString[j] = 0;
    
}

static void NotifyWapCallback
( 
    WE_INT32 iEvent,
    void *hSecAppEvent,        
    void *pvPrivData
)
{
    secapp *pMe = (secapp *)pvPrivData;
    char acResult[12] = {0,};
    switch ( iEvent  )
    {
        case G_SEARCHPEER_RESP :
        {
                        
            {
                St_SecSearchPeerResp *pstSearchPeer = (St_SecSearchPeerResp *)hSecAppEvent;
                if (NULL == pstSearchPeer)
                {
                    WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "WTLS:WapWtls_ProcSecResp SEARCHPEER--error-- NULL response pointer ",
                             STRLEN("WTLS:WapWtls_ProcSecResp SEARCHPEER--error-- NULL response pointer ")
                            );

                    DBGPRINTF( "WTLS:WapWtls_ProcSecResp SEARCHPEER--error-- NULL response pointer");
                    break ;
                }
                
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "receive sec G_SEARCHPEER_RESP;",
                             STRLEN("receive sec G_SEARCHPEER_RESP;")
                            );
                
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "serach result is ",
                             STRLEN("serach result is ")
                            );
                IntToString( acResult,pstSearchPeer->usResult );                
                   
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             acResult,
                             STRLEN(acResult)
                            );
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "\n",
                             STRLEN("\n")
                            );
                DBGPRINTF( "receive sec G_SEARCHPEER_RESP result %d\n",pstSearchPeer->usResult );
                break;
            }
        }

        case G_SESSIONGET_RESP :            
            {
                St_SecSessionGetResp *pstSessionFetch = (St_SecSessionGetResp *)hSecAppEvent;
                if (NULL == pstSessionFetch)
                {
                    WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "G_SESSIONGET_RESP --error-- NULL response pointer",
                             STRLEN("G_SESSIONGET_RESP --error-- NULL response pointer")
                            );
                    DBGPRINTF("G_SESSIONGET_RESP --error-- NULL response pointer\n");
                    break ;
                }
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "receive sec G_SESSIONGET_RESP;",
                             STRLEN("receive sec G_SESSIONGET_RESP;")
                            );
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "session get result is ",
                             STRLEN("session get result is ")
                            );
                IntToString( acResult,pstSessionFetch->usResult );                
                   
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             acResult,
                             STRLEN(acResult)
                            );
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "\n",
                             STRLEN("\n")
                            );
                break;
            }

        case G_GETCIPHERSUITE_RESP :
            
            {
                St_SecGetCipherSuiteResp *pstGetCipherSuite = (St_SecGetCipherSuiteResp *)hSecAppEvent;
                                            
                if (NULL == pstGetCipherSuite)
                {
                    
                    DBGPRINTF( "WTLS:WapWtls_ProcSecResp GETCIPHERSUITE--error-- no memory space!\n");
                    return ;
                }      
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "receive sec G_GETCIPHERSUITE_RESP;",
                             STRLEN("receive sec G_GETCIPHERSUITE_RESP;")
                            );
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "get cipher suites result is ",
                             STRLEN("get cipher suites result is ")
                            );
                IntToString( acResult,pstGetCipherSuite->usResult );                
                   
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             acResult,
                             STRLEN(acResult)
                            );
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "\n",
                             STRLEN("\n")
                            );
                
                break;
            }
            
        case G_KEYEXCH_RESP :
            
            {
                St_SecKeyExchResp *pstKeyExch = (St_SecKeyExchResp*)hSecAppEvent;
                if (NULL == pstKeyExch)
                {
                    DBGPRINTF( "WTLS:WapWtls_ProcSecResp G_KEYEXCH_RESP--error-- no memory space!\n");
                    return ;
                }

                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "receive sec G_KEYEXCH_RESP;",
                             STRLEN("receive sec G_KEYEXCH_RESP;")
                            );
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "key exchange result is ",
                             STRLEN("key exchange result is ")
                            );
                IntToString( acResult,pstKeyExch->usResult );                
                   
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             acResult,
                             STRLEN(acResult)
                            );
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "\n",
                             STRLEN("\n")
                            );
                break;
            }

        case  G_VERIFYSVRCERT_RESP :
            
            {
                St_SecVerifySvrCertResp *pstVerSvrCert = (St_SecVerifySvrCertResp *)hSecAppEvent;
                if (NULL == pstVerSvrCert)
                {
                     DBGPRINTF( "WTLS:WapWtls_ProcSecResp G_VERIFYSVRCERT_RESP--error-- no memory space!\n");
                    return ;
                }
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "receive sec G_VERIFYSVRCERT_RESP;",
                             STRLEN("receive sec G_VERIFYSVRCERT_RESP;")                            
                            );
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "verify server certificate result is ",
                             STRLEN("verify server certificate result is ")
                            );
                IntToString( acResult,pstVerSvrCert->usResult );                
                   
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             acResult,
                             STRLEN(acResult)
                            );
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "\n",
                             STRLEN("\n")
                            );
                break;
            }
        case G_GETPRF_RESP :
            
            {
                St_SecGetPrfResp *pstPrf = (St_SecGetPrfResp *)hSecAppEvent;
                
                if (NULL == pstPrf)
                {
                    
                    DBGPRINTF( "WTLS:WapWtls_ProcSecResp G_GETPRF_RESP--error-- no memory space!\n");
                    return ;
                }
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "receive sec G_GETPRF_RESP;",
                             STRLEN("receive sec G_GETPRF_RESP;")
                            );
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "prf result is ",
                             STRLEN("prf result is ")
                            );
                IntToString( acResult,pstPrf->usResult );                
                   
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             acResult,
                             STRLEN(acResult)
                            );
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "\n",
                             STRLEN("\n")
                            );
                break;
            }

        case G_GETUSERCERT_RESP :
            
            {
                St_SecGetUserCertResp *pstGetUserCert = (St_SecGetUserCertResp *)hSecAppEvent;
                if (NULL == pstGetUserCert)
                {
                    DBGPRINTF( "WTLS:WapWtls_ProcSecResp G_GETUSERCERT_RESP--error-- no memory space!\n");
                    return ;
                }

                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "receive sec G_GETUSERCERT_RESP;",
                             STRLEN("receive sec G_GETUSERCERT_RESP;")
                            );
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "get user certificate result is ",
                             STRLEN("get user certificate result is ")
                            );
                IntToString( acResult,pstGetUserCert->usResult );                
                   
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             acResult,
                             STRLEN(acResult)
                            );
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "\n",
                             STRLEN("\n")
                            );
                break;
            }

        case G_COMPUTESIGN_RESP :
            
            {
                St_SecCompSignResp *pstCompSig = (St_SecCompSignResp *)hSecAppEvent;
                if (NULL == pstCompSig)
                {
                    
                    DBGPRINTF( " G_COMPUTESIGN_RESP--error-- no memory space!\n");
                    return ;
                }
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "receive sec G_COMPUTESIGN_RESP;",
                             STRLEN("receive sec G_COMPUTESIGN_RESP;")
                            );
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "compute signature result is ",
                             STRLEN("compute signature result is ")
                            );
                IntToString( acResult,pstCompSig->usResult );                
                   
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             acResult,
                             STRLEN(acResult)
                            );
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "\n",
                             STRLEN("\n")
                            );
                APP_LoadFile(pMe, "test_isecwresult.htm");
                break;
            }

        
        case G_Evt_SHOW_DLG :
            
            {
                St_ShowDlg *pstShowDlg = (St_ShowDlg *)hSecAppEvent;
                if (NULL == pstShowDlg)
                {
                    DBGPRINTF( "WTLS:WapWtls_ProcSecResp G_Evt_SHOW_DLG--error-- no memory space!\n");
                    return ;
                }
                WriteResult( pMe,
                             ISECWAPTESTREPORT,                                
                             "receive sec G_Evt_SHOW_DLG;\n",
                             STRLEN("receive sec G_Evt_SHOW_DLG;\n")
                            ); 
                
                break;
            }
        
        default :
            WriteResult( pMe,
                        ISECERRORREPORT,                                
                       "WTLS:WapWtls_ProcSecResp SEARCHPEER--error-- not expect event!\n",
                       STRLEN("WTLS:WapWtls_ProcSecResp SEARCHPEER--error-- not expect event!\n")
                            );
            DBGPRINTF( "WTLS:WapWtls_ProcSecResp SEARCHPEER--error-- not expect event!\n");
    }
       
}

static void NotifyBrowCallback
( 
    WE_INT32 iEvent,
    void *hSecAppEvent,        
    void *pvPrivData
)
{
    secapp *pMe = (secapp *)pvPrivData;
    WE_CHAR acResult[12] = {0};
    switch(iEvent)
    {
        /*Response*/
        case G_GENKEYPAIR_RESP:
        {
            DBGPRINTF( "receive sec G_GENKEYPAIR_RESP\n" );
            {
                St_SecGenKeyPairResp* pstData = (St_SecGenKeyPairResp*)hSecAppEvent;  
                if (NULL == pstData)
                {
                    WriteResult(pMe, ISECBROWSERERRORREPORT,                                
                        "NotifyBrowCallback G_GENKEYPAIR_RESP--error-- NULL response pointer \n",
                        STRLEN("NotifyBrowCallback G_GENKEYPAIR_RESP--error-- NULL response pointer \n")
                        );
                    DBGPRINTF("NotifyBrowCallback G_GENKEYPAIR_RESP--error-- NULL response pointer \n");
                    break ;
                }
                
                WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                    "receive sec G_GENKEYPAIR_RESP;", STRLEN("receive sec G_GENKEYPAIR_RESP; "));                
                WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                    "serach result is:", STRLEN("serach result is:"));
                IntToString( acResult, pstData->usResult );
                WriteResult( pMe, ISECBROWSERTESTREPORT, acResult, STRLEN(acResult)); 
                WriteResult( pMe, ISECBROWSERTESTREPORT, "\n", STRLEN("\n"));
                
                DBGPRINTF( "receive sec G_GENKEYPAIR_RESP result %d\n", pstData->usResult );                
            }
            break;
        }
        
        case G_USERCERTREQ_RESP:
        {
            DBGPRINTF( "receive sec G_USERCERTREQ_RESP\n" );
            {
                St_SecUserCertRequestResp*  pstData = (St_SecUserCertRequestResp*)hSecAppEvent;  
                if (NULL == pstData)
                {
                    WriteResult(pMe, ISECBROWSERERRORREPORT,                                
                        "NotifyBrowCallback G_USERCERTREQ_RESP--error-- NULL response pointer \n",
                        STRLEN("NotifyBrowCallback G_USERCERTREQ_RESP--error-- NULL response pointer \n")
                        );
                    DBGPRINTF("NotifyBrowCallback G_USERCERTREQ_RESP--error-- NULL response pointer \n");
                    break ;
                }
                
                WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                    "receive sec G_USERCERTREQ_RESP; ", STRLEN("receive sec G_USERCERTREQ_RESP; "));                
                WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                    "serach result is:", STRLEN("serach result is:"));
                IntToString( acResult, pstData->iResult );
                WriteResult( pMe, ISECBROWSERTESTREPORT, acResult, STRLEN(acResult)); 
                WriteResult( pMe, ISECBROWSERTESTREPORT, ";", STRLEN(";"));
                
                WriteResult( pMe, USERCERTREQ, (char *)pstData->pucCertReqMsg, pstData->uiCertReqMsgLen);                
                WriteResult( pMe, USERCERTREQ, "\n", STRLEN("\n"));
                DBGPRINTF( "receive sec G_USERCERTREQ_RESP result %d\n", pstData->iResult );                
            }
            break;
        }
        
        case G_Evt_HASH:
        {
            St_Hash* pstData = (St_Hash*)hSecAppEvent;
            St_HashAction stHash = {0};
            DBGPRINTF( "receive sec G_Evt_HASH\n" );
            if (NULL == pstData)
            {
                WriteResult(pMe, ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_Evt_HASH--error-- NULL response pointer \n",
                    STRLEN("NotifyBrowCallback G_Evt_HASH--error-- NULL response pointer \n")
                    );
                DBGPRINTF("NotifyBrowCallback G_Evt_HASH--error-- NULL response pointer \n");
                break ;
            }
            
            WriteResult(pMe, ISECBROWSERTESTREPORT,                                
                "receive sec G_Evt_HASH; ", STRLEN("receive sec G_Evt_HASH; ")); 
            WriteResult(pMe, ISECBROWSERTESTREPORT,                                
                "Display Name is:", STRLEN("Display Name is:"));
            WriteResult(pMe, ISECBROWSERTESTREPORT,                                
                pstData->pcDisplayName, STRLEN(pstData->pcDisplayName));
            WriteResult( pMe, USERCERTREQ, "\n", STRLEN("\n"));
            DBGPRINTF("Display Name is: %s\n", pstData->pcDisplayName );
            
            stHash.pcHashValue = "613786052902159285544924458810";
            stHash.bIsOk = TRUE;
            DBGPRINTF(" Hash value: %s\n", stHash.pcHashValue );
            ISecB_EvtHashAction(pMe->pstSecBHandle, 0, stHash);  
            break;
        }
            
        
        case G_Evt_STORECERT:
        {
            St_StoreCert* pstEvt = (St_StoreCert*)hSecAppEvent;
            DBGPRINTF( "receive sec G_Evt_STORECERT\n" );
            if (NULL == pstEvt)
            {
                WriteResult(pMe, ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_Evt_STORECERT--error-- NULL response pointer \n",
                    STRLEN("NotifyBrowCallback G_Evt_STORECERT--error-- NULL response pointer \n")
                    );
                DBGPRINTF("NotifyBrowCallback G_Evt_STORECERT--error-- NULL response pointer \n");
                break;
            }
            
            if ('C' == pstEvt->cCertType)
            {
                St_StoreCertAction stStoreCert = {0};
                stStoreCert.pcText = NULL;
                stStoreCert.bIsOk = 1;       
                WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                    "store ca cert action is ok!\n", STRLEN("store ca cert action is ok!\n"));                  
                ISecB_EvtStoreCertAction(pMe->pstSecBHandle,0,stStoreCert);
            }
            if ('U' == pstEvt->cCertType)
            { 
                St_StoreCertAction stStoreCert = {0};
                if (NULL != pstEvt->pcSubjectStr)
                {
                    stStoreCert.pcText = NULL;
                    stStoreCert.bIsOk = 1;   
                    WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                        "store user cert action is using subject as name!\n", STRLEN("store cert action is using subject as name!\n"));                       
                }
                else
                {
                    stStoreCert.pcText = "hello";
                    stStoreCert.bIsOk = 1;   
                    WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                        "store user cert action is using hello as name!\n", STRLEN("store cert action is using hello as name!\n"));                       
                    
                }                 
                ISecB_EvtStoreCertAction(pMe->pstSecBHandle,0,stStoreCert);
            } 
            break;
        }
            
        
        case G_Evt_CONFIRM:
        {    
            St_Confirm* pstData = (St_Confirm*)hSecAppEvent; 
            St_ConfirmAction stConfirm = {0};
            DBGPRINTF( "receive sec G_Evt_CONFIRM\n" );
            if (NULL == pstData)
            {
                WriteResult(pMe, ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_Evt_CONFIRM--error-- NULL response pointer \n",
                    STRLEN("NotifyBrowCallback G_Evt_CONFIRM--error-- NULL response pointer \n")
                    );
                DBGPRINTF("NotifyBrowCallback G_Evt_CONFIRM--error-- NULL response pointer \n");
                break ;
            } 
            
            stConfirm.bIsOk = 1;   
            WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                "confirm action is ok!\n", STRLEN("confirm action is ok!\n"));  
            ISecB_EvtConfirmAction(pMe->pstSecBHandle,0,stConfirm);
            break;
        }
            
        
        case G_Evt_NAMECONFIRM:
        {   
            St_NameConfirm* pstData = (St_NameConfirm*)hSecAppEvent;
            St_NameConfirmAction stNameConfirm = {0};
            DBGPRINTF( "receive sec G_Evt_NAMECONFIRM\n" );
            if (NULL == pstData)
            {
                WriteResult(pMe, ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_Evt_NAMECONFIRM--error-- NULL response pointer \n",
                    STRLEN("NotifyBrowCallback G_Evt_NAMECONFIRM--error-- NULL response pointer \n")
                    );
                DBGPRINTF("NotifyBrowCallback G_Evt_NAMECONFIRM--error-- NULL response pointer \n");
                break ;
            } 
            WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                "name is:", STRLEN("name is:"));
            WriteResult( pMe, ISECBROWSERTESTREPORT, pstData->pcCertName, STRLEN(pstData->pcCertName)); 
            WriteResult( pMe, ISECBROWSERTESTREPORT, "\n", STRLEN("\n"));
            
            stNameConfirm.bIsOk = 1;     
            WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                "name confirm action is ok!\n", STRLEN("name confirm action is ok!\n"));        
            ISecB_EvtNameConfirmAction(pMe->pstSecBHandle, 0, stNameConfirm); 
            break;
        }
            
        
        case G_Evt_SHOWCERTLIST:
        { 
            St_ShowCertList* pstData = (St_ShowCertList*)hSecAppEvent;
            St_ShowCertListAction stShowCertList = {0};
            DBGPRINTF( "receive sec G_Evt_SHOWCERTLIST\n" );
            if (NULL == pstData)
            {
                WriteResult(pMe, ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_Evt_SHOWCERTLIST--error-- NULL response pointer \n",
                    STRLEN("NotifyBrowCallback G_Evt_SHOWCERTLIST--error-- NULL response pointer \n")
                    );
                DBGPRINTF("NotifyBrowCallback G_Evt_SHOWCERTLIST--error-- NULL response pointer \n");
                break ;
            }            
            WriteResult(pMe, ISECBROWSERTESTREPORT,                                
                "receive sec G_Evt_SHOWCERTLIST; \n", STRLEN("receive sec G_Evt_SHOWCERTLIST; \n"));
            
            WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                "number of cert is:", STRLEN("number of cert is:"));
            IntToString( acResult, pstData->nbrOfCerts );
            WriteResult( pMe, ISECBROWSERTESTREPORT, acResult, STRLEN(acResult)); 
            WriteResult( pMe, ISECBROWSERTESTREPORT, "\n", STRLEN("\n"));
            
            if (0 == pMe->iFlag%3)
            {
                stShowCertList.iSelId = 0;
                stShowCertList.bIsOk = 1;    /* ok */                
                WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                    "show cert list action is ok!\n", STRLEN("show cert list action is ok!\n"));
            }
            else if (1 == pMe->iFlag%3)
            {
                stShowCertList.iSelId = 0;
                stShowCertList.bIsOk = 0;
                stShowCertList.eRKeyType = 0; /* change wtls cert to diable. */
                WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                    "show cert list action is change wtls to disable!\n", STRLEN("show cert list action is change wtls to disable!\n"));
            }
            else
            {
                stShowCertList.iSelId = 0;
                stShowCertList.bIsOk = 0;
                stShowCertList.eRKeyType = 2; /* cancel */
                WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                    "show cert list action is cancel!\n", STRLEN("show cert list action is cancel!\n"));
            }
            ISecB_EvtShowCertListAction(pMe->pstSecBHandle, 0, stShowCertList);
            pMe->iFlag++;
            break;
        }
            
        
        case G_Evt_SHOWCERTCONTENT: /* need change */
        {    
            St_ShowCertContent* pstData = (St_ShowCertContent*)hSecAppEvent;
            St_ShowCertContentAction stShowCert = {0};
            DBGPRINTF( "receive sec G_Evt_SHOWCERTCONTENT\n" );
            if (NULL == pstData)
            {
                WriteResult(pMe, ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_Evt_SHOWCERTCONTENT--error-- NULL response pointer \n",
                    STRLEN("NotifyBrowCallback G_Evt_SHOWCERTCONTENT--error-- NULL response pointer \n")
                    );
                DBGPRINTF("NotifyBrowCallback G_Evt_SHOWCERTCONTENT--error-- NULL response pointer \n");
                break ;
            }            
            WriteResult(pMe, ISECBROWSERTESTREPORT,                                
                "receive sec G_Evt_SHOWCERTCONTENT; ", STRLEN("receive sec G_Evt_SHOWCERTCONTENT; "));
            
            WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                "subject of cert is:", STRLEN("subject of cert is:"));
            WriteResult( pMe, ISECBROWSERTESTREPORT, pstData->pcSubjectStr, STRLEN(pstData->pcSubjectStr)); 
            WriteResult( pMe, ISECBROWSERTESTREPORT, "\n", STRLEN("\n"));
            
            stShowCert.bIsOk = 0;  /* delete */
            WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                "action of show cert content is delete\n", STRLEN("action of show cert content is delete\n"));
            DBGPRINTF("delete the cert. \n");
            ISecB_EvtShowCertContentAction(pMe->pstSecBHandle,0,stShowCert);
            break;
        }
        
        
        case G_DELCERT_RESP:
        {
            St_SecRemCertResp* pstData = (St_SecRemCertResp*)hSecAppEvent;
            DBGPRINTF( "receive sec G_DELCERT_RESP\n" );
            if (NULL == pstData)
            {
                WriteResult(pMe, ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_DELCERT_RESP--error-- NULL response pointer \n",
                    STRLEN("NotifyBrowCallback G_DELCERT_RESP--error-- NULL response pointer \n")
                    );
                DBGPRINTF("NotifyBrowCallback G_DELCERT_RESP--error-- NULL response pointer \n");
                break ;
            }            
            WriteResult(pMe, ISECBROWSERTESTREPORT,                                
                "receive sec G_DELCERT_RESP; ", STRLEN("receive sec G_DELCERT_RESP; "));
            
            WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                "serach result is:", STRLEN("serach result is:"));
            IntToString( acResult, pstData->usResult );
            WriteResult( pMe, ISECBROWSERTESTREPORT, acResult, STRLEN(acResult)); 
            WriteResult( pMe, ISECBROWSERTESTREPORT, "\n", STRLEN("\n"));
            
            MEMSET(acResult, 0, sizeof(acResult));
            WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                "ID of delete cert is:", STRLEN("ID of delete cert is:"));
            IntToString( acResult, pstData->ucCertId);
            WriteResult( pMe, ISECBROWSERTESTREPORT, acResult, STRLEN(acResult)); 
            WriteResult( pMe, ISECBROWSERTESTREPORT, "\n", STRLEN("\n"));
            
            DBGPRINTF("result is: %d\n", pstData->usResult); 
            break;
        }
            
        
        case G_VIEWGETCERT_RESP:
        {
            St_SecGetCertContentResp* pstData = (St_SecGetCertContentResp*)hSecAppEvent;
            DBGPRINTF( "receive sec G_VIEWGETCERT_RESP\n" );
            if (NULL == pstData)
            {
                WriteResult(pMe, ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_VIEWGETCERT_RESP--error-- NULL response pointer \n",
                    STRLEN("NotifyBrowCallback G_VIEWGETCERT_RESP--error-- NULL response pointer \n")
                    );
                DBGPRINTF("NotifyBrowCallback G_VIEWGETCERT_RESP--error-- NULL response pointer \n");
                break ;
            }            
            WriteResult(pMe, ISECBROWSERTESTREPORT,                                
                "receive sec G_VIEWGETCERT_RESP; ", STRLEN("receive sec G_VIEWGETCERT_RESP; "));
            
            WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                "serach result is:", STRLEN("serach result is:"));
            IntToString( acResult, pstData->usResult );
            WriteResult( pMe, ISECBROWSERTESTREPORT, acResult, STRLEN(acResult)); 
            WriteResult( pMe, ISECBROWSERTESTREPORT, "\n", STRLEN("\n"));
            
            MEMSET(acResult, 0, sizeof(acResult));
            WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                "ID of cert is:", STRLEN("ID of cert is:"));
            IntToString( acResult, pstData->ucCertId);
            WriteResult( pMe, ISECBROWSERTESTREPORT, acResult, STRLEN(acResult)); 
            WriteResult( pMe, ISECBROWSERTESTREPORT, "\n", STRLEN("\n"));
            
            DBGPRINTF("result is: %d\n", pstData->usResult);
            break;
        }
            
        
        case G_GETCONTRACT_RESP:
        {     
            St_SecGetContractsListResp* pstData = (St_SecGetContractsListResp *)hSecAppEvent;
            DBGPRINTF( "receive sec G_GETCONTRACT_RESP\n" );
            if (NULL == pstData)
            {
                WriteResult(pMe, ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_GETCONTRACT_RESP--error-- NULL response pointer \n",
                    STRLEN("NotifyBrowCallback G_GETCONTRACT_RESP--error-- NULL response pointer \n")
                    );
                DBGPRINTF("NotifyBrowCallback G_GETCONTRACT_RESP--error-- NULL response pointer \n");
                break ;
            }            
            WriteResult(pMe, ISECBROWSERTESTREPORT,                                
                "receive sec G_GETCONTRACT_RESP; ", STRLEN("receive sec G_GETCONTRACT_RESP; "));
            
            WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                "serach result is:", STRLEN("serach result is:"));
            IntToString( acResult, pstData->usResult );
            WriteResult( pMe, ISECBROWSERTESTREPORT, acResult, STRLEN(acResult)); 
            WriteResult( pMe, ISECBROWSERTESTREPORT, "\n", STRLEN("\n"));
            
            DBGPRINTF("result is: %d\n", pstData->usResult);
            break;
        }
            
        
        case G_DELCONTRACT_RESP:
        {     
            St_SecDelContractResp* pstData = (St_SecDelContractResp*)hSecAppEvent;
            DBGPRINTF( "receive sec G_DELCONTRACT_RESP\n" );
            if (NULL == pstData)
            {
                WriteResult(pMe, ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_DELCONTRACT_RESP--error-- NULL response pointer \n",
                    STRLEN("NotifyBrowCallback G_DELCONTRACT_RESP--error-- NULL response pointer \n")
                    );
                DBGPRINTF("NotifyBrowCallback G_DELCONTRACT_RESP--error-- NULL response pointer \n");
                break ;
            }            
            WriteResult(pMe, ISECBROWSERTESTREPORT,                                
                "receive sec G_DELCONTRACT_RESP; \n", STRLEN("receive sec G_DELCONTRACT_RESP; \n"));
            
            WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                "serach result is:", STRLEN("serach result is:"));
            IntToString( acResult, pstData->usResult );
            WriteResult( pMe, ISECBROWSERTESTREPORT, acResult, STRLEN(acResult)); 
            WriteResult( pMe, ISECBROWSERTESTREPORT, "\n", STRLEN("\n"));
            
            DBGPRINTF("result is: %d\n", pstData->usResult);
            break;
        }
            
        
        case G_Evt_SHOWCONTRACTLIST:
        {    
            St_ShowContractsList* pstData = (St_ShowContractsList*)hSecAppEvent;
            St_ShowContractsListAction  stContractList = {0};
            
            
            DBGPRINTF( "receive sec G_Evt_SHOWCONTRACTLIST\n" );
            if (NULL == pstData)
            {
                WriteResult(pMe, ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_Evt_SHOWCONTRACTLIST--error-- NULL response pointer \n",
                    STRLEN("NotifyBrowCallback G_Evt_SHOWCONTRACTLIST--error-- NULL response pointer \n")
                    );
                DBGPRINTF("NotifyBrowCallback G_Evt_SHOWCONTRACTLIST--error-- NULL response pointer \n");
                break ;
            } 
            WriteResult(pMe, ISECBROWSERTESTREPORT,                                
                "receive sec G_Evt_SHOWCONTRACTLIST; \n", STRLEN("receive sec G_Evt_SHOWCONTRACTLIST; \n"));
            
            WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                "number of contracts is:", STRLEN("number of contracts is:"));
            IntToString( acResult, pstData->nbrOfContract);
            WriteResult( pMe, ISECBROWSERTESTREPORT, acResult, STRLEN(acResult)); 
            WriteResult( pMe, ISECBROWSERTESTREPORT, "\n", STRLEN("\n"));
            
            DBGPRINTF("number of contracts is: %d\n", pstData->nbrOfContract);            
            if (0 == pMe->iFlag%2)
            {             
                stContractList.iSelId = 0;
                stContractList.bIsOk = 1;
            }
            else /* cancel */
            {
                stContractList.bIsOk = 0;
            }
            ISecB_EvtShowContractsListAction(pMe->pstSecBHandle, 0, stContractList);
            pMe->iFlag++; 
            break;
        }
            
        
        case G_Evt_SHOWCONTRACTCONTENT:
        {       
            St_ShowContractContent* pstData = (St_ShowContractContent*)hSecAppEvent;
            St_ShowContractContentAction stShowContractContent = {0};
            DBGPRINTF( "receive sec G_Evt_SHOWCONTRACTCONTENT\n" );
            if (NULL == pstData)
            {
                WriteResult(pMe, ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_Evt_SHOWCONTRACTCONTENT--error-- NULL response pointer \n",
                    STRLEN("NotifyBrowCallback G_Evt_SHOWCONTRACTCONTENT--error-- NULL response pointer \n")
                    );
                DBGPRINTF("NotifyBrowCallback G_Evt_SHOWCONTRACTCONTENT--error-- NULL response pointer \n");
                break ;
            }
            WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                "signature of contract is:", STRLEN("signature of contract is:"));
            WriteResult( pMe, ISECBROWSERTESTREPORT, pstData->pcSignatureStr, STRLEN(pstData->pcSignatureStr)); 
            WriteResult( pMe, ISECBROWSERTESTREPORT, "\n", STRLEN("\n"));
            
            stShowContractContent.bIsOk = 0; /* delete */     
            WriteResult( pMe, ISECBROWSERTESTREPORT, 
                "show contract action is delete.\n", STRLEN("show contract action is delete.\n"));
            ISecB_EvtShowContractAction(pMe->pstSecBHandle, 0, stShowContractContent);
            break;
        }
              
        
        case G_Evt_SHOWSESSIONCONTENT:
        {      
            St_ShowSessionContent* pstData = (St_ShowSessionContent*)hSecAppEvent;
            DBGPRINTF( "receive sec G_Evt_SHOWSESSIONCONTENT\n" ); 
            if (NULL == pstData)
            {
                WriteResult(pMe, ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_Evt_SHOWSESSIONCONTENT--error-- NULL response pointer \n",
                    STRLEN("NotifyBrowCallback G_Evt_SHOWSESSIONCONTENT--error-- NULL response pointer \n")
                    );
                DBGPRINTF("NotifyBrowCallback G_Evt_SHOWSESSIONCONTENT--error-- NULL response pointer \n");
                break ;
            }
            break;
        }
            
        
        case G_GETSESSIONINFO_RESP:
        {        
            St_SecGetSessionInfoResp* pstData = (St_SecGetSessionInfoResp*)hSecAppEvent;
            DBGPRINTF( "receive sec G_GETSESSIONINFO_RESP\n" );
            if (NULL == pstData)
            {
                WriteResult(pMe, ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_GETSESSIONINFO_RESP--error-- NULL response pointer \n",
                    STRLEN("NotifyBrowCallback G_GETSESSIONINFO_RESP--error-- NULL response pointer \n")
                    );
                DBGPRINTF("NotifyBrowCallback G_GETSESSIONINFO_RESP--error-- NULL response pointer \n");
                break ;
            }
            WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                "serach result is:", STRLEN("serach result is:"));
            IntToString( acResult, pstData->usResult );
            WriteResult( pMe, ISECBROWSERTESTREPORT, acResult, STRLEN(acResult)); 
            WriteResult( pMe, ISECBROWSERTESTREPORT, "\n", STRLEN("\n"));
            break;
        }
            
        
        case G_WTLSCURCLASS_RESP:
        {          
            St_SecGetWtlsCurClassResp* pstData = (St_SecGetWtlsCurClassResp*)hSecAppEvent;
            DBGPRINTF( "receive sec G_WTLSCURCLASS_RESP\n" );
            if (NULL == pstData)
            {
                WriteResult(pMe, ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_WTLSCURCLASS_RESP--error-- NULL response pointer \n",
                    STRLEN("NotifyBrowCallback G_WTLSCURCLASS_RESP--error-- NULL response pointer \n")
                    );
                DBGPRINTF("NotifyBrowCallback G_WTLSCURCLASS_RESP--error-- NULL response pointer \n");
                break ;
            }
            WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                "SEC class is:", STRLEN("SEC class is:"));
            IntToString( acResult, pstData->ucSecClass);
            WriteResult( pMe, ISECBROWSERTESTREPORT, acResult, STRLEN(acResult)); 
            WriteResult( pMe, ISECBROWSERTESTREPORT, "\n", STRLEN("\n"));
            
            MEMSET(acResult, 0, sizeof(acResult));
            WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                "session ready is:", STRLEN("session ready is:"));
            IntToString( acResult, pstData->ucSessionInfoReady);
            WriteResult( pMe, ISECBROWSERTESTREPORT, acResult, STRLEN(acResult)); 
            WriteResult( pMe, ISECBROWSERTESTREPORT, "\n", STRLEN("\n"));
            
            MEMSET(acResult, 0, sizeof(acResult));
            WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                "cert ready is:", STRLEN("cert ready is:"));
            IntToString( acResult, pstData->ucCertReady);
            WriteResult( pMe, ISECBROWSERTESTREPORT, acResult, STRLEN(acResult)); 
            WriteResult( pMe, ISECBROWSERTESTREPORT, "\n", STRLEN("\n"));
            break;
        }
            
        
        case G_GETCURSVRCERT_RESP:
        {      
            St_SecGetCurSvrCertResp* pstData = (St_SecGetCurSvrCertResp*)hSecAppEvent;
            DBGPRINTF( "receive sec G_GETCURSVRCERT_RESP\n" );
            if (NULL == pstData)
            {
                WriteResult(pMe, ISECBROWSERERRORREPORT,                                
                    "NotifyBrowCallback G_GETCURSVRCERT_RESP--error-- NULL response pointer \n",
                    STRLEN("NotifyBrowCallback G_GETCURSVRCERT_RESP--error-- NULL response pointer \n")
                    );
                DBGPRINTF("NotifyBrowCallback G_GETCURSVRCERT_RESP--error-- NULL response pointer \n");
                break ;
            }
            
            WriteResult( pMe, ISECBROWSERTESTREPORT,                                
                "serach result is:", STRLEN("serach result is:"));
            IntToString( acResult, pstData->usResult );
            WriteResult( pMe, ISECBROWSERTESTREPORT, acResult, STRLEN(acResult)); 
            WriteResult( pMe, ISECBROWSERTESTREPORT, "\n", STRLEN("\n"));
            APP_LoadFile(pMe, "test_isecbresult.htm");
            break;
        }
            
        
        default:
            break;
    }
}
           
static void NotifySignTextCallback 
( 
   WE_INT32 iEvent,
   void *hSecAppEvent,        
   void *pvPrivData
)
{
   secapp *pMe = (secapp *)pvPrivData;
   char acResult[12] = {0,};
   switch ( iEvent )
   {
       case G_SIGNTEXT_RESP :
       {
           St_SecSignTextResp *pstSignResp = (St_SecSignTextResp *)hSecAppEvent;
           DBGPRINTF("*************************************\n");
           DBGPRINTF("received event G_SIGNTEXT_RESP !\n");
           DBGPRINTF("Response value: Target ID = %d\n", pstSignResp->iTargetID);
           DBGPRINTF("Response value: Sign ID = %d\n", pstSignResp->iSignId);
           DBGPRINTF("Response value: Type of cert = %d\n", pstSignResp->ucCertificateType);
           DBGPRINTF("Response value: Length of cert = %d\n", pstSignResp->usCertificateLen);
           DBGPRINTF("Response value: Length of hash = %d\n", pstSignResp->usHashedKeyLen);
           DBGPRINTF("Response value: length of signature = %d\n", pstSignResp->usSigLen);
           DBGPRINTF("Response value: Result = %d\n", pstSignResp->ucErr);
           DBGPRINTF("and write result to file !\n");
           DBGPRINTF("*************************************\n");
           if ( NULL == pstSignResp )
           {
               WriteResult( pMe,
                       ISIGNTESTREPORT,                                
                      "sign Resp --error-- NULL pointer;\n",
                      STRLEN("sign Resp --error-- NULL pointer;\n")
                           );
               break;
           }
           WriteResult( pMe,
                        ISIGNTESTREPORT,                                
                        "sign Resp result is;",
                         STRLEN("sign Resp result is;")
                      );
           IntToString( acResult, pstSignResp->ucErr );
           WriteResult( pMe,
                        ISIGNTESTREPORT,                                
                        acResult,
                        STRLEN(acResult)
                           );
           WriteResult( pMe,
                        ISIGNTESTREPORT,                                
                        "\n",
                        STRLEN("\n")
                      );
           break;
       }

       case G_Evt_SELECTCERT:
       {
           /*create dialog*/
           /*free mem*/
           /*dialog action and call sec IF */
           St_SelectCertAction stChooseCerByName = {0};
           stChooseCerByName.bIsOk = 1;
           stChooseCerByName.iSelId = 0;
           /*encode action*/
           ISignText_EvtSelCertAction(pMe->pstSignTextHandle, 0, stChooseCerByName);
           break;
       }
         

       default :
           WriteResult( pMe,
                        ISIGNTESTREPORT,                                
                        "sign response is not expect event;\n",
                         STRLEN("sign response is not expect event;\n")
                      );
   }
   APP_LoadFile(pMe, "test_isigntext.htm");
}





/************************************************************************************************************
* FUNCTION        : APP_LoadFile
* DESCRIPTION     : Load local file data
* PARAMETERS      : pMe
*                   pszFileName
* RETURN          : None
************************************************************************************************************/
static void APP_LoadFile( secapp *pMe, const char *pszFileName)
{
    IFile *pf = NULL;
    char *temp = NULL;    
   

    if( STRSTR(pszFileName,"local") == NULL )
        temp = MALLOC(STRLEN(pszFileName)+STRLEN("local/")+1);

    
    if( temp != NULL )
    {
        SPRINTF(temp,"local/%s",pszFileName);
        temp[STRLEN(pszFileName)+STRLEN("local/")] = 0;
        pf = IFILEMGR_OpenFile(pMe->m_pIFileMgr, temp, _OFM_READ);
    }  
    
    if (pf) 
    {           
        // Set the file from which the viewer will get its text
        IHTMLVIEWER_LoadStream( pMe->m_pIHtmlViewer, (IAStream*)pf);
        
        // Release our reference to the file.  (The HTML viewer is responsible for
        // its own reference count while it uses the stream.)
        IFILE_Release(pf);
    } 
    else 
    {
        // Set the file from which the viewer will get its text
        IHTMLVIEWER_SetData( pMe->m_pIHtmlViewer, "<html><body>ERROR &nbsp<a href=\"test_main.htm\">HOME</a><br>File Not Found</body></html>", -1);
    }

    if( temp != NULL )
        FREE(temp);
}





/************************************************************************************************************
* FUNCTION        : APP_HTMLViewerNotifyCallback
* DESCRIPTION     : Respond HTMLViewer`s event
* PARAMETERS      : pvUser
*                   pNotify
* RETURN          : None
************************************************************************************************************/
static void APP_HTMLViewerNotifyCallback( void* pvUser, HViewNotify* pNotify )
{
   secapp *pMe = (secapp *)pvUser;

   switch(pNotify->code)
   {
        case HVN_REDRAW_SCREEN:
            IHTMLVIEWER_Redraw(pMe->m_pIHtmlViewer);
            IDISPLAY_Update(pMe->a.m_pIDisplay);
        break;
        case HVN_JUMP:
        case HVN_SUBMIT:
            APP_GotoURL(pMe, pNotify->u.jump.pszURL);
        break;
        case HVN_DONE:
        case HVN_CONTENTDONE:
            IHTMLVIEWER_SetRect(pMe->m_pIHtmlViewer, &pMe->m_rc);
            IHTMLVIEWER_Redraw(pMe->m_pIHtmlViewer);
        break;
   }
}

/************************************************************************************************************
* FUNCTION        : APP_GotoURL
* DESCRIPTION     : Turn to a url
* PARAMETERS      : pMe
*                   pszURL
* RETURN          : None
************************************************************************************************************/
static void APP_GotoURL( secapp* pMe, const char *pszURL)
{           
    if(STRBEGINS("test:", pszURL)) 
    {
        // read form data and begin test
        APP_DoTest(pMe, pszURL);
    }
    else
    {
        // goto the specified url
        APP_LoadFile(pMe, pszURL);
    }
}


WE_INT32 GetBrxFileName
(
    secapp *pMe, 
    char *pszBarName
)
{
    IFileMgr *pIFileMgr = NULL;
    
    WE_INT32 iResult = EFAILED;
    FileInfo  stInfo = {0};
    char  *pName = NULL;

    char  *pszNameBuf = NULL;
  
    pIFileMgr = pMe->m_pIFileMgr;    
    pszNameBuf = pMe->stTestInfo.BrxFileInfo;

    iResult = IFILEMGR_EnumInit(pIFileMgr, "bar/*.bar", FALSE);    
    
    if( SUCCESS == iResult )
    {
        while( IFILEMGR_EnumNext(pIFileMgr, &stInfo) )
        {
            
            if(!MEMCMP( pszBarName,stInfo.szName,STRLEN(pszBarName)))
            {
                /*Get the information of directory in the current dir*/
                MEMSET(pszNameBuf, 0, FILE_NAME_LEN);
                pName = (char *)stInfo.szName;
                STRNCPY(pszNameBuf, pName, FILE_NAME_LEN - 1);
                
                return iResult;
            }            
            
        }
        return -100;
          
    }
    else
    {
        iResult = IFILEMGR_GetLastError(pIFileMgr);
    }
    
    return iResult;
}


//get the string value according to item name from named "barfile" file 
char * GetItemString
(
    secapp *pMe, 
    const char   *barfile,
    uint16        ItemID
)   
{       
    AECHAR  *dwBuf = NULL;
    char    *pcBuf  = NULL;
    uint32   dwResSize = 0;
    
    ISHELL_LoadResDataEx(pMe->a.m_pIShell, barfile, ItemID, RESTYPE_STRING, (void *)-1, &dwResSize);        
    if(0 == dwResSize)                              
    {   
        DBGPRINTF("GetItemString: size is 0 when get the ItemID(%d)\n", ItemID);    
        return NULL;
    }   
    else
    {
        dwBuf = (AECHAR *)MALLOC( dwResSize );
        pcBuf = (char *)MALLOC( dwResSize >> 1 );
        
        if ( (NULL == dwBuf) || (NULL == pcBuf) )
        {
            
            IFFREE(dwBuf);
            IFFREE(pcBuf);
            return NULL;
        }
        else
        {
            ISHELL_LoadResString(pMe->a.m_pIShell, barfile, ItemID, dwBuf, dwResSize);
            WSTRTOSTR(dwBuf, pcBuf, dwResSize >> 1);
            IFFREE(dwBuf);
            
            if( ( 0 == STRCMP(pcBuf , "NULL") ) || ( 0 == STRCMP(pcBuf , "null") ) )
            {
                IFFREE(pcBuf);
                pcBuf = NULL;
            }
            
            return pcBuf;
        }
    }
    
}

static void CreateIpAddress( unsigned char *pszIp,const char *pszIpString )
{
    WE_INT32 i = 0;
    for( i = 0; i < 4; i++ )
    {
        pszIp[i] = ATOI(pszIpString);
        pszIpString = STRCHR( pszIpString,'.' );
        if ( NULL ==  pszIpString )
        {
            break;
        }
        pszIpString++;
    }
        
}

static WE_INT32 Test_ISecW_SearchPeer( secapp *pMe )
{
    WE_INT32 iTargetID = 0;                     
    WE_UINT8 aucAddress[6] = { 0,4, };
    
    WE_UINT16 usPortnum = 9203;
    WE_INT32 iResult = -1;
    
    uint16  uiPos = 1001;
    char   *pcItem =NULL; 
    
    
            
    /*StartTestFromNewBrx( pMe ); */
    iResult = GetBrxFileName( pMe, "bar/isecw_searchpeer.bar" );     

    if ( SUCCESS == iResult )
    {   
        do 
        {    
            if ( pcItem )
            {
                IFFREE(pcItem);
            }
            /* target id*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iTargetID = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                break;
            }
            
            uiPos++;
            /* ip address */
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                CreateIpAddress( aucAddress + 2,pcItem );                
                IFFREE( pcItem );
                
            }
            else
            {
                break;
            }
            
            
            uiPos++;
            /* gateway port */
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                usPortnum = ATOI( pcItem );
                IFFREE(pcItem);
            }
            else
            {
                break;
            }
            
            ISecW_SearchPeer( pMe->pstSecWHandle,iTargetID,aucAddress,6,usPortnum );
            uiPos++;
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            
        }while( pcItem );
    }

    
    return iResult;
 
}

static WE_INT32 Test_ISecW_SessionGet( secapp *pMe )
{
    WE_INT32 iTargetID = -1; 
    WE_INT32 iMasterSecretId = -1;
    
    WE_INT32 iResult  = -1;
    uint16  uiPos = 1001;
    char   *pcItem =NULL; 
    
    
    
    /*StartTestFromNewBrx( pMe ); */
    iResult = GetBrxFileName( pMe, "bar/isecw_sessionget.bar" );     

    if ( SUCCESS == iResult )
    {   
        do 
        {    
            if ( pcItem )
            {
                IFFREE(pcItem);
            }
            /* target id*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iTargetID = ATOI(pcItem) ;
                IFFREE(pcItem);
            }
            else
            {
                break;
            }
            
            uiPos++;

            /*master id*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iMasterSecretId = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                break;
            }
            
            ISecW_SessionGet( pMe->pstSecWHandle,iTargetID,iMasterSecretId );
            uiPos++;
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            
        }while( pcItem );
    }
    return iResult;
}


static WE_INT32 Test_ISecW_GetCipherSuite( secapp *pMe )
{
    WE_INT32 iTargetID = -1;   
    
    WE_INT32 iResult  = -1;
    uint16  uiPos = 1001;
    char   *pcItem =NULL; 
    
    
    
    /*StartTestFromNewBrx( pMe ); */
    iResult = GetBrxFileName( pMe, "bar/isecw_getcipher.bar" );
    

    if ( SUCCESS == iResult )
    {   
        do 
        {    
            if ( pcItem )
            {
                IFFREE(pcItem);
            }
            /* target id*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iTargetID = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                break;
            }           
            
            ISecW_GetCipherSuite( pMe->pstSecWHandle,iTargetID );
            uiPos++;
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            
        }while( pcItem );
    }
    return iResult;
}

static WE_INT32 Test_ISecW_KeyExchange( secapp *pMe )
{
    WE_INT32 iTargetID = -1; 
    
    
    WE_INT32 iResult  = -1;
    uint16  uiPos = 1001;
    char   *pcItem =NULL; 
    St_SecWtlsKeyExchParams stParam = {0};
    WE_UCHAR aucRandval[32] = {0};
    WE_UINT8 ucHashAlg = 1;
    
    WE_UINT8 ucExp[3];
    WE_UINT8 ucMod[128];

    IFile *File = NULL;
    FileInfo  stInfo = {0};
    
    /*StartTestFromNewBrx( pMe ); */
    iResult = GetBrxFileName( pMe, "bar/isecw_keyexchange.bar" );
    

    if ( SUCCESS == iResult )
    {   
        do 
        {    
            if ( pcItem )
            {
                IFFREE(pcItem);
            }
            /* target id*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iTargetID = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                break;
            }  
            
            uiPos++;

            /*hash algorithm*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                ucHashAlg = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                break;
            }
            
            MEMSET( &stParam,0,sizeof( stParam ) );
            
            uiPos++;

            /* key exchange algorithm */
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                stParam.ucKeyExchangeSuite = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                break;
            }

            
            switch ( stParam.ucKeyExchangeSuite )
            {
                case E_SEC_WTLS_KEYEXCHANGE_NULL :
                    
                    break;
                case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON      : 
                case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_512  : 
                case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_768  : 
                    uiPos++;

                    /* exponent file name */
                    pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
                    if ( pcItem != NULL )
                    {
                        File = IFILEMGR_OpenFile( pMe->m_pIFileMgr,pcItem ,_OFM_READ );
                        IFILE_Read( File,ucExp,3 );
                        IFILE_Release( File );                       
                        IFFREE(pcItem);
                    }
                    else
                    {
                        break;
                    }

                    uiPos++;

                    /* module file name */
                    pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
                    if ( pcItem != NULL )
                    {
                        File = IFILEMGR_OpenFile( pMe->m_pIFileMgr,pcItem ,_OFM_READ );
                        IFILE_Read( File,ucMod,128 );
                        IFILE_Release( File );                       
                        IFFREE(pcItem);
                    }
                    else
                    {
                        break;
                    }
                    stParam.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usExpLen = 3;            
                    stParam.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent = ucExp;                
                    stParam.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usModLen = 128;
                    stParam.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus = ucMod; 
                                        
                    break;

                case E_SEC_WTLS_KEYEXCHANGE_RSA           : 
                case E_SEC_WTLS_KEYEXCHANGE_RSA_512       : 
                case E_SEC_WTLS_KEYEXCHANGE_RSA_768       :
                    
                    uiPos++;
                    /* certificate file name */
                    pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
                    if ( pcItem != NULL )
                    {
                        iResult = IFILEMGR_GetInfo( pMe->m_pIFileMgr,pcItem,&stInfo);
                        stParam.stParams.stCertificates.usBufLen = stInfo.dwSize + 1;
                        stParam.stParams.stCertificates.pucBuf = MALLOC( stInfo.dwSize + 1 );
                        File = IFILEMGR_OpenFile( pMe->m_pIFileMgr,pcItem ,_OFM_READ );
                        IFILE_Read( File,stParam.stParams.stCertificates.pucBuf + 1,stInfo.dwSize );
                        IFILE_Release( File );                       
                        IFFREE(pcItem);
                    }
                    else
                    {
                        break;
                    }

                    uiPos++;

                    /* certificate tpye 1= wtls; 2 = x509 */
                    pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
                    if ( pcItem != NULL )
                    {
                        stParam.stParams.stCertificates.pucBuf[0] = ATOI(pcItem);
                        IFFREE(pcItem);
                    }
                    else
                    {
                        break;
                    }
                    
                    break;                    
            }
            
            
            ISecW_KeyExchange( pMe->pstSecWHandle,iTargetID,stParam,ucHashAlg,aucRandval );
            
            switch ( stParam.ucKeyExchangeSuite )
            {
                case E_SEC_WTLS_KEYEXCHANGE_RSA           : 
                case E_SEC_WTLS_KEYEXCHANGE_RSA_512       : 
                case E_SEC_WTLS_KEYEXCHANGE_RSA_768       :
                   IFFREE( stParam.stParams.stCertificates.pucBuf ); 
                   break;
            }            
            
            uiPos++;
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            
        }while( pcItem );
    }
    return iResult;
}

static WE_INT32 Test_ISecW_GetUserCert( secapp *pMe )
{
    WE_INT32 iTargetID = -1; 
    
    
    WE_INT32 iResult  = -1;
    uint16  uiPos = 1001;
    char   *pcItem = NULL; 
    WE_UCHAR *pucBuf = NULL;
    WE_INT32 iBufLen  = -1;
    IFile *pFile = NULL; 
        
    FileInfo  stInfo = {0};
    
    /*StartTestFromNewBrx( pMe ); */
    
    
    iResult = GetBrxFileName( pMe, "bar/isecw_getusercert.bar" );   

    if ( SUCCESS == iResult )
    {   
        do 
        {    
            if ( pcItem )
            {
                IFFREE(pcItem);
            }
            /* target id*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iTargetID = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                break;
            } 

            
            
            uiPos++;

            /*file name*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iResult = IFILEMGR_GetInfo( pMe->m_pIFileMgr,pcItem,&stInfo);                
                iBufLen = stInfo.dwSize;
                pucBuf = MALLOC( iBufLen );
                if ( pucBuf )
                {
                    pFile = IFILEMGR_OpenFile( pMe->m_pIFileMgr,pcItem,_OFM_READ );
                    if (pFile)
                    {
                        IFILE_Read( pFile,pucBuf,iBufLen );
                        IFILE_Release( pFile );
                    }
                    
                }
                IFFREE(pcItem);
                
            }
            else
            {
                break;
            } 
            
            ISecW_GetUserCert( pMe->pstSecWHandle,iTargetID,pucBuf,iBufLen );
            IFFREE( pucBuf );
            uiPos++;
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            
        }while( pcItem );
        IFFREE( pucBuf );
    }
    return iResult;
}


static WE_INT32 Test_ISecW_CompSign( secapp *pMe )
{
    WE_INT32 iTargetID = -1; 
    
    
    WE_INT32 iResult  = -1;
    uint16  uiPos = 1001;
    char   *pcItem = NULL; 
    WE_UCHAR *pucBuf = NULL;
    WE_INT32 iBufLen  = 0;
    
    
    WE_UCHAR *pucKeyId = NULL;
    WE_INT32 iKeyIdLen = -1;

    FileInfo  stInfo = {0};
    IFile *pFile = NULL;
                                     
    /*StartTestFromNewBrx( pMe ); */
    
    
    iResult = GetBrxFileName( pMe, "bar/isecw_compsign.bar" );   

    if ( SUCCESS == iResult )
    {   
        do 
        {    
            if ( pcItem )
            {
                IFFREE(pcItem);
            }
            /* target id*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iTargetID = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                break;
            }  

            uiPos++;

            /*key id file name*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iResult = IFILEMGR_GetInfo( pMe->m_pIFileMgr,pcItem,&stInfo);                
                iKeyIdLen = stInfo.dwSize;
                pucKeyId = MALLOC( iKeyIdLen );
                if (pucKeyId)
                {
                    pFile = IFILEMGR_OpenFile( pMe->m_pIFileMgr,pcItem,_OFM_READ );
                    if (pFile)
                    {
                        IFILE_Read( pFile,pucKeyId,iKeyIdLen );
                        IFILE_Release( pFile );
                    }
                }
                IFFREE(pcItem);
                
                
            }
            else
            {
                break;
            }
            
            uiPos++;

            /*buffer file name*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                
                iResult = IFILEMGR_GetInfo( pMe->m_pIFileMgr,pcItem,&stInfo);                
                iBufLen = stInfo.dwSize;
                
                pucBuf = MALLOC( iBufLen );
                if ( pucBuf )
                {
                    pFile = IFILEMGR_OpenFile( pMe->m_pIFileMgr,pcItem,_OFM_READ );
                    if (pFile)
                    {
                        IFILE_Read( pFile,pucBuf,iBufLen );
                        IFILE_Release( pFile );
                    }
                }
                IFFREE(pcItem);
                
            }
            else
            {
                break;
            }
            
            ISecW_CompSign( pMe->pstSecWHandle,iTargetID,pucKeyId,iKeyIdLen,pucBuf,iBufLen );
            IFFREE( pucKeyId );
            IFFREE( pucBuf );
            uiPos++;
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            
        }while( pcItem );
        IFFREE( pucKeyId );
        IFFREE( pucBuf );
    }
    return iResult;
}

static WE_INT32 Test_ISecW_GetPrfResult( secapp *pMe )
{
    WE_INT32 iTargetID = -1;    
    WE_INT32 iResult  = -1;
    uint16  uiPos = 1001;
    char   *pcItem = NULL; 
    WE_UINT8 ucAlg = 1; 
    WE_INT32 iMasterSecretId = -1;
    WE_UCHAR *pucSecret = NULL; 
    WE_INT32 iSecretLen = 0;
    WE_CHAR *pcLabel = NULL; 
    WE_UCHAR *pucSeed = NULL;
    WE_INT32 iSeedLen = 0;
    WE_INT32 iOutputLen = 0;

    FileInfo  stInfo = {0};
    IFile *pFile = NULL;                            
    /*StartTestFromNewBrx( pMe ); */
    
    
    iResult = GetBrxFileName( pMe, "bar/isecw_getprf.bar" );   

    if ( SUCCESS == iResult )
    {   
        do 
        {    
            if ( pcItem )
            {
                IFFREE(pcItem);
            }
            /* target id*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iTargetID = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                break;
            }  

            uiPos++;
            /* hash algorithm*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                ucAlg = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                break;
            } 

            uiPos++;
            /* master secret id*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iMasterSecretId = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                break;
            }
            
            uiPos++;

            /*secret file name*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iResult = IFILEMGR_GetInfo( pMe->m_pIFileMgr,pcItem,&stInfo);                
                iSecretLen = stInfo.dwSize;
                pucSecret = MALLOC( iSecretLen );
                if ( pucSecret )
                {
                    pFile = IFILEMGR_OpenFile( pMe->m_pIFileMgr,pcItem,_OFM_READ );
                    if (pFile)
                    {
                        IFILE_Read( pFile,pucSecret,iSecretLen );
                        IFILE_Release( pFile );
                    }
                }
                IFFREE(pcItem);
                
                
            }
            else
            {
                break;
            }

            uiPos++;
            /* label*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                pcLabel = MALLOC( STRLEN(pcItem) + 1 ) ;
                if ( pcLabel )
                {
                    MEMSET( pcLabel,0,STRLEN(pcItem) + 1 );
                    MEMCPY( pcLabel,pcItem,STRLEN(pcItem));                    
                }
                
                IFFREE(pcItem);
            }
            else
            {
                break;
            }
            
            uiPos++;

            /*seed file name*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                
                iResult = IFILEMGR_GetInfo( pMe->m_pIFileMgr,pcItem,&stInfo);                
                iSeedLen = stInfo.dwSize;
                pucSeed = MALLOC( iSeedLen );
                if ( pucSeed )
                {
                    pFile = IFILEMGR_OpenFile( pMe->m_pIFileMgr,pcItem,_OFM_READ );
                    if (pFile)
                    {
                        IFILE_Read( pFile,pucSeed,iSeedLen );
                        IFILE_Release( pFile );
                    }
                }
                IFFREE(pcItem);
                
            }
            else
            {
                break;
            }

            uiPos++;
            /* out data length */
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                
                iOutputLen = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                break;
            }
            
            
            ISecW_GetPrfResult( pMe->pstSecWHandle,iTargetID,ucAlg,iMasterSecretId,
                                pucSecret,iSecretLen,
                                pcLabel,
                                pucSeed,
                                iSeedLen,
                                iOutputLen );
            IFFREE( pcLabel );
            IFFREE( pucSecret );
            IFFREE( pucSeed );
            uiPos++;
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            
        }while( pcItem );
        IFFREE( pcLabel );
        IFFREE( pucSecret );
        IFFREE( pucSeed );
    }
    return iResult;
}


static WE_INT32 Test_ISecW_VerifySvrCertChain( secapp *pMe )
{
    WE_INT32 iTargetID = -1;    
    WE_INT32 iResult  = -1;
    uint16  uiPos = 1001;
    char   *pcItem = NULL; 
    
    WE_UCHAR *pucBuf = NULL;
    WE_INT32 iBufLen  = 0;
    WE_UCHAR aucAddr[6] = {0,4};
    WE_INT32 iCertType = 0;
    

    FileInfo  stInfo = {0};
    IFile *pFile = NULL;                                
    /*StartTestFromNewBrx( pMe ); */
    
    
    iResult = GetBrxFileName( pMe, "bar/isecw_verifysvrcert.bar" );   

    if ( SUCCESS == iResult )
    {   
        do 
        {    
            if ( pcItem )
            {
                IFFREE(pcItem);
            }
            /* target id*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iTargetID = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                break;
            }  

            uiPos++;
            
            /* certificate type 1 = wtls, 2 = x509*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iCertType = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                break;
            }
            
            uiPos++;

            /*certificate file name*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iResult = IFILEMGR_GetInfo( pMe->m_pIFileMgr,pcItem,&stInfo);
                iBufLen = stInfo.dwSize;
                pucBuf = MALLOC( iBufLen + 1 );
                if ( pucBuf )
                {
                    pFile = IFILEMGR_OpenFile( pMe->m_pIFileMgr,pcItem,_OFM_READ );
                    if (pFile)
                    {
                        pucBuf[0] = iCertType;
                        IFILE_Read( pFile,pucBuf + 1,iBufLen );
                        IFILE_Release( pFile );
                    }
                }
                IFFREE(pcItem);
                
                
            }
            else
            {
                break;
            }

            

            uiPos++;
            /* ipaddress string*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                CreateIpAddress( aucAddr+2,pcItem );                
                IFFREE(pcItem);
            }
            else
            {
                break;
            }           
            
            
            ISecW_VerifySvrCertChain( pMe->pstSecWHandle,iTargetID,pucBuf,iBufLen+1,aucAddr,6 ); 
            IFFREE( pucBuf );
            
            uiPos++;
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            
        }while( pcItem );
        IFFREE( pucBuf );
    }
    return iResult;
}

static WE_INT32 Test_ISecW_RemovePeer( secapp *pMe )
{
    WE_INT32 iMasterSecretID = -1;    
    WE_INT32 iResult  = -1;
    uint16  uiPos = 1001;
    char   *pcItem = NULL;    
    
                                     
    /*StartTestFromNewBrx( pMe ); */
    
    
    iResult = GetBrxFileName( pMe, "bar/isecw_removepeer.bar" );   

    if ( SUCCESS == iResult )
    {   
        do 
        {    
            if ( pcItem )
            {
                IFFREE(pcItem);
            }
            /* target id*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iMasterSecretID = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                break;
            }                      
            
            
            ISecW_RemovePeer( pMe->pstSecWHandle,iMasterSecretID );                    
            
            uiPos++;
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            
        }while( pcItem );
    }
    return iResult;
}

static WE_UINT32 GetCurrentTime( void )
{
    WE_UINT32 uiTmpTime = 0;
    

    uiTmpTime = GETTIMESECONDS();
    uiTmpTime += (WE_UINT32)((10 * 365 + 2 + 5) * 3600 * 24);

    return uiTmpTime;
}


static WE_INT32 Test_ISecW_SessionRenew( secapp *pMe )
{
        
    WE_INT32 iResult  = -1;
    uint16  uiPos = 1001;
    char   *pcItem = NULL; 
    
    WE_INT32 iMasterSecretId = -1; 
    WE_UINT8 ucSessionOptions = 0;
    WE_UCHAR aucSessionId[8] = {0}; 
    WE_UINT8 ucSessionIdLen = 0;
    WE_UINT8 aucCipherSuite[2] = {0};
       
    WE_UINT32 uiCreationTime = GetCurrentTime();

    
                                     
    /*StartTestFromNewBrx( pMe ); */
    
    
    iResult = GetBrxFileName( pMe, "bar/isecw_sessionrenew.bar" );   

    if ( SUCCESS == iResult )
    {   
        do 
        {    
            if ( pcItem )
            {
                IFFREE(pcItem);
            }
            /* Master Secret Id */
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iMasterSecretId = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                break;
            }  

            
            uiPos++;

            /* Session Options */
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                ucSessionOptions = ATOI(pcItem);
                IFFREE(pcItem);
                
            }
            else
            {
                break;
            }

            uiPos++;
            /* Session Id*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                ucSessionIdLen = 8 < STRLEN(pcItem) ? 8 : STRLEN(pcItem);
                MEMCPY( aucSessionId,pcItem,ucSessionIdLen );                
                IFFREE(pcItem);
            }
            else
            {
                break;
            }             
            
            ISecW_SessionRenew( pMe->pstSecWHandle,iMasterSecretId,ucSessionOptions,aucSessionId,
                                ucSessionIdLen,aucCipherSuite,0,
                               ( WE_UINT8 *)"\x00\x00\x00\x00",
                                uiCreationTime 
                              );                     
            
            uiPos++;
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            
        }while( pcItem );
    }
    return iResult;
}

static WE_INT32 Test_ISecW_AttachPeerToSession( secapp *pMe )
{
        
    WE_INT32 iResult  = -1;
    uint16  uiPos = 1001;
    char   *pcItem = NULL; 
    
    WE_UCHAR aucAddress[6] = {0,4,};
    
    WE_UINT16 usPortnum = 0;
    WE_INT32 iMasterSecretId = -1;
                                     
    /*StartTestFromNewBrx( pMe ); */
    
    
    iResult = GetBrxFileName( pMe, "bar/isecw_attechpeertosession.bar" );   

    if ( SUCCESS == iResult )
    {   
        do 
        {    
            if ( pcItem )
            {
                IFFREE(pcItem);
            }
            /* ip address */
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                CreateIpAddress(  aucAddress + 2, pcItem );
                IFFREE(pcItem);
            }
            else
            {
                break;
            }  

            
            uiPos++;

            /* port number */
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                usPortnum = ATOI( pcItem );
                IFFREE(pcItem);
                
            }
            else
            {
                break;
            }

            uiPos++;
            /* master id*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iMasterSecretId =  ATOI( pcItem );          
                IFFREE(pcItem);
            }
            else
            {
                break;
            }            
                                      
            ISecW_AttachPeerToSession( pMe->pstSecWHandle,aucAddress,6,usPortnum,iMasterSecretId );                     
            
            uiPos++;
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            
        }while( pcItem );
    }
    return iResult;
}

static WE_INT32 Test_ISecW_EnableSession( secapp *pMe )
{
        
    WE_INT32 iResult  = -1;
    uint16  uiPos = 1001;
    char   *pcItem = NULL; 
    WE_INT32 iMasterSecretId = 0;
    WE_UINT8 ucIsActive = 0;
    
                                     
    /*StartTestFromNewBrx( pMe ); */
    
    
    iResult = GetBrxFileName( pMe, "bar/isecw_enablesession.bar" );   

    if ( SUCCESS == iResult )
    {   
        do 
        {    
            if ( pcItem )
            {
                IFFREE(pcItem);
            }
            /* Master Secret Id*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iMasterSecretId = ATOI( pcItem );
                IFFREE(pcItem);
            }
            else
            {
                break;
            }            
            

            uiPos++;
            /* active */
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                ucIsActive = ATOI( pcItem );              
                IFFREE(pcItem);
            }
            else
            {
                break;
            }           
            
            
            ISecW_EnableSession( pMe->pstSecWHandle,iMasterSecretId,ucIsActive );                     
            
            uiPos++;
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            
        }while( pcItem );
    }
    return iResult;
}


static WE_INT32 Test_ISecW_SetUpConnection( secapp *pMe )
{
        
    WE_INT32 iResult  = -1;
    uint16  uiPos = 1001;
    char   *pcItem = NULL; 
    St_SecSessionInformation             stSessionInfo = {0};
    WE_INT32 iTargetID = 0;
    WE_INT32 iMasterSecretId = 0;    
    WE_INT32 iSecurityId = 0;
    WE_INT32 iFullHandshake = 0;
                                          
                                     
    /*StartTestFromNewBrx( pMe ); */
    
    
    iResult = GetBrxFileName( pMe, "bar/isecw_setupconn.bar" );   

    if ( SUCCESS == iResult )
    {   
        do 
        {    
            if ( pcItem )
            {
                IFFREE(pcItem);
            }
            /* target Id*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iTargetID = ATOI( pcItem );
                IFFREE(pcItem);
            }
            else
            {
                break;
            }            
            

            uiPos++;
            /* master secret id */
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iMasterSecretId = ATOI( pcItem );              
                IFFREE(pcItem);
            }
            else
            {
                break;
            }      

            uiPos++;
            /* Security Id */
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iSecurityId = ATOI( pcItem );              
                IFFREE(pcItem);
            }
            else
            {
                break;
            } 

            uiPos++;
            /* full handshake flag */
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iFullHandshake = ATOI( pcItem );              
                IFFREE(pcItem);
            }
            else
            {
                break;
            }
             
            ISecW_SetUpConnection( pMe->pstSecWHandle,iTargetID,iMasterSecretId,iSecurityId,
                                   iFullHandshake,stSessionInfo );                     
            
            uiPos++;
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            
        }while( pcItem );
    }
    return iResult;
}

static WE_INT32 Test_ISecW_StopConnection( secapp *pMe )
{
        
    WE_INT32 iResult  = -1;
    uint16  uiPos = 1001;
    char   *pcItem = NULL; 
    
    WE_INT32 iTargetID = 0;
    WE_INT32 iSecurityId = 0;
                                     
    /*StartTestFromNewBrx( pMe ); */
    
    
    iResult = GetBrxFileName( pMe, "bar/isecw_stopconn.bar" );   

    if ( SUCCESS == iResult )
    {   
        do 
        {    
            if ( pcItem )
            {
                IFFREE(pcItem);
            }
            /* target Id*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iTargetID = ATOI( pcItem );
                IFFREE(pcItem);
            }
            else
            {
                break;
            }            
            

            uiPos++;
            /* Security Id */
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iSecurityId = ATOI( pcItem );              
                IFFREE(pcItem);
            }
            else
            {
                break;
            }           
            
            
            ISecW_StopConnection( pMe->pstSecWHandle,iTargetID,iSecurityId );                     
            
            uiPos++;
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            
        }while( pcItem );
    }
    return iResult;
}

static WE_INT32 Test_ISecW_DisableSession( secapp *pMe )
{
        
    WE_INT32 iResult  = -1;
    uint16  uiPos = 1001;
    char   *pcItem = NULL; 
    WE_INT32 iMasterSecretId = 0;
    
    
                                     
    /*StartTestFromNewBrx( pMe ); */
    
    
    iResult = GetBrxFileName( pMe, "bar/isecw_disablesession.bar" );   

    if ( SUCCESS == iResult )
    {   
        do 
        {    
            if ( pcItem )
            {
                IFFREE(pcItem);
            }
            /* Master Secret Id*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iMasterSecretId = ATOI( pcItem );
                IFFREE(pcItem);
            }
            else
            {
                break;
            }                   
            
            
            ISecW_DisableSession( pMe->pstSecWHandle,iMasterSecretId );                     
            
            uiPos++;
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            
        }while( pcItem );
    }
    return iResult;
}






static WE_INT32 Test_ISecB_GenKeyPair( secapp *pMe )
{
    WE_INT32 iTargetID = -1; 
    WE_UINT8 ucKeyType = 0;
    
    WE_INT32 iResult  = -1;
    uint16  uiPos = 1001;
    char   *pcItem =NULL; 
    
    
    
    /*StartTestFromNewBrx( pMe ); */
    iResult = GetBrxFileName( pMe, "bar/isecbgenkeypair.bar" );
    

    if ( SUCCESS == iResult )
    {   
        do 
        {    
            if ( pcItem )
            {
                IFFREE(pcItem);
            }
            /* target id*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iTargetID = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                break;
            }
            
            uiPos++;

            /*Key Type option*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                ucKeyType = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                break;
            }
            
            ISecB_GenKeyPair( pMe->pstSecBHandle,iTargetID,ucKeyType );
            uiPos++;
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            
        }while( pcItem );
    }
    return iResult;
}

static WE_INT32 Test_ISecB_GetRequestUserCert( secapp *pMe )
{
    WE_INT32   iTargetID = -1;  
    WE_INT32   iResult  = -1;
    WE_UINT16  uiPos = 1001;
    WE_CHAR    *pcItem = NULL;    
    St_SecUserCertReqInfo stShowContractContent = {0};
    stShowContractContent.iTargetID = 0;
    stShowContractContent.pucCountry = (WE_UINT8 *)"CN";
    stShowContractContent.pucCity = (WE_UINT8 *)"hangzhou";
    stShowContractContent.pucCompany = (WE_UINT8 *)"techfaith";
    stShowContractContent.pucDepartment = (WE_UINT8 *)"sec";
    stShowContractContent.pucEMail = (WE_UINT8 *)"stone_an@163.com";
    stShowContractContent.pucName = (WE_UINT8 *)"techfaith";
    stShowContractContent.pucProvince = (WE_UINT8 *)"zhejiang";
    stShowContractContent.usCityLen = 8;
    stShowContractContent.usCompanyLen = 9;
    stShowContractContent.usCountryLen = 2;
    stShowContractContent.usDepartmentLen = 3;
    stShowContractContent.usEMailLen = 16;
    stShowContractContent.usNameLen = 5;
    stShowContractContent.usProvinceLen = 8;
    
    /*StartTestFromNewBrx( pMe ); */
    iResult = GetBrxFileName( pMe, "bar/isecbgetrequestusercert.bar");  
    if (SUCCESS == iResult)
    {   
        do 
        {
            IFFREE(pcItem);
            /* target id*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
            if ( pcItem != NULL )
            {
                iTargetID = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                iResult = EFAILED;
                break;
            }
            
            uiPos++;
            /* cert usage */
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
            if ( pcItem != NULL )
            {
                stShowContractContent.ucCertUsage = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                iResult = EFAILED;
                break;
            }
            
            uiPos++;
            /* key usage */
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
            if ( pcItem != NULL )
            {
                stShowContractContent.ucKeyUsage = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                iResult = EFAILED;
                break;
            }
            
            uiPos++;
            /* using signature type*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
            if ( pcItem != NULL )
            {
                stShowContractContent.ucSignType= ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                iResult = EFAILED;
                break;
            }
            
            iResult = ISecB_GetRequestUserCert(pMe->pstSecBHandle, stShowContractContent);
            if (iResult != G_SEC_OK)
            {
                break;
            }
            uiPos++;
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
        }while( pcItem );
    }
    return iResult;
}


static WE_INT32 Test_ISecB_TransferCert( secapp *pMe )
{
    WE_INT32   iTargetID = -1; 
    WE_INT32   iResult  = EFAILED;
    WE_UINT16  uiPos = 1001;
    WE_CHAR    *pcItem =NULL;    
    WE_UCHAR   *pucTemp = NULL;
    WE_INT32   iLen = 0;
    FileInfo   stFileInfo ={0};
    WE_INT32   iHaveReadLen = 0;
    IFile      *pIFile = NULL;
    St_SecCertContent stCertContent ={0};
    
    /*StartTestFromNewBrx( pMe ); */
    iResult = GetBrxFileName( pMe, "bar/isecbtransfercert.bar" );  
    if ( SUCCESS == iResult )
    {   
        do 
        {
            IFFREE(pcItem);
            /* target id*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
            if ( pcItem != NULL )
            {
                iTargetID = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                iResult = EFAILED;
                break;
            }
            
            uiPos++;            
            /* MIME type */
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
            if ( pcItem != NULL )
            {
                stCertContent.pcMime = STRDUP(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                iResult = EFAILED;
                break;
            }
            
            uiPos++;            
            /* CERT NAME */
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
            if ( pcItem != NULL )
            {
                if (SUCCESS != (IFILEMGR_GetInfo(pMe->m_pIFileMgr, pcItem, &stFileInfo)))
                {
                    IFFREE(pcItem);
                    return EFAILED;
                } 
                pIFile = IFILEMGR_OpenFile(pMe->m_pIFileMgr, pcItem, _OFM_READ);
                if (NULL == pIFile)
                {
                    IFFREE(pcItem);
                    return EFAILED;
                }
                IFFREE(pcItem);
                iLen = stFileInfo.dwSize;
                pucTemp = (WE_UCHAR *)MALLOC(iLen);
                if (NULL == pucTemp)
                {
                    return EFAILED;        
                }
                iHaveReadLen = IFILE_Read(pIFile,pucTemp,iLen);
                if (iHaveReadLen < iLen)
                {
                    IFFREE(pucTemp);
                    return EFAILED;        
                }
                IFILE_Release(pIFile);
                stCertContent.pucCertData = pucTemp;
                stCertContent.uiCertLen = iLen;
            }
            else
            {
                iResult = EFAILED;
                break;
            }

            iResult = ISecB_TransferCert(pMe->pstSecBHandle, stCertContent);
            IFFREE(stCertContent.pcMime);
            IFFREE(stCertContent.pucCertData);
            if (iResult != G_SEC_OK)
            {
                break;
            }
            uiPos++;
            pcItem = GetItemString(pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
        }while( pcItem );
        
    }
    return iResult;
}

static WE_INT32 Test_ISecB_GetCertNameList( secapp *pMe )
{
    WE_INT32    iTargetID = -1; 
    WE_INT32    iResult  = EFAILED;
    WE_UINT16   uiPos = 1001;
    WE_CHAR     *pcItem = NULL;
    WE_INT32    iCertOptions = 0;
    
    /*StartTestFromNewBrx( pMe ); */
    iResult = GetBrxFileName( pMe, "bar/isecbgetcertnamelist.bar" );  
    if ( SUCCESS == iResult )
    {   
        do 
        {
            IFFREE(pcItem);
            /* target id*/
            pcItem = GetItemString(pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
            if ( pcItem != NULL )
            {
                iTargetID = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                iResult = EFAILED;
                break;
            }
            
            uiPos++;            
            /* cert type */
            pcItem = GetItemString(pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
            if ( pcItem != NULL )
            {
                iCertOptions = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                iResult = EFAILED;
                break;
            }            

            iResult = ISecB_GetCertNameList(pMe->pstSecBHandle, iTargetID, iCertOptions);
            if (iResult != G_SEC_OK)
            {
                break;
            }
            uiPos++;
            pcItem = GetItemString(pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
        }while( pcItem );
    }
    return iResult;
}

static WE_INT32 Test_ISecB_GetContractList( secapp *pMe )
{
    WE_INT32    iTargetID = -1; 
    WE_INT32    iResult  = EFAILED;
    WE_UINT16   uiPos = 1001;
    WE_CHAR     *pcItem = NULL;
    
    /*StartTestFromNewBrx( pMe ); */
    iResult = GetBrxFileName( pMe, "bar/isecbgetcontractnamelist.bar" );  
    if ( SUCCESS == iResult )
    {   
        do 
        {
            IFFREE(pcItem);
            /* target id*/
            pcItem = GetItemString(pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
            if ( pcItem != NULL )
            {
                iTargetID = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                iResult = EFAILED;
                break;
            }    

            iResult = ISecB_GetContractList(pMe->pstSecBHandle, iTargetID);
            if (iResult != G_SEC_OK)
            {
                break;
            }
            uiPos++;
            pcItem = GetItemString(pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
        }while( pcItem );
    }
    return iResult;
}

static WE_INT32 Test_ISecB_GetSessionInfo( secapp *pMe )
{
    WE_INT32    iTargetID = -1; 
    WE_INT32    iResult  = EFAILED;
    WE_UINT16   uiPos = 1001;
    WE_CHAR     *pcItem = NULL;
    WE_INT32    iSecID = 0;
    
    /*StartTestFromNewBrx( pMe ); */
    iResult = GetBrxFileName( pMe, "bar/isecbgetsessioninfo.bar" );  
    if ( SUCCESS == iResult )
    {   
        do 
        {
            IFFREE(pcItem);
            /* target id*/
            pcItem = GetItemString(pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
            if ( pcItem != NULL )
            {
                iTargetID = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                iResult = EFAILED;
                break;
            }    

            uiPos++;
            /* security ID */
            pcItem = GetItemString(pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
            if ( pcItem != NULL )
            {
                iSecID = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                iResult = EFAILED;
                break;
            }   

            iResult = ISecB_GetSessionInfo(pMe->pstSecBHandle, iTargetID, iSecID);
            if (iResult != G_SEC_OK)
            {
                break;
            }
            uiPos++;
            pcItem = GetItemString(pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
        }while( pcItem );
    }
    return iResult;
}

static WE_INT32 Test_ISecB_GetWtlsCurClass( secapp *pMe )
{
    WE_INT32    iTargetID = -1; 
    WE_INT32    iResult = EFAILED;
    WE_UINT16   uiPos = 1001;
    WE_CHAR     *pcItem = NULL;
    WE_INT32    iSecID = 0;
    
    /*StartTestFromNewBrx( pMe ); */
    iResult = GetBrxFileName( pMe, "bar/isecbgetwtlscurclass.bar" );  
    if ( SUCCESS == iResult )
    {   
        do 
        {
            IFFREE(pcItem);
            /* target id*/
            pcItem = GetItemString(pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
            if ( pcItem != NULL )
            {
                iTargetID = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                iResult = EFAILED;
                break;
            }    

            uiPos++;
            /* security ID */
            pcItem = GetItemString(pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
            if ( pcItem != NULL )
            {
                iSecID = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                iResult = EFAILED;
                break;
            }   

            iResult = ISecB_GetWtlsCurClass(pMe->pstSecBHandle, iTargetID, iSecID);
            if (iResult != G_SEC_OK)
            {
                break;
            }
            uiPos++;
            pcItem = GetItemString(pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
        }while( pcItem );
    }
    return iResult;
}

static WE_INT32 Test_ISecB_GetCurSvrCert( secapp *pMe )
{
    WE_INT32    iTargetID = -1; 
    WE_INT32    iResult  = EFAILED;
    WE_UINT16   uiPos = 1001;
    WE_CHAR     *pcItem = NULL;
    WE_INT32    iSecID = 0;
    
    /*StartTestFromNewBrx( pMe ); */
    iResult = GetBrxFileName( pMe, "bar/isecbgetcursevercert.bar" );  
    if ( SUCCESS == iResult )
    {   
        do 
        {
            IFFREE(pcItem);
            /* target id*/
            pcItem = GetItemString(pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
            if ( pcItem != NULL )
            {
                iTargetID = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                iResult = EFAILED;
                break;
            }    

            uiPos++;
            /* security ID */
            pcItem = GetItemString(pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
            if ( pcItem != NULL )
            {
                iSecID = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                iResult = EFAILED;
                break;
            }   

            iResult = ISecB_GetCurSvrCert(pMe->pstSecBHandle, iTargetID, iSecID);
            if (iResult != G_SEC_OK)
            {
                break;
            }
            uiPos++;
            pcItem = GetItemString(pMe, pMe->stTestInfo.BrxFileInfo, uiPos);
        }while( pcItem );
    }
    return iResult;
}




#define USER_PRIVATE_KEY    "fs:/shared/sec/user_privkey"
#define DATA_TO_BE_SIGN     "http://www.taobao.com"
static WE_INT32 Test_ISignTextHandle( secapp *pMe )
{
    
    WE_INT32 iTargetID = 1;     
    
    WE_INT32 iResult  = -1;
    uint16  uiPos = 1001;
    char   *pcItem =NULL;
    
    WE_INT32 iSignId = 0;
    WE_CHAR  *pcText = "hello";
    WE_INT32 iKeyIdType = -1;
    WE_CHAR  *pcKeyId = NULL;
    WE_INT32 iKeyIdLen = 0;
    WE_INT32 iStringToSignLen = STRLEN(DATA_TO_BE_SIGN);
    WE_CHAR *pcStringToSign = NULL;
    WE_INT32 iOptions  = -1;

    FileInfo  stInfo = {0};
    IFile *pFile = NULL;  
    
    /*StartTestFromNewBrx( pMe ); */
    iResult = GetBrxFileName( pMe, "bar/isigntexthandle.bar" );
    pcStringToSign = MALLOC(iStringToSignLen+1);
    MEMSET(pcStringToSign, 0x00, iStringToSignLen+1);
    MEMCPY(pcStringToSign, DATA_TO_BE_SIGN, iStringToSignLen);

    if ( SUCCESS == iResult )
    {   
        do 
        {    
            if ( pcItem )
            {
                IFFREE(pcItem);
            }
            

            /*key id type*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iKeyIdType = ATOI(pcItem);              
                IFFREE(pcItem);
            }
            else
            {
                break;
            }

            uiPos++;
            /*key id buffer*/
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                if (iKeyIdType == 0)
                {
                    iKeyIdLen = STRLEN(pcItem);
                    pcKeyId = MALLOC(iKeyIdLen+1);
                    MEMSET(pcKeyId, 0x00, iKeyIdLen+1);
                    MEMCPY(pcKeyId, pcItem, iKeyIdLen);
                }
                else if (iKeyIdType == 1 || iKeyIdType == 2)
                {
                    
                    WE_CHAR *pcTemp = NULL;
                    WE_INT32 iLen = 0;

                    iKeyIdLen = 20;
                    pcKeyId = MALLOC(iKeyIdLen+1);
                    MEMSET(pcKeyId, 0x00, iKeyIdLen+1);
                    if(SUCCESS != (IFILEMGR_GetInfo(pMe->m_pIFileMgr, pcItem, &stInfo)))
                    {
                        WE_INT32 ret = 0;
                        ret = IFILEMGR_GetLastError(pMe->m_pIFileMgr);
                        DBGPRINTF("****** We have no user_privkey ! ********");
                        return ret;
                    }
                    iLen = stInfo.dwSize;
                    pcTemp = MALLOC( iLen+1 );
                    MEMSET(pcTemp, 0x00, iLen+1);
                    if ( pcTemp )
                    {
                        pFile = IFILEMGR_OpenFile( pMe->m_pIFileMgr,pcItem,_OFM_READ );
                        if (pFile)
                        {
                            IFILE_Read( pFile,pcTemp,iLen );
                            IFILE_Release( pFile );
                            if (iKeyIdType == 1)
                            {
                                WE_MEMCPY(pcKeyId, 
                                          pcTemp + 2*sizeof(St_WimUCertKeyPairInfo) + 13, 
                                          iKeyIdLen);
                            }
                            else
                            {
                                WE_MEMCPY(pcKeyId, 
                                    pcTemp + 2*sizeof(St_WimUCertKeyPairInfo) + 13+20, 
                                    iKeyIdLen);
                            }
                            IFFREE(pcTemp);
                        }
                    }
                }
                else
                {
                    DBGPRINTF("****** The key type is error ! ********");
                }
                IFFREE(pcItem);
            }
            else
            {
                break;
            }

            uiPos++;

            

            /* options */
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            if ( pcItem != NULL )
            {
                iOptions = ATOI(pcItem);
                IFFREE(pcItem);
            }
            else
            {
                break;
            }
            
            
            ISignText_Handle(   pMe->pstSignTextHandle,
                                iTargetID, 
                                iSignId,
                                pcText,
                                iKeyIdType,
                                pcKeyId,
                                iKeyIdLen,                                
                                pcStringToSign,
                                iStringToSignLen,
                                iOptions         );
            iSignId++;
            IFFREE( pcKeyId );
            
            uiPos++;
            pcItem = GetItemString( pMe, pMe->stTestInfo.BrxFileInfo,uiPos );
            
        }while( pcItem );
        IFFREE( pcKeyId );
    }
    IFFREE( pcStringToSign ); 
    return iResult;
}



static void APP_DoTest( secapp *pMe, const char *pszContent)
{
    WE_INT32 iResult = -1;
    if( STRBEGINS("test:test_isecw", pszContent) )
    {
        APP_LoadFile(pMe, "testing.htm");
        iResult = Test_ISecW_SearchPeer( pMe );
        iResult = Test_ISecW_SessionGet( pMe );
        iResult = Test_ISecW_GetCipherSuite( pMe );
        iResult = Test_ISecW_KeyExchange( pMe ); 
        iResult = Test_ISecW_GetUserCert( pMe );         
        iResult = Test_ISecW_GetPrfResult( pMe );
        iResult = Test_ISecW_VerifySvrCertChain( pMe );
        iResult = Test_ISecW_RemovePeer( pMe );
        iResult = Test_ISecW_SessionRenew( pMe );
        iResult = Test_ISecW_AttachPeerToSession( pMe );
        iResult = Test_ISecW_EnableSession( pMe );
        iResult = Test_ISecW_SetUpConnection( pMe );
        iResult = Test_ISecW_StopConnection( pMe );
        iResult = Test_ISecW_DisableSession( pMe );
        iResult = Test_ISecW_CompSign( pMe ); 
        /*iResult = Test_ISecW_RemovePeer( pMe );*/        
        
        
    }

    if( STRBEGINS("test:test_isecb", pszContent) )
    {
        APP_LoadFile(pMe, "testing.htm");
        iResult = Test_ISecB_GenKeyPair( pMe );
        iResult = Test_ISecB_GetRequestUserCert( pMe );
        iResult = Test_ISecB_TransferCert(pMe);
        iResult = Test_ISecB_GetCertNameList(pMe);
        iResult = Test_ISecB_GetContractList(pMe);
        iResult = Test_ISecB_GetSessionInfo(pMe);
        iResult = Test_ISecB_GetWtlsCurClass(pMe);
        iResult = Test_ISecB_GetCurSvrCert(pMe);
        
        
    }

    if( STRBEGINS("test:test_isigntext", pszContent) )
    {
        APP_LoadFile(pMe, "testing.htm");
        iResult = Test_ISignTextHandle( pMe );        
        
    }
    
    
}








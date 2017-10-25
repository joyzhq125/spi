#ifndef AEE_H
#define AEE_H
/*======================================================
FILE:  AEE.h

SERVICES:  Base level AEE include file.

GENERAL DESCRIPTION:
   Base level definitions, typedefs, etc. for AEE

        Copyright © 1999-2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEGroupIDs.h"
#include "AEEError.h"
#ifdef BREW_MODULE
#include "AEEModuleDefs.h"
#endif

#define DIRECTORY_CHAR  '/'
#define DIRECTORY_STR   "/"


// Standard Headers

#include "AEEComdef.h"


//
// Basic Types
//

typedef struct
{
   uint16   wYear;
   uint16   wMonth;
   uint16   wDay;
   uint16   wHour;
   uint16   wMinute;
   uint16   wSecond;
   uint16   wWeekDay;
} JulianType;

typedef struct
{
   int16    x,y;
   int16   dx, dy;
} AEERect;

enum {
   SC_UNKNOWN,
   SC_ALPHA,
   SC_DIGIT,
   SC_WHITESPACE
};
#ifdef AEE_SIMULATOR
typedef unsigned TChType;
#else
typedef int8 TChType;
#endif

typedef struct _AEEImageInfo
{
   uint16   cx;
   uint16   cy;
   uint16   nColors;
   boolean  bAnimated;
   uint16   cxFrame;
} AEEImageInfo;



#ifndef CONSTRAIN
#define  CONSTRAIN( val, min, max ) (((val) < (min)) ? (min) : ((val) > (max)) ? (max) : (val))
#endif

#ifndef BETWEEN
#define  BETWEEN( val, minGE, maxLT ) ( (unsigned)((unsigned)(val) - (unsigned)(minGE)) < (unsigned)((unsigned)(maxLT) - (unsigned)(minGE)) )
#endif


#ifndef ARRAY_SIZE
#define  ARRAY_SIZE(a) (sizeof((a))/sizeof((a)[0]))
#endif

#ifndef ARRAY_MEMBER
#define  ARRAY_MEMBER(p,a) (((p) >= (a)) && ((p) < ((a) + ARRAY_SIZE(a))))
#endif


//
// SHARED DIRECTORY NAME
//
// Place this directory as the first name in your directory path to access the
// OEM-specified Shared directory.
//
// Example:
//
// Shared\foo.txt - will accesss "foo.txt" in the shared directory
//

#define AEE_SHARED_DIR     "Shared"
#define AEE_RINGER_DIR     "ringers"
#define AEE_ADDRBOOK_DIR   "addrbk"

//************************************************************
//
//  Defines for categorizing memory regions
//
//************************************************************
#define BREW_MEM_NO_ACCESS              1
#define BREW_MEM_READ_ACCESS            2
#define BREW_MEM_WRITE_ACCESS           3

#include "AEEInterface.h"

// ### Deprecated ### 
#define OBJECT(n) \
  typedef struct n n;\
  struct n

// Get object ptr from interface ptr
#define ME(pme,cls,pif)          (cls * pme = ((cls*)(pif)))

// ### Deprecated ### - Use AEEINTERFACE(), in AEEInterface.h
#define QINTERFACE(iname) struct _##iname {\
                             struct VTBL(iname)  *pvt;\
                           };\
                           typedef struct VTBL(iname) VTBL(iname);\
                           struct VTBL(iname)

// ### Deprecated ### - Use INHERIT_IBase() in AEEInterface.h
#define DECLARE_IBASE(iname) \
  uint32  (*AddRef)         (iname*);\
  uint32  (*Release)        (iname*);

// Use as first member of classes that override QINTERFACE()
#define DECLARE_VTBL(iname)      iname   vt##iname;

// returns an 'iname'Vtbl *

#define GET_PVTBL(p,iname)       ((iname*)p)->pvt

// initialize the vtbl

#ifdef BREW_MODULE
#define INIT_VTBL(p,iname,vt) \
{ \
   static void *glueVtbl; \
   if (!glueVtbl) \
      glueVtbl=(*gftbl->gfGlueVtbl)(&vt,sizeof(vt)>>2); \
   else \
      (*gftbl->gfGlueAddRef)(glueVtbl); \
   GET_PVTBL(p,iname) = (VTBL(iname) *) glueVtbl; \
}
#define FREE_VTBL(p,iname) (*gftbl->gfGlueDelRef)(GET_PVTBL(p,iname))
#else
#define INIT_VTBL(p,iname,vt)    (GET_PVTBL(p,iname) = (VTBL(iname) *)&vt)
#define FREE_VTBL(p,iname)
#endif


//
// Basic Class Typedefs
//

typedef struct _IBase           IBase;
typedef struct _IModule         IModule;
typedef struct _IApplet         IApplet;
typedef struct _IControl        IControl;
typedef struct _IAStream        IAStream;
typedef struct _IMemAStream     IMemAStream;
typedef struct _INotifier       INotifier;
typedef struct _IShell          IShell;
typedef struct _IHeap           IHeap;
typedef struct _IFileMgr        IFileMgr;
typedef struct _IImage          IImage;
typedef struct _IImage          IViewer;

#define AEE_HI(dwParm)         ((uint16)(((uint32)(dwParm) >> 16) & 0x0000ffff))
#define AEE_LO(dwParm)         ((uint16)((uint32)(dwParm) &  0x0000ffff))

#define AEE_GET_X(dwParm)      ((int16)AEE_HI(dwParm))
#define AEE_GET_Y(dwParm)      ((int16)AEE_LO(dwParm))
#define AEE_SET_XY(X,Y)        (((uint32) X) << 16 | (((uint32) Y) & 0xffff))
//
// Generic callback header.  This may be used for various interfaces where
// memory allocation for callbacks is prohibitive.
//
// pfnCancel - Filled by the handler.  It can be called at any time to cancel the callback
// pfnNotify - Filled by the caller.  It is called when the callback is issued.
// pNotifyData - Filled by the caller.  It is passed to the callback.
//

typedef struct _AEECallback AEECallback;

typedef void (*PFNCBCANCEL)(AEECallback * pcb);

struct _AEECallback
{
   AEECallback *pNext;            // RESERVED
   void        *pmc;              // RESERVED
   PFNCBCANCEL  pfnCancel;        // Filled by callback handler
   void        *pCancelData;      // Filled by callback handler
   PFNNOTIFY    pfnNotify;        // Filled by caller
   void        *pNotifyData;      // Filled by caller
   void        *pReserved;        // RESERVED - Used by handler
};


#define CALLBACK_Init(pcb,pcf,pcx) {(pcb)->pfnNotify = (PFNNOTIFY)(pcf); \
                                    (pcb)->pNotifyData = (pcx);}

#define CALLBACK_Cancel(pcb)    if (0 != (pcb)->pfnCancel) (pcb)->pfnCancel(pcb)
#define CALLBACK_IsQueued(pcb)  (0 != (pcb)->pfnCancel)


//********************************************************************************************************************************
//
// IBase Definition
//
//********************************************************************************************************************************

QINTERFACE(IBase)
{
   INHERIT_IBase(IBase);
};

#define IBASE_AddRef(p)                   GET_PVTBL(p,IBase)->AddRef(p)
#define IBASE_Release(p)                  GET_PVTBL(p,IBase)->Release(p)

enum {
   AEE_SUSPEND_NORMAL,     // Suspended by another app starting
   AEE_SUSPEND_EXTERNAL    // Suspended by external event
};
#ifdef AEE_SIMULATOR
typedef unsigned AEESuspendReason;
#else
typedef int8 AEESuspendReason;
#endif

typedef struct _AEESuspendInfo
{
   int      nErr;                      // App sets if there is some error
   boolean  bCloseDialogs;             // App sets to FALSE to avoid dialogs closing
} AEESuspendInfo;


#define SYSERR_APP_START      (0)         // General error during app startup (EVT_APP_START)
#define SYSERR_APP_START_OEM  (1)         // OEM failed to allow app to start
#define SYSERR_APP_START_LOAD (2)         // Unable to load starting app
#define SYSERR_MEMFREE        (3)
#define SYSERR_OBJFREE        (4)

typedef struct _AEESysError
{
   AEECLSID       cls;     // AEECLSID of offender...
   int            nErr;    // Error code...
   uint32         nType;   // SYSERR_XXXX
} AEESysError;

#define AEEAsyncError   AEESysError

//
// AEE Events - These events are passed to Applets and/or Controls
//
// Note:  User/App defined events are specified as starting at EVT_USER
//

#define EVT_APP_START            0     // Main App started - dwParam = (AEEAppStart *)
#define EVT_APP_STOP             0x1   // App stopped - dwParam = (boolean *).  Set to FALSE if not closed.
#define EVT_APP_SUSPEND          0x2   // App suspended.  wParam = AEESuspendReason, dwParam = AEESuspendInfo *
#define EVT_APP_RESUME           0x3   // App resume - dwParam = (AEEAppStart *)
#define EVT_APP_CONFIG           0x4   // Alternate App Start - configuration screen should be shown...
#define EVT_APP_HIDDEN_CONFIG    0x5   // Alternate App Start - hidden configuration screen...
#define EVT_APP_BROWSE_URL       0x6   // Called after EVT_APP_START - dwParam = (const AECHAR * pURL)
#define EVT_APP_BROWSE_FILE      0x7   // Called after EVT_APP_START
#define EVT_APP_MESSAGE          0x8   // Text message - wParam = AEESMSEncoding, dwParam = String format dependant on wParam value
#define EVT_ASYNC_ERROR          0x9   // Async error notification - dwParam = AEEAsyncError *
#define EVT_APP_TERMINATE        0xa   // App terminate - Forceful version of EVT_APP_CLOSE
#define EVT_EXIT                 0xb   // BREW shutdown/exit message
#define EVT_APP_RESTART          0xc   // Sent to background apps after upgrades
#define EVT_EXT_STOP             0xd   // Sent to all open apps - dwParam = zero terminated list of AEECLSIDs of extension
#define EVT_EXT_CREATE           0xe   // Extension class created
#define EVT_EXT_RELEASE          0xf   // Extension class released
#define EVT_APP_MESSAGE_EX       0x10  // Text message - wParam = 0, dwParam = Transaction ID
#define EVT_APP_START_BACKGROUND 0x11  // App started in the background
#define EVT_APP_WOULD_START      0x12  // Notification to top-visible app of ISHELL_StartApplet() of same app
#define EVT_APP_LAST_EVENT       (EVT_APP_WOULD_START)

// Key Events

#define EVT_KEY                  0x100 // App key - wParam = KEYCODE, dwParam = Bitflags for modifier keys
#define EVT_KEY_PRESS            0x101 // App keydown - wParam= KEYCODE, dwParam = Bitflags for modifier keys
#define EVT_KEY_RELEASE          0x102 // App keyRelease - wParam = KEYCODE, dwParam = Bitflags for modifier keys
#define EVT_CHAR                 0x104 // Character Event wParam = AECHAR, dwParam = Bitflags for modifier keys
#define EVT_UPDATECHAR           0x105 // Char update Event wParam = AECHAR, dwParam = Bitflags for modifier keys

// key filtering/hook events.
//   These events are sent to running applications with AFLAG_PHONE set.
//   If an AFLAG_PHONE application handles the key hook event, the key event is not passed
//   to the current top visible app.  This provides a way for privileged apps to provide custom
//   handling for specific key events such as AVK_END.
#define EVT_KEY_HOOK             0x110 // hook key - wParam = KEYCODE, dwParam = Bitflags for modifier keys        
#define EVT_KEY_HOOK_PRESS       0x111 // hook keydown - wParam = KEYCODE, dwParam = Bitflags for modifier keys    
#define EVT_KEY_HOOK_RELEASE     0x112 // hook keyRelease - wParam = KEYCODE, dwParam = Bitflags for modifier keys 


// Control Events...

#define EVT_COMMAND              0x200 // App custom control...
#define EVT_CTL_TAB              0x201 // App TAB event sent by controls - dwParam = control, wParam = 0-left, 1-right
#define EVT_CTL_SET_TITLE        0x202 // Message interface to set title - wParam = ID, dwParam = res file (if ID != 0) or text
#define EVT_CTL_SET_TEXT         0x203 // Message interface to set text - wParam = ID, dwParam = res file (if ID != 0) or text
#define EVT_CTL_ADD_ITEM         0x204 // Message interface to add item - dwParam = CtlAddItem
#define EVT_CTL_CHANGING         0x205 // App dwParam = CtlValChange
#define EVT_CTL_MENU_OPEN        0x206 // Sent by ITextCtl before menu activated...
#define EVT_CTL_SKMENU_PAGE_FULL 0x207 // Sent by IMenuCtl when SK menu page is full (dwParam = IMenuCtl *)
#define EVT_CTL_SEL_CHANGED      0x208 // Menu - wParam - selection ID, (dwParam = IMenuCtl *)
#define EVT_CTL_TEXT_MODECHANGED 0x209 // Sent by ITextCtl when mode was changed - wParam = AEETextInputMode, (dwParam = ITextCtl *)

// Dialog Events...

#define EVT_DIALOG_INIT          0x300 // Dialog Event - Controls created, pre-init values, flags, etc.
#define EVT_DIALOG_START         0x301 // Dialog Event - Dialog opening - wShortData = ID, dwParam = IDialog *
#define EVT_DIALOG_END           0x302 // Dialog Event - Dialog completed normally - wShortData = ID, dwParam = IDialog *
#define EVT_COPYRIGHT_END        0x303 // Dialog Event - Copyright dialog ended

// Shell Events...

#define EVT_ALARM                0x400 // App wShortData = uCode
#define EVT_NOTIFY               0x401 // dwParam = AEENotify *
#define EVT_APP_NO_CLOSE         0x404 // App should not be closed
#define EVT_APP_NO_SLEEP         0x405 // App is working - called after long periods of non-idle app
#define EVT_MOD_LIST_CHANGED     0x406 // List of modules changed.  May be sent while app suspended!

#define EVT_BUSY                 (EVT_APP_NO_CLOSE)

// Device Events...

#define EVT_FLIP                 0x500 // wParam = TRUE if open, FALSE if closed...
#define EVT_LOCKED               0x501 // wParam = TRUE if ui is locked...
#define EVT_KEYGUARD             0x502 // wParam = TRUE if keyguard is on...
#define EVT_HEADSET              0x503 // wParam = TRUE if headset is plugged in, FALSE if otherwise
#define EVT_PEN_DOWN             0x504 // dwParam = position: signed x in upper 16 bits, y in lower
#define EVT_PEN_MOVE             0x505 // dwParam = position: signed x in upper 16 bits, y in lower
#define EVT_PEN_UP               0x506 // dwParam = position: signed x in upper 16 bits, y in lower
#define EVT_PEN_STALE_MOVE       0x507 // dwParam = position: signed x in upper 16 bits, y in lower
                                       // Stale move events are reported when the event queue contains
                                       // additional moves
                                       // AEE_GET_X() and AEE_GET_Y() macros may be used to extract
                                       // the position from dwParam
// Clipboard events...

#define EVT_CB_CUT               0x600 // Cut request - dwParam = (const char *)preferred format, NULL for all
#define EVT_CB_COPY              0x601 // Copy request - dwParam = (const char *)preferred format, NULL for all
#define EVT_CB_PASTE             0x602 // Paste request - no parameters

// Events registered via ISHELL_RegisterEvent

#define EVT_DYNAMIC_START        0x6000
#define EVT_DYNAMIC_END          0x6fff

#define EVT_USER                 0x7000   // Start of App/User defined Events.  Private to apps...

#define AEEEvent  uint16

typedef boolean (*AEEHANDLER)(void * pData, AEEEvent evt, uint16 wParam, uint32 lParam);

//
// Note - Control COMMAND IDs above this range are reserved...
//

#define IDC_COMMAND_RESERVED  (0xff00)


//********************************************************************************************************************************
//
// IApplet Interface...
//
//********************************************************************************************************************************

#define INHERIT_IApplet(iname) \
   INHERIT_IBase(iname); \
   boolean  (*HandleEvent)(iname * po, AEEEvent evt, uint16 wp, uint32 dwp)

QINTERFACE(IApplet)
{
   INHERIT_IApplet(IApplet);
};


#define IAPPLET_AddRef(p)                    GET_PVTBL(p,IApplet)->AddRef(p)
#define IAPPLET_Release(p)                   GET_PVTBL(p,IApplet)->Release(p)

//********************************************************************************************************************************
//
// IModule Interface...
//
//********************************************************************************************************************************
#define INHERIT_IModule(iname) \
   INHERIT_IBase(iname); \
   int         (*CreateInstance)(iname * po,IShell * pIShell,AEECLSID ClsId,void ** ppObj); \
   void        (*FreeResources)(iname * po, IHeap * ph, IFileMgr * pfm)

QINTERFACE(IModule)
{
   INHERIT_IModule(IModule);
};

#define IMODULE_AddRef(p)                    GET_PVTBL(p,IModule)->AddRef(p)
#define IMODULE_Release(p)                   GET_PVTBL(p,IModule)->Release(p)
#define IMODULE_CreateInstance(p,ps,id,ppo)  GET_PVTBL(p,IModule)->CreateInstance(p,ps,id,ppo)
#define IMODULE_FreeResources(p,ph,pfm)      GET_PVTBL(p,IModule)->FreeResources(p,ph,pfm)

//************************************************************************************************
//
// IControl Interface
//
//************************************************************************************************

//
// Standard Control Properties, so far not implemented in every control
//

#define CP_BORDER          (0x00000001l)  // Control has a border
#define CP_STATIC          (0x00000002l)  // Control is static.  SetActive does nothing...
#define CP_3D_BORDER       (0x00000004l)  // 3D Border
#define CP_ICON            (0x00000008l)  // Item is a tri-state image
#define CP_USE_DEFAULT     (0x00008000l)  // Use default properties

#define CP_RESERVED        (0x0000ffffl)

//
// AddItem structure - Used for adding items via EVT_CTL_ADD_ITEM
//

typedef struct _CtlAddItem
{
   // pText and pImage are used by default.  If they are
   // not set (NULL), the pszResImage and pszResText will
   // be used with wText and WImage to load the text
   // and/or image respectively.

   const AECHAR * pText;         // Text
   IImage *       pImage;        // Image
   const char *   pszResImage;   // Resource for Image
   const char *   pszResText;    // Resource for Text
   uint16         wText;         // Text ID
   uint16         wFont;         // 0 - Default
   uint16         wImage;        // Large Image for the item
   uint16         wItemID;
   uint32         dwData;
} CtlAddItem;

typedef struct _CtlValChange
{
   IControl *  pc;
   uint32      dwParam;
   boolean     bValid;
} CtlValChange;

// ### Deprecated ### - Use INHERIT_IControl
#define DECLARE_ICONTROL(iname) \
   boolean     (*HandleEvent)    (iname *, AEEEvent evt, uint16 wParam, uint32 dwParam);\
   boolean     (*Redraw)         (iname *);\
   void        (*SetActive)      (iname *, boolean);\
   boolean     (*IsActive)       (iname *);\
   void        (*SetRect)        (iname *, const AEERect *);\
   void        (*GetRect)        (iname *, AEERect *); \
   void        (*SetProperties)  (iname *, uint32);\
   uint32      (*GetProperties)  (iname *); \
   void        (*Reset)          (iname *);


#define INHERIT_IControl(iname) \
   INHERIT_IBase(iname);\
   boolean     (*HandleEvent)    (iname *, AEEEvent evt, uint16 wParam, uint32 dwParam);\
   boolean     (*Redraw)         (iname *);\
   void        (*SetActive)      (iname *, boolean);\
   boolean     (*IsActive)       (iname *);\
   void        (*SetRect)        (iname *, const AEERect *);\
   void        (*GetRect)        (iname *, AEERect *); \
   void        (*SetProperties)  (iname *, uint32);\
   uint32      (*GetProperties)  (iname *); \
   void        (*Reset)          (iname *)


QINTERFACE(IControl)
{
   INHERIT_IControl(IControl);
};

#define ICONTROL_AddRef(p)                      GET_PVTBL(p,IControl)->AddRef(p)
#define ICONTROL_Release(p)                     GET_PVTBL(p,IControl)->Release(p)
#define ICONTROL_HandleEvent(p,ec,wp,dw)        GET_PVTBL(p,IControl)->HandleEvent(p,ec,wp,dw)
#define ICONTROL_Redraw(p)                      GET_PVTBL(p,IControl)->Redraw(p)
#define ICONTROL_SetActive(p,a)                 GET_PVTBL(p,IControl)->SetActive(p,a)
#define ICONTROL_IsActive(p)                    GET_PVTBL(p,IControl)->IsActive(p)
#define ICONTROL_SetRect(p,prc)                 GET_PVTBL(p,IControl)->SetRect(p,prc)
#define ICONTROL_GetRect(p,prc)                 GET_PVTBL(p,IControl)->GetRect(p,prc)
#define ICONTROL_SetProperties(p,props)         GET_PVTBL(p,IControl)->SetProperties(p,props)
#define ICONTROL_GetProperties(p)               GET_PVTBL(p,IControl)->GetProperties(p)
#define ICONTROL_Reset(p)                       GET_PVTBL(p,IControl)->Reset(p)

//***********************************************************************************************
//
//  Notifier Base Class - This base class must be used for any control created as the
//  the result of ISHELL_RegisterNotify.  This base class allows the shell to check with the
//  class to see if there is any pending work to do...
//
//  Wakeup and SetMask are RESERVED for system use ONLY.  They must not be called by
//  client applications!
//
//***********************************************************************************************

//
// Notifier event.  When an INotifier object issues a notification, the
// IAPPLET_HandleEvent call is made and passed a pointer to this structure
// as the dwParam.
//

// Notify Status - Indicates action taken on notify...

enum {
   NSTAT_IGNORED,
   NSTAT_PROCESSED,
   NSTAT_STOP
};
#ifdef AEE_SIMULATOR
typedef unsigned AEENotifyStatus;
#else
typedef int8 AEENotifyStatus;
#endif

typedef struct
{
   AEECLSID          cls;           // Notifier class
   INotifier *       pNotifier;     // Notifier object that issued the notify
   uint32            dwMask;        // Mask of bit(s) that occurred
   void *            pData;         // Event-specific data
   AEENotifyStatus   st;
} AEENotify;

//
// SetMask is called on any change to the Notifier masks with the new array of
// notification masks for the INotifier.  The last mask is 0.
//

#define GET_NOTIFIER_MASK(dwv)         (uint16)(dwv & 0x0000ffffL)
#define GET_NOTIFIER_VAL(dwv)          (uint16)((dwv >> 16) & 0x0000ffff)
#define NOTIFIER_VAL_ANY               (uint16)(0xffff)

// ### Deprecated ### - use INHERIT_INotifier
#define DECLARE_INOTIFIER(iname) \
   void     (*SetMask)(iname *, const uint32 * dwMasks);

#define INHERIT_INotifier(iname) \
   INHERIT_IBase(iname); \
   void     (*SetMask)(iname *, const uint32 * dwMasks)

QINTERFACE(INotifier)
{
   INHERIT_INotifier(INotifier);
};

#define INOTIFIER_AddRef(p)                     GET_PVTBL(p,INotifier)->AddRef(p)
#define INOTIFIER_Release(p)                    GET_PVTBL(p,INotifier)->Release(p)

#include "AEEQueryInterface.h"

//***********************************************************************************************
//
//  IAStream Base Class - This class is exposed as a simple way to read
//  file, network and memory streams.
//
//***********************************************************************************************

// ### Deprecated ### - Use INHERIT_IAStream
#define DECLARE_IASTREAM(iname) \
   void     (*Readable)(iname * po, PFNNOTIFY pfn, void * pUser);\
   int32    (*Read)(iname * po, void * pDest, uint32 nWant);\
   void     (*Cancel)(iname * po, PFNNOTIFY pfn, void * pUser);

#define INHERIT_IAStream(iname) \
   INHERIT_IBase(iname);\
   void     (*Readable) (iname * po, PFNNOTIFY pfn, void * pUser);\
   int32    (*Read)     (iname * po, void * pDest, uint32 nWant);\
   void     (*Cancel)   (iname * po, PFNNOTIFY pfn, void * pUser)

QINTERFACE(IAStream)
{
   INHERIT_IAStream(IAStream);
};

#define IASTREAM_AddRef(p)           GET_PVTBL(p,IAStream)->AddRef(p)
#define IASTREAM_Release(p)          GET_PVTBL(p,IAStream)->Release(p)
#define IASTREAM_Readable(p,pfn,pu)  GET_PVTBL(p,IAStream)->Readable(p,pfn,pu)
#define IASTREAM_Read(p,pd,n)        GET_PVTBL(p,IAStream)->Read(p,pd,n)
#define IASTREAM_Cancel(p)           GET_PVTBL(p,IAStream)->Cancel(p,NULL,NULL)

//***********************************************************************************************
//
//  IMemAStream - This class is provided to allow the caller to use memory
//  chunks as streams.
//
//***********************************************************************************************

#define INHERIT_IMemAStream(iname) \
   INHERIT_IAStream(iname);\
   void (*Set)  (iname * po, byte * pBuff, uint32 dwSize, uint32 dwOffset, boolean bSysMem);\
   void (*SetEx)(iname * po, byte * pBuff, uint32 dwSize, uint32 nOffset, PFNNOTIFY pUserFreeFn, void *pUserFeeData)

QINTERFACE(IMemAStream)
{
   INHERIT_IMemAStream(IMemAStream);
};

#define IMEMASTREAM_AddRef(p)                        GET_PVTBL(p,IMemAStream)->AddRef(p)
#define IMEMASTREAM_Release(p)                       GET_PVTBL(p,IMemAStream)->Release(p)
#define IMEMASTREAM_Readable(p,pfn,pu)               GET_PVTBL(p,IMemAStream)->Readable(p,pfn,pu)
#define IMEMASTREAM_Read(p,pd,n)                     GET_PVTBL(p,IMemAStream)->Read(p,pd,n)
#define IMEMASTREAM_Cancel(p)                        GET_PVTBL(p,IMemAStream)->Cancel(p,NULL,NULL)
#define IMEMASTREAM_Set(p,pb,dw,o,b)                 GET_PVTBL(p,IMemAStream)->Set(p,pb,dw,o,b)
#define IMEMASTREAM_SetEx(p,pb,dw,o,puffn, puffd)    GET_PVTBL(p,IMemAStream)->SetEx(p,pb,dw,o, puffn, puffd)

#if defined(AEE_STATIC)

/*
  || People building dynamic apps (for the sim or for the phone) won't
  ||  have this include file, and shouldn't.
*/
#define AEE_STATIC_H__INC__BY__AEE_H
#include "AEE_static.h"
#undef  AEE_STATIC_H__INC__BY__AEE_H

#endif /* defined(AEE_STATIC) */


#endif    // AEE_H





/*
=======================================================================
  DATA STRUCTURES DOCUMENTATION
=======================================================================

AECHAR

Description:
    AECHAR is BREW defined data type for wide strings.

Definition:
    typedef uint16 AECHAR;

Members:
    None

Comments:
    None

See Also:
    None

========================================================================

DIRECTORY_CHAR

Description:
This macro specifies the directory separation character. Since the character is different on Windows as well
as on handset, using this macro helps to keep the program generic. Also, please note that using '\' in the app
will still cause BREW to replace all '\' with '/' on the handset.

Definition:
#define DIRECTORY_CHAR '/'

Members:
None

Comments:
None

See Also:
None

=======================================================================
DIRECTORY_STR

Description:
This macro specifies the directory separation string. Since the string is different on Windows as well
as on handset, using this macro helps to keep the program generic. Also, please note that using '\' in the app
will still cause BREW to replace all '\' with '/' on the handset.

Definition:
#define DIRECTORY_STR "/"

Members:
None

Comments:
None

See Also:
None

=======================================================================

AEERect

Description:
AEERect is used to define a rectangle used by various Display, Graphics, Text Control,
and other helper functions.

Definition:
typedef struct
{
   int16 x,y;
   int16 dx, dy;
} AEERect;

Members:

x : The horizontal coordinate for the beginning (top left corner) of the rectangle
y : The vertical coordinate for the beginning (top left corner) of the rectangle
dx : The width of the rectangle (in pixels)
dy : The height of the rectangle (in pixels)


Comments:
None

See Also:
None

=======================================================================
TChType

Description:
TChType is an enumeration used to return the type of the wide character by the GETCHTYPE()
function.

Definition:
typedef enum {
   SC_UNKNOWN,
   SC_ALPHA,
   SC_DIGIT,
   SC_WHITESPACE
} TChType;

Members:

SC_UNKNOWN : Unknown type
SC_ALPHA : Alphabet type
SC_DIGIT : Numeric type (0-9)
SC_WHITESPACE : White Space

Comments:
None

See Also:
None

=======================================================================

AEECallback

Description:

    This structure specifies the data and functions for a callback
    registered with the ISHELL_Resume() and other functions.

Definition:
   typedef struct _AEECallback AEECallback;
   struct _AEECallback
   {
      AEECallback *pNext;
      void        *pmc;
      PFNCBCANCEL  pfnCancel;
      void        *pCancelData;
      PFNNOTIFY    pfnNotify;
      void        *pNotifyData;
      void        *pReserved;
   };

Members:

  pNext:  Reserved and the caller should not modify this member

  pmc: Reserved and the caller should not modify this member

  pfnCancel: Pointer to function called by the callback handler if this
             callback is cancelled. The caller should set this pointer to NULL.

  pCancelData: Data passed to pfnCancel. The caller should not modify
                this member.

  pfnNotify: This is the callback function that is invoked by AEE. The
           caller must set this pointer to the function to be called by the
           AEE callback handler.

  pNotifyData: Data to be passed to pfnNotify, the caller must set this
          pointer to the data that must be passed to the pfnNotify function.

  pReserved: Reserved and this member will be used by the callback handler


Comments:
   None

See Also:
   None

=======================================================================
AEEImageInfo

Description:
This structure gets the information about an image

Definition:
typedef struct _AEEImageInfo
{
   uint16 cx;
   uint16 cy;
   uint16 nColors;
   boolean bAnimated;
   uint16 cxFrame;
} AEEImageInfo;

Members:

cx : The width of the image (in pixels)
cy : The height of the image (in pixels)
nColors : The number of colors in the image
bAnimated : TRUE, if the image contains animation
cxFrame : If the image is divided into frames, this member indicates the width of
          each frame

Comments:
None

See Also:
None

=======================================================================
AEESysError

Description:
This structure is used to send notifications about system errors. It is used
in conjunction with NMASK_SHELL_SYS_ERROR and EVT_ASYNC_ERROR

Definition:
typedef struct _AEESysError
{
   AEECLSID       cls;     // AEECLSID of offender...
   int            nErr;    // Error code...
   uint32         nType;   // SYSERR_XXXX
} AEESysError;

Members:

cls : The class (or module) that caused the error. The value of this member depends on each error

nErr : The error code that identifies the system error

nType: The system error type. The supported error types are:

SYSERR_APP_START      (0)  // General error during app startup (EVT_APP_START)
SYSERR_APP_START_OEM  (1)  // OEM failed to allow app to start
SYSERR_APP_START_LOAD (2)  // Unable to load starting app
SYSERR_MEMFREE        (3)  // Module failed to free memory
SYSERR_OBJFREE        (4)  // Module failed to free system objects


Comments:
None

See Also:
EVT_ASYNC_ERROR

=======================================================================
AEEAsyncError

Description:
This is identical to AEESysError. Please see AEESysError for details.

Comments:
None

See Also:
None

=======================================================================
EVT_

EVT_XXX_XXX is the format for AEEEvents.
=======================================================================
AEEEvent

Description:
   The defined AEE events that can be received by an applet or control. For each
event, the wParam and dwParam parameters, if any, that are passed to the applet
or control are given.

Definition:
   The following tables list the event codes and key codes supported by BREW.
Key Codes are received with EVT_KEY, EVT_KEY_PRESS and EVT_KEY_RELEASE.

Applet Events

EVT_APP_START:             Main App started
EVT_APP_STOP:              App stopped : dwParam = (boolean *).  On return, app must set *dwParam to FALSE if it does not want to close and desires to become a background app
EVT_APP_SUSPEND:           App suspended:  wParam == AEESuspendReason, dwParam = AEESuspendInfo *
EVT_APP_RESUME:            App resumed: dwParam = ( AEEAppStart * )
EVT_APP_CONFIG:            Alternate App Start: configuration screen should be shown
EVT_APP_HIDDEN_CONFIG:     Alternate App Start: hidden configuration screen
EVT_APP_BROWSE_URL:        Called after EVT_APP_START: dwParam = (const AECHAR * pURL)
EVT_APP_BROWSE_FILE:       Called after EVT_APP_START
EVT_APP_MESSAGE:           Text message: wParam = AEESMSEncoding, dwParam = String format dependant on wParam value
EVT_ASYNC_ERROR:           Async error notification - dwParam = AEEAsyncError *
EVT_APP_TERMINATE          Forced version of EVT_APP_STOP.  App will be released.
EVT_EXIT                   Sent to any loaded apps when BREW terminates
EVT_APP_RESTART            Sent to background apps after upgrades
EVT_EXT_STOP               Sent to all open apps - dwParam == zero terminated list of AEECLSIDs of extension
EVT_EXT_CREATE             Extension class created
EVT_EXT_RELEASE            Extension class released
EVT_APP_NO_CLOSE           Application should not be closed
EVT_APP_NO_SLEEP           Application is working - called after long periods of non-idle application
EVT_APP_START_BACKGROUND   App started in background
EVT_APP_WOULD_START        Sent when ISHELL_StartApplet() is called on currently top-visible application

Key Events

EVT_KEY:                   App keyup: wParam = KEYCODE, dwParam = Bitflags for modifier keys
EVT_KEY_PRESS:             App keydown: wParam = KEYCODE, dwParam = Bitflags for modifier keys
EVT_KEY_RELEASE:           App keyrelease: wParam = KEYCODE, dwParam = Bitflags for modifier keys
EVT_CHAR                   Character Event: wParam = AECHAR, dwParam = Bitflags for modifier keys
EVT_UPDATECHAR             Character update Event: wParam = AECHAR, dwParam = Bitflags for modifier keys

Key filtering/hook events.
   These events are sent to running applications with AFLAG_PHONE set.
   If an AFLAG_PHONE application handles the key hook event, the key event is not passed
   to the current top visible app.  This provides a way for privileged apps to provide custom
   handling for specific key events such as AVK_END.
                                 
EVT_KEY_HOOK             hook key: wParam = KEYCODE, dwParam = Bitflags for modifier keys        
EVT_KEY_HOOK_PRESS        hook keydown: wParam = KEYCODE, dwParam = Bitflags for modifier keys    
EVT_KEY_HOOK_RELEASE      hook keyrelease: wParam = KEYCODE, dwParam = Bitflags for modifier keys 
                                     
Control Events

EVT_COMMAND:               App custom control
EVT_CTL_TAB:               App TAB event sent by controls: dwParam = control, wParam = 0-left,
                           1-right
EVT_CTL_SET_TITLE:         Message interface to set title: wParam = ID, dwParam = res file
                           (if ID != 0) or text
EVT_CTL_SET_TEXT:          Message interface to set text: wParam = ID, dwParam = res file (if ID
                           != 0) or text
EVT_CTL_ADD_ITEM:          Message interface to add item: dwParam = CtlAddItem
EVT_CTL_CHANGING:          App dwParam = CtlValChange
EVT_CTL_CHANGING:          App dwParam = CtlValChange
EVT_CTL_MENU_OPEN:         Sent by text controls before their associated softkey menus are activated
EVT_CTL_SKMENU_PAGE_FULL:  Sent by IMenuCtl when SK menu page is full
EVT_CTL_SEL_CHANGED:       Sent by IMenuCtl when selection has changed
EVT_CTL_TEXT_MODECHANGED:  Sent by ITextCtl when input mode was changed

Dialog Events

EVT_DIALOG_INIT:           Dialog Event: Controls created, pre-init values, flags, and other items
EVT_DIALOG_START:          Dialog Event: Dialog opening, wParam = ID, dwParam = IDialog *
EVT_DIALOG_END:            Dialog Event: Dialog completed normally, wParam = ID, dwParam = IDialog *
EVT_COPYRIGHT_END:         Dialog Event: Copyright dialog ended

AEE Shell Events

EVT_ALARM:                 App wParam = uCode
EVT_NOTIFY:                dwParam = AEENotify *
EVT_BUSY:                  Sent to app to determine if app can be suspended or stopped

Device Events

EVT_FLIP:                  wParam = TRUE if open, FALSE if closed
EVT_LOCKED:                wParam = TRUE if user interface is locked
EVT_KEYGUARD:              wParam = TRUE if keyguard is on
EVT_HEADSET                wParam = TRUE if headset is plugged in, FALSE if otherwise
EVT_PEN_DOWN               dwParam = position: signed x in upper 16 bits, y in lower
EVT_PEN_MOVE               dwParam = position: signed x in upper 16 bits, y in lower
EVT_PEN_UP                 dwParam = position: signed x in upper 16 bits, y in lower
EVT_PEN_STALE_MOVE         dwParam = position: signed x in upper 16 bits, y in lower
                           Stale move events are reported when the event queue contains
                           additional unprocessed moves
                           For pen positions, 0,0 is the upper left corner of the active frame
                           AEE_GET_X() and AEE_GET_Y() macros may be used to extract
                           the position from dwParam

User defined events

EVT_USER:                  Start of App/User defined Events (Private to apps)

Members:
None:

Comments:
The user defined events start from EVT_USER.

See Also:
None

=======================================================================
AEE Standard Control Properties

Description:
These are the defined Standard Control Properties.

Definition:

CP_BORDER:        Control has a border
CP_STATIC:        Control is static and SetActive has no effect with this control type
CP_3D_BORDER:     3D Border
CP_USE_DEFAULT:   Use default properties
CP_ICON:          Control is a tri-state image
CP_RESERVED:      Item reserved

Members:
None

Comments:
None

See Also:
None

=======================================================================
AEENotify

Description:
A pointer to this structure is passed as dwParam when EVT_NOTIFY event is sent to
an app. An app receives this event as part of the notification(s) that it has registered
for.

Definition:
   typedef struct
   {
   AEECLSID cls;
   INotifier * pNotifier;
   uint32 dwMask;
   void * pData;
   AEENotifyStatus st;
   } AEENotify;

Members:

cls: Notifier Class
pNotifier: Notifier Object that issued the Notify
dwMask: Mask of bits that occurred
pData: Notification-specific data
st: Indicates to IShell if the app processed the notificaiot

Comments:
None

See Also:
ISHELL_RegisterNotify()
ISHELL_Notify()
AEENotifyStatus

=============================================================================

AEENotifyStatus

Description:
This enumerated type defines the notification status values that are returned to
the shell by an applet that receives a notification. The applet returns the status
of its processing of the notification by setting the st member of the AEENotify
structure it is passed along with the EVT_NOTIFY event.

Definition:
typedef enum {
   NSTAT_PROCESSED,
   NSTAT_IGNORED,
   NSTAT_STOP
} AEENotifyStatus;

Members:

NSTAT_PROCESSED: The applet successfully processed the notification
NSTAT_IGNORED: The applet ignored the notification
NSTAT_STOP: The applet processed the notification, and the notification should not
be sent to any other applets that have registered to be notified of this event.

Comments:
None.

See Also:
ISHELL_RegisterNotify()
ISHELL_Notify()
AEENotify

=============================================================================

AEEHANDLER

Description:
    This data type is a function pointer that specifies the prototype for the event
handling function in BREW. The IApplet interface consists of a member function with
this prototype

Definition:
    typedef boolean (*AEEHANDLER)
    (
    void * pData,
    AEEEvent evt,
    uint16 wParam,
    uint32 lParam
    );

Members:
    pData :  Interface or data pointer
    evt :  Event Code
    wParam :  Event-specific parameter
    dwParam :  Event-specific parameter

Comments:
    None

See Also:
    None

=============================================================================
AEESuspendReason

Description:
    This is sent as wParam along with the EVT_APP_SUSPEND event. This parameter
specifies the cause for the application to be suspended.
    The following are the possible values:
    AEE_SUSPEND_NORMAL: Suspended by another application starting
    AEE_SUSPEND_EXTERNAL: Suspended by external event (ex:incoming call)

Definition:
    typedef enum {
    AEE_SUSPEND_NORMAL, // Suspended by another application starting
    AEE_SUSPEND_EXTERNAL // Suspended by external event
    } AEESuspendReason;

Members:
    AEE_SUSPEND_NORMAL: Suspended by another application starting
    AEE_SUSPEND_EXTERNAL: Suspended by external event (ex:incoming call)

Comments:
    None

See Also:
    EVT_APP_SUSPEND

=============================================================================
AEESuspendInfo

Description:
    This is sent as swParam along with the EVT_APP_SUSPEND event. This allows
    apps to control whether ot not BREW closes the dialogs belonging to this app
    when the app handles EVT_APP_SUSPEND

Definition:
typedef struct _AEESuspendInfo {
   int      nErr;
   boolean  bCloseDialogs;
} AEESuspendInfo;

Members:
    nErr: App sets if there is some error. This is currently ignored by BREW
    bCloseDialogs: App sets to FALSE to indicate to BREW to avoid closing dialogs.

Comments:
    None

See Also:
    EVT_APP_SUSPEND

=============================================================================
    IDC_COMMAND_RESERVED

Description:
    This is a placeholder to indicate that all command IDs above this are reserved
for internal BREW use

Definition:
    #define IDC_COMMAND_RESERVED (0xff00)

Members:
    None

Comments:
    This is used in conjunction with the EVT_COMMAND sent by controls

See Also:
    EVT_COMMAND

=============================================================================

    PFNCBCANCEL

Description:
    This data structure specifies the prototype of the Cancel Function that can
be used to cancel a callback

Definition:
    typedef void (*PFNCBCANCEL)(AEECallback * pcb);

Members:
    pcb :  Pointer to the AEECallback that must be cancelled using this function

Comments:
    This is used in conjunction with AEECallback

See Also:
    AEECallback

=============================================================================
PFNNOTIFY

Description:
    PFNNOTIFY specifies a data type which is a function pointer to a function type
    void foo(void * pData)..

Definition:
    typedef void (* PFNNOTIFY)(void * pData);

Members:
    None

Comments:
    None

See Also:
    None

=======================================================================

*/

/*
=======================================================================
MACROS DOCUMENTATION
=======================================================================

CALLBACK_Init()

Description:
  Set up internal members of an AEECallback for scheduling, sets the
  notify function and the notify context

Definition:

   CALLBACK_Init(pcb,pcf,pcx) {(pcb)->pfnNotify = (pcf); \
                               (pcb)->pNotifyData = (pcx);}

Parameters:
  AEECallback *pcb: pointer to the AEECallback to set up

  PFNNOTIFY pcf: notify function

  void *pcx: notify context

Evaluation Value:
   None

Comments:
   pcf(pcx) is called when the AEECallback comes due (either via Resume
   or other scheduling operation)

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

=======================================================================

CALLBACK_Cancel()

Description:
  fire an AEECallback's cancel function, if any.

Definition:

   CALLBACK_Cancel(pcb)    if (0 != (pcb)->pfnCancel) (pcb)->pfnCancel(pcb)

Parameters:
  AEECallback *pcb: pointer to the AEECallback to set up

Evaluation Value:
   None

Comments:
   Setting up the cancel function is the responsibility of the called
   scheduling API.  E.g.: the ISHELL_Resume cancel function is likely
   something that removes the callback from a list of pending notifies

   Similarly, clearing the cancel function during a cancel callback or
   before calling the notify function is *also* the responsibility of
   the called scheduling API. E.g.: before ISHELL_Resume fires the
   notify function, the AEECallback is taken out of a list of pending
   notifies, and so the cancel function in the callback must be set to
   0.

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

=======================================================================

CALLBACK_IsQueued()

Description:
   deduce whether an AEECallback is pending, scheduled

Definition:

   CALLBACK_IsQueued(pcb) (0 != (pcb)->pfnCancel)


Parameters:
  AEECallback *pcb: pointer to the AEECallback to set up

Evaluation Value:
   boolean TRUE if the AEECallback has a cancel function, is on
          somebody's scheduling, pending list

Comments:
   This is not an exhaustive test and is only a convenience for called
   scheduling APIs that use the cancel function (should be all of them,
   but callers must not assume so).

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

===========================================================================
AEE_HI()

Description:
   This is a macro that can be used to extract the upper 16 bits of a
   given 32-bit value

Prototype:

uint16 AEE_HI(uint32 dwParm)


Parameters:
   dwParm: 32-bit value whose upper 16 bits needs to be extracted

Return Value:
   Upper 16 bits of the given 32-bit value

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   AEE_LO()

=======================================================================
AEE_LO()

Description:
   This is a macro that can be used to extract the lower 16 bits of a
   given 32-bit value

Prototype:

uint16 AEE_LO(uint32 dwParm)


Parameters:
   dwParm: 32-bit value whose lower 16 bits needs to be extracted

Return Value:
   lower 16 bits of the given 32-bit value

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   AEE_HI()

=======================================================================
AEE_GET_X()

Description:
   This is a macro that can be used to extract the X-Coordinate value from
   the given 32-bit dwParam. This is used in conjunction with the AVK_TAP key code 
   and EVT_PEN events.
   When a TAP click occurs, the app receives the event EVT_KEY with
   wParam set to AVK_TAP and dwParam containing the X and Y coordinates of the location
   of the tap click. The upper 16 bits of the dwParam contains the X coordinate while
   the lower 16 bits contains the Y coordinate of the click. This macro (AEE_GET_X()) can be
   used to get the X coordinate of the event..

Prototype:

int16 AEE_GET_X(uint32 dwParm)


Parameters:
   dwParm: 32-bit value containing the X and Y coordinate of the event

Return Value:
   X coordinate of the event

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   AEE_GET_Y()
=======================================================================
AEE_GET_Y()

Description:
   This is a macro that can be used to extract the Y-Coordinate value from
   the given 32-bit dwParam. This is used in conjunction with the AVK_TAP key code
   and EVT_PEN events.
   When a TAP click occurs, the app receives the event EVT_KEY with
   wParam set to AVK_TAP and dwParam containing the X and Y coordinates of the location
   of the tap click. The upper 16 bits of the dwParam contains the X coordinate while
   the lower 16 bits contains the Y coordinate of the click. This macro (AEE_GET_Y()) can be
   used to get the Y coordinate of the event.

Prototype:

int16 AEE_GET_Y(uint32 dwParm)


Parameters:
   dwParm: 32-bit value containing the X and Y coordinate of the click

Return Value:
   Y coordinate of the event

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   AEE_GET_X()

=======================================================================
AEE_SET_XY()

Description:
   This is a macro that can be used to combine the x and y-coordinate values.
   It shifts the current value of x to the left by 16-bits and then sets the
   value of y into the lower 16 bits of the result.

Prototype:

void AEE_SET_XY(int16 x, int16 y)

Parameters:
   x: 16 bit value that will be set in the upper word of result.
   y: 16 bit value that must be set in the lower-word of result.

Return Value:
   None

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   AEE_GET_X()
   AEE_GET_Y()

=======================================================================
GET_NOTIFIER_MASK()

Description:
   This is a macro that can be used to extract the Notification Mask i.e. the lower
   16 bits of the 32-bit dwMask passed to ISHELL_RegisterNotify()

Prototype:

#define GET_NOTIFIER_MASK(dwv)         (uint16)(dwv & 0x0000ffffl)


Parameters:
   dwv: 32-bit mask passed to ISHELL_RegisterNotify()

Return Value:
   lower 16 bits (also called Notification Mask)

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   ISHELL_RegisterNotify()
   ISHELL_Notify()
   GET_NOTIFIER_VAL()

=======================================================================
GET_NOTIFIER_VAL()

Description:
   This is a macro that can be used to extract the Notification Value i.e. the upper
   16 bits of the 32-bit dwMask passed to ISHELL_RegisterNotify()

Prototype:

#define GET_NOTIFIER_VAL(dwv)          (uint16)((dwv >> 16) & 0x0000ffff)


Parameters:
   dwv: 32-bit mask passed to ISHELL_RegisterNotify()

Return Value:
   Upper 16 bits (also called Notification Value)

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   ISHELL_RegisterNotify()
   ISHELL_Notify()
   GET_NOTIFIER_MASK()
=======================================================================
*/


/*

====================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

IBase Interface

Description:

   IBase is the base level object class from which all API object
   classes are derived.  It supplies the object reference counting
   mechanisms that allow objects to manage their own memory instances.

   The following header file is required:~
   AEE.h

=======================================================================

IBASE_AddRef()

Description:

   This method increments the reference count on an object.  This
   allows the object to be shared by multiple callers.  The object is
   freed when the reference count reaches 0 (zero).

Prototype:

   uint32 IBASE_AddRef(IBase * po);

Parameters:
   po: Pointer to the object

Return Value:

   Returns the incremented reference count for the object.  A valid
   object returns a positive reference count.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   IBASE_Release()
=======================================================================
IBASE_Release()

Description:

   This function decrements the reference count of an object.  The
   object is freed from memory and is no longer valid once its
   reference count reaches 0 (zero).

Prototype:

   uint32 IBASE_Release(IBase * po);

Parameters:

   po: Pointer to the object

Return Value:

   Returns the decremented reference count for the object.  The object
   has been freed and is no longer valid if 0 (zero) is returned.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   IBASE_AddRef()

=======================================================================
 INTERFACES DOCUMENTATION
=======================================================================

IApplet Interface

The IApplet Interface is the interface that handles events in the Application Execution
Environment (AEE). The interface is derived from IBase and implements a simple event
handling routine. This routine is called by the AEE Shell in response to events
generated by the system, other components, or applets. All applets in the BREW AEE
must implement this interface. In other words, a BREW applet is a class that implements
the IApplet Interface. IAPPLET_HandleEvent() is used by the BREW AEE Shell for sending
events to the applet.


NOTE: IAPPLET_HandleEvent() can be called only by the AEE Shell. Events sent by
other applets or components can be sent by ISHELL_SendEvent().

The following header file is required:~
AEE.h
=======================================================================
IAPPLET_AddRef()

Description:
    This function is inherited from IBASE_AddRef().

Version:
   Introduced BREW Client 1.0

See Also:
    IAPPLET_Release()
=======================================================================
IAPPLET_Release()

Description:
    This function is inherited from IBASE_Release().

Version:
   Introduced BREW Client 1.0

See Also:
    IAPPLET_AddRef()
=======================================================================

IAPPLET_HandleEvent()

Description:
    This function provides the main event processing for a BREW applet.
    It is called when any event is passed to the applet. Events can
    include system-level notifications, keypress events, and so on.
    System alarms or system notifications call this function to respond.
    If the applet is not currently running, the applet is loaded and the
    event is sent to the applet. The EVT_APP_STOP event is not sent to the
    applet, and unless the applet starts itself, the applet terminates after
    the completion of the event. The IAPPLET_HandleEvent() function must be
    implemented by all applets. The IAPPLET_HandleEvent() function is also
    used to support applet startup, shutdown, suspend, and resume.

Prototype:
    boolean IAPPLET_HandleEvent
    (
    IApplet * pIApplet,
    AEEEvent evt,
    uint16 wp,
    uint32 dwp
    )

Parameters:
    pIApplet : Pointer to the IApplet Interface object.
    evt : Event code.
    wp : 16-bit event-specific parameter.
    dwp : 32-bit event-specific parameter.

Return Value:
    TRUE, If the event was handled by the applet.

    FALSE, if otherwise.~
    It is very important to return FALSE if the operation did not process
    the event. This allows the AEE to do further processing of the event.
    A typical example occurs when the AVK_CLR key is received. If the application
    did not handle the event, the return of FALSE helps the AEE close the application.

Comments:
    This function can be called only by the AEE Shell.

Version:
   Introduced BREW Client 1.0

See Also:
    ISHELL_SendEvent()
    AEE Events

=======================================================================
 INTERFACES DOCUMENTATION
=======================================================================

IModule Interface

The IModule Interface provides a mechanism for controlling access to a group of
associated applets or components. The module is a single point of entry for the
AEE Shell to request classes owned by the module. In most cases, a module exposes
a single applet or shared-component class. However, the IModule Interface allows
modules to expose a wide variety of classes without fixed entry-points. The module
can also serve as a base object for any associated objects, allowing the modules
to share memory and other resources through the private implementation of the module
class.

The following header file is required:~
AEE.h

======================================================================

IMODULE_AddRef()

Description:
    This function is inherited from IBASE_AddRef().

Version:
   Introduced BREW Client 1.0

See Also:
    IMODULE_Release()

======================================================================

IMODULE_Release()

Description:
    This function is inherited from IBASE_Release().

Version:
   Introduced BREW Client 1.0

See Also:
    IMODULE_AddRef()
=======================================================================

IMODULE_CreateInstance()

Description:
    This function provides the mechanism for the AEE to request classes
    on an as-needed basis from the module. After successfully creating the
    requested object class, the module returns the class object with a
    positive reference count.

    NOTE: The requested class must be implemented according to the class
    definition specified for the class.

Prototype:
    int IMODULE_CreateInstance
    (
    IModule * pIModule,
    IShell * pIShell,
    AEECLSID ClsId,
    void * * ppObj
    )

Parameters:
    pIModule :  [in] :  Pointer to theIModule Interface object.
    pIShell :  [in] :  Pointer to the IShell Interface object.
    ClsId :  [in] :  Requested ClassID exposed by the module.
    ppObj :  [out] :  Returned object. Filled by the IMODULE_CreateInstance()
function.

Return Value:
    SUCCESS, object class was created.

    ENOMEMORY, insufficient memory.

    EBADCLASS, requested class is unsupported.

Comments:
    The object must be returned with a positive reference count.

Version:
   Introduced BREW Client 1.0

See Also:
    AEECLSID List

=======================================================================
IMODULE_FreeResources()

Description:
    This function is called by the AEE Shell when the shell or device
    detects a low memory or storage condition. This can include low RAM
    or Flash/File storage. The module is passed a pointer to IHeap Interface
    or IFileMgr Interface, depending on the specific condition involved.
    The module releases any unused RAM or File storage under this condition.

Prototype:
    void IMODULE_FreeResources
    (
    IModule * pIModule,
    IHeap * ph,
    IFileMgr * pfm
    )

Parameters:
    pIModule : Pointer to the IModule Interface object.
    ph : Pointer to the IHeap Interface object.
    pfm : Pointer to the IFileMgr Interface object.

Return Value:
    None

Comments:
    None

Version:
   Introduced BREW Client 1.0

See Also:
    None

=======================================================================
 INTERFACES DOCUMENTATION
=======================================================================

IControl Interface

   The IControl Interface is an abstract interface that is implemented by each of the
BREW control interfaces (IDateCtl Interface, IMenuCtl Interface, IStatic Interface,
ITextCtl Interface and ITimeCtl Interface). Because the interface is abstract, it
is not possible to create an instance of the IControl Interface directly. Given
a pointer to an instance of a control interface, invoke an IControl Interface function
using either an IControl function or a function in that control's interface. For
example, a menu control can be redrawn by calling either ICONTROL_Redraw() or IMENUCTL_Redraw().
The IControl functions are useful if you want to perform the same operation on many
different types of control. For example, suppose that the array pControls[numControls]
contains pointers to controls of different types that must be displayed together
on the screen. The following loop can be used to redraw all the controls:

===pre>
for (i = 0; i < numControls; i++)
ICONTROL_Redraw(pControls[i])
===/pre>
===p>
Each BREW control implements all of the functions in the IControl Interface. However,
the behavior of each function may be different in each interface; refer to the function
descriptions of each control interface for details. Each BREW control also implements
control-specific functions that are not part of the IControl Interface.
===H2>
The IControl Interface functions include:
===/H2>
===p>
 -    ICONTROL_HandleEvent() passes events to a control. The BREW controls process
various key events to allow a device user to enter a text, time, or date value or
choose an item from a menu. Refer to the descriptions of each control for the events
it handles. A control can receive events only when it is active. Your applet must
pass a control any events it handles when it is active, unless the control is part
of a dialog.

 -    ICONTROL_Redraw() draws the control on the screen. This function can be used
to re-display a control after it has been overwritten.

 -    ICONTROL_SetActive() sets the activity state (active or inactive) of the control.
Only an active control can receive and process events. In the case of a multi control
dialog, only the control that currently has the focus is active.

 -    ICONTROL_IsActive() retrieves the activity state of a control.

 -    ICONTROL_SetRect() sets the pixel dimensions of the screen rectangle in which
the control appears. The pixel dimensions are specified in an AEERect structure
that is passed as a parameter to the function. Some controls provide a scrolling
mechanism that is used when the contents of the control do not fit in the rectangle.
You can display multiple controls on the screen at the same time by setting their
rectangle sizes appropriately.

 -    ICONTROL_GetRect() retrieves the current rectangle of a control.

 -    ICONTROL_SetProperties() allows you to change a control's property values,
which are used to customize the control's behavior and appearance by enabling some
optional features. Each control can have up to 32 properties, with each represented
by a bit in a 32-bit variable. ICONTROL_SetProperties() allows you to set the value
of this variable. These properties are all unset initially, and a property is set
by turning its bit on. Each BREW control has a different set of properties; refer
to the descriptions of each control for a list of the properties it supports. The
header file for each BREW control has a set of bit-mask constants that can be used
to test and set the values of the bits corresponding to each property.

 -    ICONTROL_GetProperties() retrieves the current value of the control's property
value. To set a property without changing the values of the other properties, do
the following:
===pre>
   dwProps = ICONTROL_GetProperties(pIControl);
ICONTROL_SetProperties(pIControl,(dwProps | PROP_BITMASK));
===/pre>

 -    where pIControl is a pointer to the control and PROP_BITMASK is a bit-mask
for the property to be set.

 -    ICONTROL_Reset() frees all the resources associated with the control, but
does not release its interface pointer. This function can be used to reinitialize
a control.

The following header file is required:~
AEE.h

=============================================================================
ICONTROL_AddRef()
This function is inherited from IBASE_AddRef().
=============================================================================
ICONTROL_Release()
This function is inherited from IBASE_Release().
=============================================================================

ICONTROL_GetProperties()

Description:
   This function returns the control-specific properties or flags.

Prototype:
   uint32 ICONTROL_GetProperties(IControl * pIControl)

Parameters:
   pIControl : Pointer to the IControl Interface object.

Return Value:
   32-bit properties for the control.

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   ICONTROL_SetProperties()

=============================================================================

ICONTROL_GetRect()

Description:
   This function fills a pointer to an input AEERect structure with the active screen
coordinates for the control. This is particularly useful after a control is created
to determine its optimal/default size and position.

Prototype:
   void ICONTROL_GetRect(IControl * pIControl, AEERect * prc)

Parameters:
   pIControl : Pointer to the IControl Interface object.
   prc : Rectangle to be filled with the coordinates of the control.

Return Value:
   None

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   AEERect
   ICONTROL_SetRect()

=============================================================================

ICONTROL_HandleEvent()

Description:
   This function provides the main event processing for a control. It is called when
any event is passed to the control. Events mainly include keypress events. This
function must be implemented by all controls.


Prototype:
   boolean ICONTROL_HandleEvent(IControl * pIControl, AEEEvent evt, uint16 wp, uint32 dwp)

Parameters:
   pIControl : Pointer to the IControl Interface object.
   evt :  Event code.
   wp : 16-bit event data.
   dwp : 32-bit event data.

Return Value:
   TRUE, if the event was processed by the control.

   FALSE, if otherwise.

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

=============================================================================

ICONTROL_IsActive()

Description:
   This function returns the active or focus state of the control.

Prototype:
   boolean ICONTROL_IsActive(IControl * pIControl)

Parameters:
   pIControl : Pointer to the IControl Interface object.

Return Value:
   TRUE, if the control is active.

   FALSE, if otherwise.

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

=============================================================================

ICONTROL_Redraw()

Description:
   This function instructs the control to redraw its contents. Under normal conditions,
user-interface controls do not redraw their contents when the underlying data behind
the control changes. This allows several data updates to occur while minimizing
screen flashes. For example, several items can be added to a menu with no visible
effect until the Redraw function is called.

Prototype:
   boolean ICONTROL_Redraw(IControl * pIControl)

Parameters:
   pIControl : Pointer to the IControl Interface object.

Return Value:
   TRUE, if the event was processed by the control.

   FALSE, if otherwise.

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

=============================================================================

ICONTROL_Reset()

Description:
   This function instructs the control to reset (free/delete) its contents and to immediately
leave active/focus mode. This is useful in freeing all underlying memory in text
or menu controls or removing all menu items in a single call.

Prototype:
   void ICONTROL_Reset(IControl * pIControl)

Parameters:
   pIControl : Pointer to the IControl Interface object.

Return Value:
   None

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   ICONTROL_SetActive()

=============================================================================

ICONTROL_SetActive()

Description:
   This function instructs the control to enter/leave focus or selected mode. The concept
of focus is left somewhat to the control. In the case of menus, focus indicates
that the menu is active. In terms of text controls it means the control is active
and in edit mode. This call usually results in the underlying control redrawing
its contents.

NOTE: Controls still have their event handling function called even when they are
inactive, allowing them to process special events such as scrolling multiline text
controls.


Prototype:
   void ICONTROL_SetActive(IControl * pIControl,boolean bActive)

Parameters:
   pIControl : Pointer to the IControl Interface object.
   bActive : Specifies whether to activate (TRUE) or deactivate (FALSE) the control.

Return Value:
   None

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

=============================================================================

ICONTROL_SetProperties()

Description:
   This function sets control-specific properties or flags. Although some properties
are defined across controls (CP_FRAME, and so forth), most controls expose a range
of properties to allow more specific control over the object.

Prototype:
   void ICONTROL_SetProperties(IControl * pIControl, uint32 dwProps)

Parameters:
   pIControl: Pointer to the IControl Interface object.
   dwProps : 32-bit set of flags/properties.

Return Value:
   None

Comments:
   None

Side Effects:
   May redraw the control.

Version:
   Introduced BREW Client 1.0

See Also:
   ICONTROL_GetProperties()

=============================================================================

ICONTROL_SetRect()

Description:
   This function sets the active screen coordinates of the control. This may result
in the control redrawing its contents.

Prototype:
   void ICONTROL_SetRect(IControl * pIControl, const AEERect * prc)

Parameters:
   pIControl : Pointer to the IControl Interface object.
   prc : The bounding rectangle for the control.

Return Value:
   None

Comments:
   None

Side Effects:
   May redraw the control.

Version:
   Introduced BREW Client 1.0

See Also:
   AEERect
   ICONTROL_GetRect()

=======================================================================
 INTERFACES DOCUMENTATION
=======================================================================

INotifier Interface

Description:
   The AEE supports the concept of notifications. Notification is a mechanism where-in
   different classes can register for events when certain incidents occur in other classes.
   For example, an applet can regsiter to be notified when say, there is an incoming SMS
   message. This means, whenever there is an incoming SMS message, the applet that has
   registered to be notified will receive an event. This event is EVT_NOTIFY.

   Notifiers are the classes that generate notifications. For ex: the TAPI class acts as a
   notifier and it sends out notifications when there is an incoming SMS message.
   Notifiers are classes that want to send out notifications when certain incidents
   occur. Whenever a notifier needs to send out notifications, it uses the ISHELL_Notify()
   method.

   Applets can register for notifications i.e. they can register to be notified whenever
   certain incidents occur in the notifiers. To do this, the function ISHELL_RegisterNotify()
   must be used. For example, an applet can use this mechanism to register for
   notification from the AEECLSID_TAPI interface for events specific to that class.

   INotifier is the interface that specifies the functions that must be implemented by
   any class that needs to be a notifier. Any class that wants to send out
   notifications so that other applets can receive it must implement the functions
   in the INotifier interface.

   Here is a broad description of the steps that must be followed by a class
   if it needs to be a notifier:~
   1. The class must implement the INotifier interface.
   2. Define the set of notifications (or masks) that the class can issue. For ex: some
   of the notifications that the the TAPI class can issue are: NMASK_TAPI_SMS,
   NMASK_TAPI_TS_PAGE and NMASK_TAPI_TS_VMAIL. These masks must be made available to
   other applets so the applets that are interested in these notifications can register
   for them.
   3. Whenever the notifier class wants to issue a notification, it must invoke
   the ISHELL_Notify() function. The shell will then take care of informing all the
   applets that have registered for this notification.

   In addition to the standard IBase methods (AddRef() and Release()), the INotifier interface
   has the following additional method(s):

=============================================================================
INOTIFIER_AddRef()
This function is inherited from IBASE_AddRef().
=======================================================================
INOTIFIER_Release()
This function is inherited from IBASE_Release().
=======================================================================

INOTIFIER_SetMask()

Description:
   This method is invoked by the Shell to inform the notifier class of
   all the notifications (issued by that class) that other applets are
   interested in.
   This function must be implemented by all notifiers (i.e. by all classes that
   implement the INotifier interface). This function is strictly meant to be
   invoked internally by the Shell. It must never be directly invoked by other
   applets or classes.


Prototype:

   void  INOTIFIER_SetMask(INotifier * po, const uint32 * dwMasks);

Parameters:
   po: Pointer to the INotifier interface

   dwMasks: Specifies the array of masks representing the notifications that
   other applets are interested in.
   A class may be capable  of emitting multiple notifications  (say,
   NMASK_TAPI_SMS, NMASK_TAPI_TS_PAGE and NMASK_TAPI_TS_VMAIL).However, the applets
   that have registered for notifications may only be interested in the
   NMASK_TAPI_SMS and NMASK_TAPI_TS_PAGE notifications. Whenever an application registers
   for a notification using ISHELL_RegisterNotify(), it must specify the mask for
   the notification that it is interested in. The shell then invokes this function
   INOTIFIER_SetMask() on the notifier to inform the notifier of all the notifications
   that the applets are interested in.


Return Value:
   None

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   ISHELL_RegisterNotify(),
   ISHELL_Notify()

=======================================================================
 INTERFACES DOCUMENTATION
=======================================================================

IAStream Interface

The IAStream Interface reads data from an asynchronous stream. It is an abstract
interface implemented by classes providing access to data that may not all be available
at once and must be retrieved asynchronously. At present, the IFile and ISocket
classes implement the IAStream Interface. The IImage Interface and ISoundPlayer
Interface implement IIMAGE_SetStream() and ISOUNDPLAYER_SetStream() functions that
allow an IAStream Interface to be supplied as the source of image or sound input.

IASTREAM_Read() reads data from the stream and returns the number of bytes read.
If no data is available for reading, this function returns the value AEE_STREAM_WOULDBLOCK.
In the latter case, call IASTREAM_Readable() to schedule a callback function that
is invoked when there is more data available. IASTREAM_Cancel() cancels a callback
that was scheduled with IASTREAM_Readable().
===H2>
To use an IAStream instance to retrieve data asynchronously
===/H2>
===p>
1.   Call ISHELL_CreateInstance() to create an instance of a class that implements
the IAStream interface.

2.   Call IASTREAM_Read() to read the required number of bytes of data from the
stream. Because IAStream Interface is an abstract interface, call IFILE_Read() for
files, or ISOCKET_Read() for sockets.

3.   If IASTREAM_Read() returns AEE_STREAM_WOULDBLOCK, call IASTREAM_Readable()
to schedule a callback to try again later.

4.   If IASTREAM_Read() reads fewer than the required number of bytes, call the
function again to read the remaining data.

5.   Repeat steps 2-4 until all the data has been received on the stream.
===H2>
To use an asynchronous stream as the source of image or sound data
===/H2>
===p>
1.   Call ISHELL_CreateInstance() to create an instance of a class that implements
the IAStream Interface.

2.   Call ISHELL_CreateInstance() to create an instance of IImage Interface or ISoundPlayer
Interface.

3.   Call IIMAGE_Notify() or ISOUNDPLAYER_RegisterNotify() to schedule a callback
that is invoked when the image or sound data has been completely retrieved.

4.   Call IIMAGE_SetStream() or ISOUNDPLAYER_SetStream() to associate the stream
created in step 1 with the IImage or ISoundPlayer instance created in step 2. This
initiates the retrieval of image or sound data on the stream.

5.   When retrieval is complete, the callback registered in step 3 is invoked. Use
functions in the IImage Interface or the ISoundPlayer Interface to access the retrieved
image or sound data.

The following header file is required:~
AEE.h

=============================================================================
IASTREAM_AddRef()
This function is inherited from IBASE_AddRef().
=======================================================================
IASTREAM_Release()
This function is inherited from IBASE_Release().
=======================================================================
IASTREAM_Cancel()

Description:
    This function cancels a callback that was scheduled with IASTREAM_Readable().

Prototype:
    void IASTREAM_Cancel(IAStream * pIAStream)

Parameters:
    pIAStream :  Pointer to the IAStream Interface object.

Return Value:
    None

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   PFNNOTIFY
    IASTREAM_Read()
    IASTREAM_Readable()

=======================================================================
IASTREAM_Read()

Description:
    This function attempts to read data from a stream, and returns the number of
bytes read. If no data is available for reading, it returns the value AEE_STREAM_WOULDBLOCK.

Prototype:
    int32 IASTREAM_Read
    (
    IAStream * pIAStream,
    void * pBuffer,
    uint32 dwCount
    )

Parameters:
    pIAStream :  Pointer to the IAStream Interface object.
    pBuffer :  Pointer to the buffer into which the data is read.
    dwCount :  Number of bytes to read.

Return Value:
    Number of bytes read, if data is available for reading.

    0 (zero), if all available data has been read.

    AEE_STREAM_WOULDBLOCK, if no data is currently available.

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
    IASTREAM_Readable()
    IASTREAM_Cancel()

=======================================================================
IASTREAM_Readable()

Description:
    This function registers a callback that checks whether data is available to
be read. It is called when the IASTREAM_Read() returns AEE_STREAM_WOULDBLOCK.

Prototype:
    void IASTREAM_Readable
    (
    IAStream * pIAStream,
    PFNNOTIFY pfn,
    void * pUser
    )

Parameters:
    pIAStream :  Pointer to the IAStream Interface object.
    pfn :  Pointer to the callback function.
    pUser :  Pointer to user-specified data that is passed as a parameter to the
callback function.

Return Value:
   None

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
    PFNNOTIFY
    IASTREAM_Read()
    IASTREAM_Cancel()

=======================================================================
 INTERFACES DOCUMENTATION
=======================================================================

IMemAStream Interface

The IMemAStream interface extends the IAStream Interface to allow a specified memory
chunk to be read as a stream. An instance of the IMemAStream can be created using
ISHELL_CreateInstance() with ClassID AEECLSID_MEMASTREAM. The specified memory chunk
is freed when the IMemAStream Interface object is released.

In addition to the standard IBase Interface functions, AddRef() and Release(), and
the standard IAStream Interface functions, Readable(), Read(), and Cancel(), the
IMemAStream Interface includes the Set() function.

The following header file is required:~
AEE.h

=======================================================================
IMEMASTREAM_AddRef()
This function is inherited from IBASE_AddRef().
=======================================================================
IMEMASTREAM_Release()
This function is inherited from IBASE_Release().
=======================================================================
IMEMASTREAM_Cancel()
This function is inherited from IASTREAM_Cancel().
=======================================================================
IMEMASTREAM_Read()
This function is inherited from IASTREAM_Read().
=======================================================================
IMEMASTREAM_Readable()
This function is inherited from IASTREAM_Readable()
=======================================================================
IMEMASTREAM_Set()

Description:
    This function allows you to set the memory chunk that needs to be read as a
stream. An instance of the IMemAStream Interface object must already exist.
    The responsibility of freeing the buffer pBuff lies with the IMemAStream Interface.
Do not free this buffer. It is freed when either of the following two actions occur:
     -    The IMemAStream Interface object is released using IMEMASTREAM_Release().
     -    A subsequent call to IMEMASTREAM_Set() is attempted with another buffer.

If a buffer has already been set into the stream using a previous call to this
function, that buffer is freed before setting the new buffer.
===H2>
NOTE:
===/H2>
===p>
It is dangerous to attempt two consecutive calls to IMEMASTREAM_Set()
   with the same buffer.
===/p>
Prototype:
    void IMEMASTREAM_Set
    (
                             IMemAStream * pIMemAStream,
                             byte * pBuff,
                             uint32 dwSize,
                             uint32 dwOffset,
                             boolean bSysMem
                           )

Parameters:
    pIMemAStream :  Pointer to the IMemAStream Interface object.
    pBuff  :  Pointer to the memory chunk that needs to be read as a stream.
    dwSize :  Size of the memory chunk.
    dwOffset  :  Offset from the beginning of the memory chunk to be set as the
start data for the memory stream.
    bSysMem  :  Flag to specify if the memory for the buffer pBuff belongs to the
user area or the system memory. This flag indicates whether the routines FREE()
or SYSFREE() must be used by the IMemAStream Interface object to free the buffer
when this object is released or when another Set() is made. For example, if you
perform a MALLOC() to allocate the buffer, bSysMem must be set to FALSE.

Return Value:
   None

Comments:
   None

Version:
   Introduced BREW Client 1.0

See Also:
    IMEMASTREAM_Release()

=======================================================================

IMEMASTREAM_SetEx()

Description:
    This function behaves the same as IMEMASTREAM_Set(), with the additional feature
of a User callback to free the memory chunk. You can specify a callback function
pUserFreeFn, which is called when the memory chunk pBuff needs to be freed.

Prototype:
    void IMEMASTREAM_SetEx
    (
                             IMemAStream * pIMemAStream,
                             byte *        pBuff,
                             uint32        dwSize,
                             uint32        dwOffset,
                             PFNNOTIFY     pUserFreeFn,
                             void *        pUserFreeData
                           )

Parameters:
    pIMemAStream  :  Pointer to the IMemAStream Interface.
    pBuff  :  Pointer to the memory chunk that needs to be read as a stream.
    dwSize  :  Size of the memory chunk.
    dwOffset  :  Offset from the beginning of the memory chunk to be set as the
start data for the memory stream.
    pUserFreeFn  :  Callback function to be called when the memory chunk needs to
be freed.
    pUserFreeData  :  Parameter to be passed to pUserFreeFn.

Return Value:
   None

Comments:
   None

Version:
   Introduced BREW Client 1.1

See Also:
    PFNNOTIFY
    IMEMASTREAM_Set()

=======================================================================

*/



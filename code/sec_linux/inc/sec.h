/*==================================================================================================
    HEADER NAME : sec.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
    
    
    GENERAL DESCRIPTION
        In this file,define the  common data structure and event 
        used in the three interfaces.This is a public head file of sec module.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-07   Kevin Yang         None         Draft
==================================================================================================*/

/***************************************************************************************************
*   Multi-Include-Prevent Section
***************************************************************************************************/

#ifndef _SEC_H_
#define _SEC_H_


/***********************************************************************
*   macro Define Section
************************************************************************/
typedef struct _ISEC ISEC; 


/************************************************************************
*   Common Dialog Event                                                                                         *
************************************************************************/
/*added by Bird 061122 for wtls&signtext*/
#define G_Evt_SHOW_DLG        0x40 

/************************************************************************
* The result value from response
************************************************************************/
#define G_SEC_OK                            0x00                       
#define G_SEC_GENERAL_ERROR                 0x01            
#define G_SEC_UNSUPPORTED_METHOD            0x02       
#define G_SEC_NOTENOUGH_MEMORY              0x03      
#define G_SEC_NOT_SUPPORT                   0x04          
#define G_SEC_INVALID_PARAMETER             0x05        
#define G_SEC_DATA_LENGTH                   0x06              
#define G_SEC_INVALID_KEY                   0x07              
#define G_SEC_MISSING_KEY                   0x08
#define G_SEC_UNKNOWN_CERTTYPE              0x09
#define G_SEC_NOMATCH_ROOTCERT              0x0a
#define G_SEC_INVALIDATE_CERT               0x0b
#define G_SEC_CERTEXPIRED                   0x0c
#define G_SEC_NOTFOUND_CERT                 0x0d
#define G_SEC_USER_NOTVERIFIED              0x0e
#define G_SEC_CANCEL                        0x10

#define G_SEC_INVALID_SOCKET                0x11
#define G_SEC_BAD_HANDLE                    0x12
#define G_SEC_ERROR_OTHER                   0x13


/*******************************************************************************
*   Structure Define Section
*******************************************************************************/
typedef struct tagSt_ShowDlg
{
    WE_INT32 iTargetId;
    WE_VOID* pvBuf;
    WE_UINT32 uiLength;
}St_ShowDlg;

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
typedef void (*Fn_ISecEventHandle)(WE_INT32 iEvent, void* pvSecEventData,
                                   WE_HANDLE hPrivData);




WE_UINT32 ISec_Create(WE_INT32 iType, ISEC **ppISec);
WE_UINT32 ISec_SendToSecD
(
    ISEC *pISec,
    WE_VOID *pvMsg
);
WE_UINT32 ISec_RecvFromSecD
(
    ISEC *pISec,
    WE_CHAR *pcBuffer
);
WE_UINT32 ISec_ExcuteCallBack(ISEC *pISec);
WE_INT32 ISec_GetSocketFd(ISEC *pISec);
WE_UINT32 ISec_Release(ISEC *pISec);
WE_UINT32 ISec_RegClientEventCB
(
    ISEC *pISec,
    WE_HANDLE hPrivData,
    Fn_ISecEventHandle pcbSecEvtFunc
);
WE_UINT32 ISec_DeRegClientEventCB
(
    ISEC *pISec,
    WE_HANDLE hPrivData,
    Fn_ISecEventHandle pcbSecEvtFunc
);


#endif


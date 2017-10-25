
/*=====================================================================================
    FILE NAME : sec_evtdcvt.h
    MODULE NAME : SEC
    
    GENERAL DESCRIPTION
        
    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2002-2006 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
    
    Modification Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-11-24 Bird        none      Init
    
=====================================================================================*/

#ifndef _SEC_EVTDCVT_H
#define _SEC_EVTDCVT_H

/*evt action*/
WE_INT32 
Sec_EncodeEvtAction(WE_INT32 iEvent,WE_VOID* pvSrcData,WE_VOID** ppvDesData,WE_UINT32* puiLen );
WE_INT32 
Sec_DecodeEvtAction(WE_VOID* pvSrcData,WE_UINT32 uiLen,WE_INT32 *piEvent,WE_VOID** pvDesData);

/*evt*/
WE_INT32 
Sec_DecodeEvt(WE_VOID* pvSrcData,WE_UINT32 uiLen,WE_INT32 *piEvent,WE_VOID** pvDesData);
WE_INT32 
Sec_EncodeEvt(WE_INT32 iEvent,WE_VOID* pvSrcData,WE_VOID** ppvDesData,WE_UINT32* puiLen );




#endif

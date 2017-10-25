/*=====================================================================================
    FILE NAME :
        Sec_ueh.h
    MODULE NAME :
        sec
    GENERAL DESCRIPTION
        
    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2002-2006 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
    Modification              Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-07-20 Bird           none      Init
    
=====================================================================================*/
/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/

#ifndef SEC_UEH_H
#define SEC_UEH_H

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
/*1.confirm*/
#define M_UE_CONFIRM_EVENT                                 G_Evt_CONFIRM 
/*2.change pin*/
#define M_UE_CHANGE_PIN_EVENT                              G_Evt_CHANGEPIN 
/*3.choose certificate by name*/
#define M_UE_SELECT_CET_EVENT                              G_Evt_SELECTCERT  
/*4.confirm display name*/
#define M_UE_NAME_CONFIRM_EVENT                            G_Evt_NAMECONFIRM 
/*5.sign text confirm*/
#define M_UE_SIGNTEXT_CONFIRM_EVENT                        G_Evt_SIGNTEXTCONFIRM  
/*6.generate pin*/
#define M_UE_CREATE_PIN_EVENT                              G_Evt_CREATEPIN  
/*7.hash*/
#define M_UE_HASH_EVENT                                    G_Evt_HASH 
/*8.pin*/ 
#define M_UE_PIN_EVENT                                     G_Evt_PIN 
/*9.show certificate*/
#define M_UE_SHOW_CERT_CONTENT_EVENT                       G_Evt_SHOWCERTCONTENT  
/*10.show certificate name*/
#define M_UE_SHOW_CERT_LIST_EVENT                          G_Evt_SHOWCERTLIST  
/*11.show session info*/
#define M_UE_SHOW_SESSION_CONTENT_EVENT                    G_Evt_SHOWSESSIONCONTENT  
/*12.show stored contract*/
#define M_UE_SHOW_CONTRACT_LIST_EVENT                      G_Evt_SHOWCONTRACTLIST  
/*13.store certificate*/
#define M_UE_STORE_CERT_EVENT                              G_Evt_STORECERT 
/*14.warning*/
#define M_UE_WARNING_EVENT                                 G_Evt_WARNING  
/*15.show stored contracts*/
#define M_UE_STORE_CONTRACT_EVENT                          G_Evt_SHOWCONTRACTCONTENT  
#define M_UE_CURSVRCERT                                           G_Evt_SHOWCURSVRCERT

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/

/*1.confirm*/
WE_INT32 Sec_UeConfirmAction(WE_HANDLE hSecData ,WE_INT32 iTargetId,St_ConfirmAction stConfirm);

#ifdef M_SEC_CFG_USE_KEYS
/*2.change pin*/

WE_INT32 Sec_UeChangePinAction(WE_HANDLE hSecData ,WE_INT32 iTargetId,St_ChangePinAction stChangePin);

#endif

#ifdef M_SEC_CFG_USE_KEYS  
/*3.choose certificate by name*/
WE_INT32 Sec_UeSelectAction(WE_HANDLE hSecData ,WE_INT32 iTargetId,St_SelectCertAction stChooseCerByName);
#endif

/*4.confirm display name*/
WE_INT32 Sec_UeNameConfirmAction(WE_HANDLE hSecData ,WE_INT32 iTargetId,St_NameConfirmAction stConfName);

/*5.sign text confirm*/
WE_INT32 Sec_UeSignTextConfirmAction(WE_HANDLE hSecData,WE_INT32 iTargetId,St_SignTextConfirmAction stExtConfirm);

#ifdef M_SEC_CFG_USE_KEYS
/*6.generate pin*/
WE_INT32 Sec_UeCreatePinAction(WE_HANDLE hSecData ,WE_INT32 iTargetId,St_CreatePinAction stGenPin);
#endif

#ifdef M_SEC_CFG_USE_CERT
/*7.hash*/
WE_INT32 Sec_UeHashAction(WE_HANDLE hSecData,WE_INT32 iTargetId,St_HashAction stHash);
#endif

#ifdef M_SEC_CFG_USE_KEYS
/*8.pin*/
WE_INT32 Sec_UePinAction(WE_HANDLE hSecData,WE_INT32 iTargetId,St_PinAction stPin);
#endif

#ifdef M_SEC_CFG_USE_CERT
/*9.show certificate*/
WE_INT32 Sec_UeShowCertContentAction(WE_HANDLE hSecData,WE_INT32 iTargetId,St_ShowCertContentAction stShowCer );

/*10.show certificate name*/
WE_INT32 Sec_UeShowCerListAction(WE_HANDLE hSecData,WE_INT32 iTargetId,St_ShowCertListAction stShowCertName);
#endif
/*11.show session info*/
/*None*/

/*12.show stored contract*/
WE_INT32 Sec_UeShowContractsListAction(WE_HANDLE hSecData,WE_INT32 iTargetId,
                                       St_ShowContractsListAction stStoredContracts);

#ifdef M_SEC_CFG_USE_CERT
/*13.store certificate*/
WE_INT32 Sec_UeStoreCertAction(WE_HANDLE hSecData,WE_INT32 iTargetId,St_StoreCertAction stStoreCert);
#endif
/*14.warning*/
/*None*/

/*15.show stored contracts*/
WE_INT32 Sec_UeShowContractContentAction(WE_HANDLE hSecData,WE_INT32 iTargetId,St_ShowContractContentAction stShowContract);


#endif /*SEC_UEH_H*/

/*==================================================================================================
    FILE NAME:
        sec_icmsg.h
    MODULE NAME:
        secic(internal communication)
    GENERAL DESCRIPTION
        This file define ic msg.

    Techfaith Software Confidential Proprietary.
    (c) Copyright 2006 by Techfaith Software. All Rights Reserved.
====================================================================================================
    Revision History

    Modification Tracking
    Date        Author        Number       Description of changes
    --------------------------------------------------------------------------------
    2007-03-14  Wangshuhua    None         Initialize   
==================================================================================================*/


#ifndef SEC_IC_MSG_H
#define SEC_IC_MSG_H

/*C--->S*/
#define M_SEC_CONN_TYPE                 1
#define M_SEC_W_GET_CIPHER_SUITE        3 
#define M_SEC_W_SETUP_CONN              5
#define M_SEC_W_STOP_CONN               7
#define M_SEC_W_REMOVE_PEER             9        
#define M_SEC_W_ATTACH_PEER_TO_SESSION  11  
#define M_SEC_W_SEARCH_PEER             13
#define M_SEC_W_ENABLE_SESSION          15  
#define M_SEC_W_DISABLE_SESSION         17
#define M_SEC_W_SESSION_GET             19
#define M_SEC_W_SESSION_RENEW           21
#define M_SEC_W_GET_PRF_RESULT          23
#define M_SEC_W_VERIFY_SVR_CERT_CHAIN   25
#define M_SEC_W_KEY_EXCHANGE            27
#define M_SEC_W_GET_USER_CERT           29
#define M_SEC_W_COMP_SIGN               31
#define M_SEC_W_SSL_HASH_UPDATE         33
#define M_SEC_W_EVT_SHOW_DLG_ACTION     35

#define M_SEC_B_GET_CERT_NAME_LIST      37
#define M_SEC_B_GEN_KEY_PAIR            39
#define M_SEC_B_PIN_MODIFY              41
#define M_SEC_B_GET_CUR_SVR_CERT        43
#define M_SEC_B_GET_SESSION_INFO        45  
#define M_SEC_B_GET_WTLS_CUR_CLASS      47
#define M_SEC_B_GET_CONTRACT_LIST       49
#define M_SEC_B_TRANSFER_CERT           51
#define M_SEC_B_GET_REQUEST_USER_CERT   53
#define M_SEC_B_EVT_CHANGE_PIN_ACTION   55
#define M_SEC_B_EVT_CREATE_PIN_ACTION   57
#define M_SEC_B_EVT_HASH_ACTION         59
#define M_SEC_B_EVT_SHOW_CERT_CONTENT_ACTION    61  
#define M_SEC_B_EVT_SHOW_CERT_LIST_ACTION       63
#define M_SEC_B_EVT_SHOW_CONTRACTS_LIST_ACTION  65  
#define M_SEC_B_EVT_STORE_CERT_ACTION           67
#define M_SEC_B_EVT_SHOW_CONTRACT_CONTENT_ACTION    69
#define M_SEC_B_EVT_CONFIRM_ACTION                  71
#define M_SEC_B_EVT_NAME_CONFIRM_ACTION             73  
#define M_SEC_B_EVT_PIN_ACTION                      75


#define M_SEC_S_HANDLE                              77
#define M_SEC_S_EVT_SELECT_CERT_ACTION              79
#define M_SEC_S_EVT_PIN_ACTION                      81

#endif

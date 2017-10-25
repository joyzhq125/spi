/*==================================================================================================
    HEADER NAME : sec_cfg.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
    
    GENERAL DESCRIPTION
        In this file,define the macro used in the sec module, and these macros can control
        the features of sec module.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-06-29   Kevin Yang        None         Init
    
==================================================================================================*/

/***************************************************************************************************
*   multi-Include-Prevent Section
***************************************************************************************************/
#ifndef SEC_CFG_H
#define SEC_CFG_H

/***************************************************************************************************
*   macro Define Section
***************************************************************************************************/
/*************************************************
*  Global Macro
**************************************************/
/*************************************************************************
The following macro is used to control the common features of the sec 
packet,and can be modified based on requirements.
*************************************************************************/
/*#define M_SEC_CFG_WTLS_CLASS_2*/
#define M_SEC_CFG_WTLS_CLASS_3
#define M_SEC_CFG_TLS    /*add by tang1219*/
 

/*************************************************************************
The following macro is used to control the sign text features which can
provide sign text function for wmlscript crypto.
*************************************************************************/
#define M_SEC_CFG_CAN_SIGN_TEXT

/*************************************************************************
The following macro is used to control whether the contract can be
stored in the media disk.
*************************************************************************/
#define M_SEC_CFG_STORE_CONTRACTS

/*************************************************************************
Define whether to send warnings event to the application.
*************************************************************************/
/*#define M_SEC_CFG_SHOW_WARNINGS*/

/*************************************************************************
Define  whether to send message to wtls& signtext  
*************************************************************************/
#define M_SEC_WTLS_NO_CONFIRM_UE

/*Define whether to send pin event to the application*/
/*#define G_SEC_CFG_SHOW_PIN*/


/*************************************************************************
Define the max number of user certificates. It also include the number of
cert that is used to save public key.
*************************************************************************/
#define M_SEC_USER_CERT_MAX_SIZE    10

/*************************************************************************
Define the number of sessions&peer that can be saved data to.
*************************************************************************/
#define M_SEC_SESSION_CACHE_SIZE    10
#define M_SEC_PEER_CACHE_SIZE       10
/*************************************************************************
The max&min length of the PIN code.
*************************************************************************/
#define M_SEC_PIN_MAX_SIZE          20
#define M_SEC_PIN_MIN_SIZE          4

/*************************************************************************
Define some algorithm used in sec packet
*************************************************************************/
#define M_SEC_CFG_RC5_CBC
#define M_SEC_CFG_3DES_CBC_EDE
#define M_SEC_CFG_MD2
#define M_SEC_CFG_3DES_CBC_EDE_INTERN
#ifdef M_SEC_CFG_TLS
#define M_SEC_CFG_RC4
#define M_SEC_CFG_RC2 
#define M_SEC_CFG_DES40
#define Test_M_SEC_CFG_3DES_CBC_EDE
#endif 

/*************************************************
*  Module Macro
**************************************************/
/*************************************************************************
Define the name of the wtls authentication cert, the name length, and 
character set. In this cert, only pk is included.
*************************************************************************/
#define M_SEC_PUB_KEY_CERT_NAME_AUT "BREW PK Authentication Cert"
#define M_SEC_PUB_KEY_CERT_NAME_AUT_LEN     27
#define M_SEC_PUB_KEY_CERT_NAME_AUT_CHARSET 4

/*************************************************************************
Define the name of the wtls non-repudiation cert, the name length, and 
character set.In this cert, only pk is included.
*************************************************************************/
#define M_SEC_PUB_KEY_CERT_NAME_NP "BREW PK NONRepudiation Cert"
#define M_SEC_PUB_KEY_CERT_NAME_NP_LEN     27
#define M_SEC_PUB_KEY_CERT_NAME_NP_CHARSET 4

/*************************************************************************
Define the maximum number of certificates to be sent in TrustedKeyIds 
in wtls. used in iwim.c
*************************************************************************/
#define M_SEC_MAX_NBR_OF_CERTS_IN_TRUSTED_KEY_IDS 60

/*************************************************************************
Define how many tries the user can enter the wrong PIN before 
it is locked. only used in iue.c
*************************************************************************/
#define M_SEC_MAX_NBR_PIN_TRIES     3 

/*************************************************************************
The max lenth of the cert name input by user.only used in iue.c
*************************************************************************/
#define M_SEC_MAX_INPUT_LEN         30

/*************************************************************************
The macro is used in cm.c, and it can control whether a Pin code should be
entered when a cert is removed.
*************************************************************************/
#define M_SEC_CFG_PIN_DELETE_CERT

#endif /* end of SEC_CFG_H */

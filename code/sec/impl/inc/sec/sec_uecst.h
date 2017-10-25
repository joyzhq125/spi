/*=====================================================================================
    FILE NAME : sec_uecst.h
    MODULE NAME : SEC
    
    GENERAL DESCRIPTION
        
    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2002-2006 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
    
    Modification Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-07-22 Bird     none      Init
    
=====================================================================================*/

/***************************************************************************************************
*   multi-Include-Prevent Section
***************************************************************************************************/
#ifndef SEC_UECST_H
#define SEC_UECST_H

/*Move from I interface head by bird 20061208*/
/*1.sec.h*/
/************************************************************************
*   string constants                                                                                                   *
************************************************************************/
/********************************************
*   Button string constants                                        *
********************************************/
/*"Ok"*/
#define G_SEC_ID_OK                                 ( 0x00) 
/*"Cancel"*/
#define G_SEC_ID_CANCEL                             ( 0x01) 
/*"View"*/
#define G_SEC_ID_VIEW                               ( 0x02) 
/*" "*/
#define G_SEC_ID_NONE                               ( 0x03) 


/********************************************
*   Warning constrants                                             *
********************************************/
/*"Not enough memory to perform."*/
#define G_SEC_ID_WARNING_NOTENOUGH_MEMORY   		( 0x05) 
/*"An error occured."*/
#define G_SEC_ID_WARNING_GENERAL_ERROR       		( 0x06) 
/*"Wrong input parameter."*/
#define G_SEC_ID_WARNING_WRONG_PARAMETER            ( 0x07) 
/*"No authentication key is found to open the softWIM."*/
#define G_SEC_ID_WARNING_NOAUTKEY_OPENWIM           ( 0x08) 
/*"The user is not verified."*/
#define G_SEC_ID_WARNING_USER_NOT_VERIFIED          ( 0x09) 
/********************************************
*   pin                                                                     *
********************************************/
/*"Enter:"*/
#define G_SEC_ID_PIN                                ( 0x0a) 

/*"Could not verify the input. Enter:"*/
#define G_SEC_ID_WRONG_PIN                          ( 0x0c) 

/*2.isecb.h*/

/*dialog event*/
#define    G_Evt_PIN            0x33
#define    G_Evt_WARNING        0x34
#define    G_Evt_CHANGEPIN                 0x35
#define    G_Evt_CREATEPIN                 0x37

/* Button string constants */

/*"Remove"*/
#define G_SEC_ID_REMOVE                       ( 0x0e) 
/*"Overwrite"*/
#define G_SEC_ID_OVERWRITE                    ( 0x0f) 
/*"Save"*/
#define G_SEC_ID_SAVE                         ( 0x10) 

/* pin related*/
/*"non-repudiation key"*/
#define G_SEC_ID_NONREP_KEY                   ( 0x28) 
/*"authentication key"*/
#define G_SEC_ID_AUTH_KEY                     ( 0x29) 

/*"Input a PIN:"*/
#define G_SEC_ID_GENPIN                       ( 0x2a) 
/*"PIN used to generate authentication key"*/
#define G_SEC_ID_PIN_GENAUTKEY                ( 0x2b) 
/*"PIN used to generate non-repudiation key"*/
#define G_SEC_ID_PIN_GENNRKEY                 ( 0x2c) 

/*"Input a new PIN"*/
#define G_SEC_ID_CHANGE_PINNEW                ( 0x2d) 
/*"The Input PIN is old. Input a new PIN"*/
#define G_SEC_ID_WRONG_OLDPIN                 ( 0x2e) 

/*"The PIN is too long. Input a new PIN:"*/
#define G_SEC_ID_PIN_TOOLONG                  ( 0x2f) 
/*"The PIN is too short. Input a new PIN:"*/
#define G_SEC_ID_PIN_TOOSHORT                 ( 0x30) 
/*"The new PINs did not match. Input a new PIN:"*/
#define G_SEC_ID_PIN_MISIATCH                 ( 0x31) 

/*"the PIN of the authentication key used to save certificate"*/
#define G_SEC_ID_PIN_SAVE_CERT                ( 0x32) 
/*"the PIN of the authentication key used to remove certificate"*/
#define G_SEC_ID_PIN_DEL_CERT                 ( 0x33) 

typedef struct tagSt_Pin
{
    WE_INT32 iInfoId;
    WE_INT32 iInfoIdAfer;
}St_Pin;
typedef struct tagSt_PinAction
{
    WE_CHAR* pcPinValue;
    WE_BOOL  bIsOk;
} St_PinAction;

typedef struct tagSt_Warning
{
    WE_INT32 iWarningContent;
}St_Warning;


typedef struct tagSt_ChangePin
{
   WE_INT32 iNewPinId;
   WE_INT32 iNewPinIdAfter;
}St_ChangePin;

typedef struct tagSt_CreatePin
{
   WE_INT32 iInfoId;
   WE_INT32 iInfoIdAfter; 
}St_CreatePin;

typedef struct tagSt_ChangePinAction
{
   WE_CHAR* pcOldPinValue;
   WE_CHAR* pcNewPinValue;
   WE_CHAR* pcNewPinAgainValue;
   WE_BOOL  bIsOk;
} St_ChangePinAction;


typedef struct tagSt_CreatePinAction
{
   WE_CHAR* pcPinValue;
   WE_BOOL bIsOk;
}St_CreatePinAction;


/*isecw.h*/
/************************************************
*   Warning constrants                          *
************************************************/
/*"No authentication key exists."*/
#define G_SEC_ID_WARNING_MISSING_AUT_KEY_HANDSHAKE       ( 0x14) 
/* Title-"Select a certificate:"*/
#define G_SEC_ID_TITLE_CHOOSE_CERT                       ( 0x15) 

/************************************************************************
*   string constants                                                                                                   *
************************************************************************/
/************************************************
*   confirmation constants                                               *
************************************************/
/* "No CA certificate is found. Connect without server or client 
   authentication?" */
#define G_SEC_ID_CONFIRM_CONNECT_NO_AUTHENTICATION       ( 0x0e) 

/* "No matched user certificate is found. Connect without user 
   authentication and only perform server authentication?" */
#define G_SEC_ID_CONFIRM_CONNECT_NO_USER_AUTHENTICATION  ( 0x0f) 

/* "No matched CA certificate is found. Continue?" */
#define G_SEC_ID_CONFIRM_NO_MATCHED_CA_CERT              ( 0x10) 

/* "Error in certificate. Continue?" */
#define G_SEC_ID_CONFIRM_ERROR_IN_CERT                   ( 0x11) 

/* "Certificate is expired. Continue?" */
#define G_SEC_ID_CONFIRM_EXPIRED_CERT                    ( 0x12) 

/* "No certificate is received from gateway. Connect without gateway 
   or client authentication?" */
#define G_SEC_ID_CONFIRM_NO_GATEWAY_CERT                 ( 0x13) 

/*isigntext.h*/
/************************************************************************/
/* string constants                                                     */
/************************************************************************/


/* Warning constrants */
/*"No non-repudiation key is found ."*/
#define G_SEC_ID_WARNING_MISSING_NR_KEY_SIGN_TEXT  ( 0x0f) 
/*"The contract can't be stored."*/
#define G_SEC_ID_WARNING_COULD_NOT_STORE_CONTRACT  ( 0x10) 

/* pin */
/*"PIN of the non-repudiation key used to sign"*/
#define G_SEC_ID_PIN_SIGN_TEXT                     ( 0x11) 

/********************************************
*   Title                                                                   *
********************************************/
/*"Select a certificate to get public key:"*/
#define G_SEC_ID_TITLE_SELECT_CERTPUBKEY            ( 0x0d) 

/* Dialog Event                                                         */
/************************************************************************/
#define    G_Evt_SIGNTEXTCONFIRM            0x36

typedef struct tagSt_SignTextConfirm
{
   WE_INT32 iInfoId;
   WE_CHAR* pcText;
}St_SignTextConfirm;

typedef struct tagSt_SignTextConfirmAction
{
   WE_BOOL bIsOk;
}St_SignTextConfirmAction;


/***************************************************************************************************
*   macro Define Section
***************************************************************************************************/
/* Button constants */
#define M_SEC_ID_OK             G_SEC_ID_OK
#define M_SEC_ID_CANCEL         G_SEC_ID_CANCEL
#define M_SEC_ID_NONE           G_SEC_ID_NONE
#define M_SEC_ID_DELETE         G_SEC_ID_REMOVE
#define M_SEC_ID_OVERWRITE      G_SEC_ID_OVERWRITE
#define M_SEC_ID_STORE          G_SEC_ID_SAVE
#define M_SEC_ID_VIEW           G_SEC_ID_VIEW

/* confirmation constants */
#define M_SEC_ID_CONFIRM_1      G_SEC_ID_CONFIRM_CONNECT_NO_AUTHENTICATION
#define M_SEC_ID_CONFIRM_2      G_SEC_ID_CONFIRM_CONNECT_NO_USER_AUTHENTICATION
#define M_SEC_ID_CONFIRM_3      G_SEC_ID_CONFIRM_CANNOT_VERIFY_CERT
#define M_SEC_ID_CONFIRM_4      G_SEC_ID_CONFIRM_SAME_DATE_CERT_EXIST
#define M_SEC_ID_CONFIRM_5      G_SEC_ID_CONFIRM_OLD_DATE_CERT_EXIST
#define M_SEC_ID_CONFIRM_6      G_SEC_ID_CONFIRM_NEW_DATE_CERT_EXIST
#define M_SEC_ID_CONFIRM_7      G_SEC_ID_CONFIRM_SAVE_SUCCESS
#define M_SEC_ID_CONFIRM_8      G_SEC_ID_CONFIRM_NO_MATCHED_CA_CERT

#define M_SEC_ID_CONFIRM_9      G_SEC_ID_CONFIRM_CANNOT_STORE_CERT
#define M_SEC_ID_CONFIRM_10     G_SEC_ID_CONFIRM_ERROR_IN_CERT
#define M_SEC_ID_CONFIRM_11     G_SEC_ID_CONFIRM_EXPIRED_CERT
#define M_SEC_ID_CONFIRM_12     G_SEC_ID_CONFIRM_NO_GATEWAY_CERT

/*Name confirm*/
#define M_SEC_ID_CONFIRM_13     -1  //G_SEC_ID_CONFIRM_REMOVE_OLD_CA_CERT

/* Certificate  */
#define M_SEC_ID_CONFIRM_14     G_SEC_ID_CONFIRM_SAVE_CERT

/*signText confirm*/
#define M_SEC_ID_CONFIRM_15     G_SEC_ID_CONFIRM_SIGN

/* Session  */
#define M_SEC_ID_SESSION_WTLS       G_SEC_ID_SESSION_WTLS  

#define M_SEC_ID_SESSION_MD5        G_SEC_ID_SESSION_MD5
#define M_SEC_ID_SESSION_SHA1       G_SEC_ID_SESSION_SHA1

#define M_SEC_ID_SESSION_BULK_NULL          G_SEC_ID_SESSION_BULK_NULL
#define M_SEC_ID_SESSION_BULK_RC5           G_SEC_ID_SESSION_BULK_RC5
#define M_SEC_ID_SESSION_BULK_RC5_40        G_SEC_ID_SESSION_BULK_RC5_40
#define M_SEC_ID_SESSION_BULK_RC5_56        G_SEC_ID_SESSION_BULK_RC5_56
#define M_SEC_ID_SESSION_BULK_RC5_64        G_SEC_ID_SESSION_BULK_RC5_64
#define M_SEC_ID_SESSION_BULK_3DES          G_SEC_ID_SESSION_BULK_3DES
 
#define M_SEC_ID_SESSION_KEYEXCH_NULL               G_SEC_ID_SESSION_KEYEXCH_NULL
#define M_SEC_ID_SESSION_KEYEXCH_RSA_ANON           G_SEC_ID_SESSION_KEYEXCH_RSA_ANON
#define M_SEC_ID_SESSION_KEYEXCH_RSA_ANON_512       G_SEC_ID_SESSION_KEYEXCH_RSA_ANON_512
#define M_SEC_ID_SESSION_KEYEXCH_RSA_ANON_768       G_SEC_ID_SESSION_KEYEXCH_RSA_ANON_768
#define M_SEC_ID_SESSION_KEYEXCH_RSA                G_SEC_ID_SESSION_KEYEXCH_RSA
#define M_SEC_ID_SESSION_KEYEXCH_RSA_512            G_SEC_ID_SESSION_KEYEXCH_RSA_512
#define M_SEC_ID_SESSION_KEYEXCH_RSA_768            G_SEC_ID_SESSION_KEYEXCH_RSA_768

/* Warning constrants */
#define M_SEC_ID_WARNING_COULD_NOT_STORE_CONTRACT   G_SEC_ID_WARNING_COULD_NOT_STORE_CONTRACT
#define M_SEC_ID_WARNING_INSUFFICIENT_MEMORY        G_SEC_ID_WARNING_NOTENOUGH_MEMORY
#define M_SEC_ID_WARNING_GENERAL_ERROR              G_SEC_ID_WARNING_GENERAL_ERROR
#define M_SEC_ID_WARNING_INVALID_PARAMETER          G_SEC_ID_WARNING_WRONG_PARAMETER 
#define M_SEC_ID_WARNING_MISSING_NR_KEY_SIGN_TEXT   G_SEC_ID_WARNING_MISSING_NR_KEY_SIGN_TEXT
#define M_SEC_ID_WARNING_MISSING_AUT_KEY_OPEN_WIM   G_SEC_ID_WARNING_NOAUTKEY_OPENWIM
#define M_SEC_ID_WARNING_USER_NOT_VERIFIED          G_SEC_ID_WARNING_USER_NOT_VERIFIED

#define M_SEC_ID_WARNING_MISSING_AUT_KEY_HANDSHAKE  G_SEC_ID_WARNING_MISSING_AUT_KEY_HANDSHAKE


/* PIN  */
#define M_SEC_ID_NONREP_KEY         G_SEC_ID_NONREP_KEY
#define M_SEC_ID_AUTH_KEY           G_SEC_ID_AUTH_KEY

#define M_SEC_ID_GEN_PIN            G_SEC_ID_GENPIN
#define M_SEC_ID_PIN_GEN_AUT_KEY    G_SEC_ID_PIN_GENAUTKEY
#define M_SEC_ID_PIN_GEN_NR_KEY     G_SEC_ID_PIN_GENNRKEY

#define M_SEC_ID_CHANGE_PIN_NEW     G_SEC_ID_CHANGE_PINNEW

#define M_SEC_ID_PIN_TOO_LONG       G_SEC_ID_PIN_TOOLONG
#define M_SEC_ID_PIN_TOO_SHORT      G_SEC_ID_PIN_TOOSHORT
#define M_SEC_ID_PIN_MISIATCH       G_SEC_ID_PIN_MISIATCH

#define M_SEC_ID_PIN                G_SEC_ID_PIN
#define M_SEC_ID_PIN_OPEN_WIM       -1  //G_SEC_ID_PIN_OPENWIM
#define M_SEC_ID_PIN_SIGN_TEXT      G_SEC_ID_PIN_SIGN_TEXT
#define M_SEC_ID_PIN_STORE_CERT     G_SEC_ID_PIN_SAVE_CERT
#define M_SEC_ID_PIN_DEL_CERT       G_SEC_ID_PIN_DEL_CERT

#define M_SEC_ID_WRONG_OLD_PIN      G_SEC_ID_WRONG_OLDPIN
#define M_SEC_ID_WRONG_PIN          G_SEC_ID_WRONG_PIN

/* Hash  */
#define M_SEC_ID_ENTER_HASH         G_SEC_ID_INPUT_HASH
#define M_SEC_ID_WRONG_HASH         G_SEC_ID_ERROR_HASH

/* Month  */
#define M_SEC_ID_JAN                G_SEC_ID_JANUARY
#define M_SEC_ID_FEB                G_SEC_ID_FEBRUARY
#define M_SEC_ID_MARCH              G_SEC_ID_MARCH
#define M_SEC_ID_APR                G_SEC_ID_APRIL
#define M_SEC_ID_MAY                G_SEC_ID_MAY
#define M_SEC_ID_JUNE               G_SEC_ID_JUNE
#define M_SEC_ID_JULY               G_SEC_ID_JULY
#define M_SEC_ID_AUG                G_SEC_ID_AUGUST
#define M_SEC_ID_SEP                G_SEC_ID_SEPTEMBER
#define M_SEC_ID_OCT                G_SEC_ID_OCTOBER
#define M_SEC_ID_NOV                G_SEC_ID_NOVEMBER
#define M_SEC_ID_DEC                G_SEC_ID_DECEMBER

/*Title*/
#define M_SEC_ID_TITLE_CHOOSE_CERT              G_SEC_ID_TITLE_CHOOSE_CERT
#define M_SEC_ID_TITLE_CHOOSE_CERT_PUB_KEY      G_SEC_ID_TITLE_SELECT_CERTPUBKEY


#endif

/*==================================================================================================
    HEADER NAME : sec_wimpkcresp.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
    
    GENERAL DESCRIPTION
        In this file,define the function prototype 
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-04      Sam             None         Init
==================================================================================================*/

/***************************************************************************************************
*   Multi-Include-Prevent Section
***************************************************************************************************/
#ifndef SEC_SWMN_H
#define SEC_SWMN_H

/***************************************************************************************************
*   include File Section
***************************************************************************************************/

/***************************************************************************************************
* Macro Define Section
***************************************************************************************************/
/* state of wim pkc response */
#define M_SEC_WIM_GENERATE_KEYPAIR                          1
#define M_SEC_WIM_PRODUCE_PUBKEY_CERT                       2
#define M_SEC_WIM_WTLS_KEYEXCHANGE_RSA                      3
#define M_SEC_WIM_COMPUTE_SIGNATURE                         6
#define M_SEC_WIM_FIND_MATCHING_PRIVATE_KEY_START           7
#define M_SEC_WIM_FIND_MATCHING_PRIVATE_KEY_COMPUTE         8
#define M_SEC_WIM_FIND_MATCHING_PRIVATE_KEY_VERIFY          9
#define M_SEC_WIM_GET_USER_CERT_AND_SIGNED_KEY              10
#define M_SEC_WIM_SIGNTEXT                                  11
#define M_SEC_WIM_SIGNATURE_VERIFICATION                    12
#define M_SEC_WIM_VERIFY_ROOT_CERTIFICATE                   13
#define M_SEC_WIM_VERIFY_CERT_CHAIN_INTERMEDIATE            14
#define M_SEC_WIM_VERIFY_CERT_CHAIN_ROOT                    15
#define M_SEC_WIM_CHECK_PRESTORED_ROOT_CERT                 16

#define M_SEC_WIM_USER_CERT_REQ_COMPUTE_SIGNATURE           17

/***************************************************************************************************
*   Type Define Section
***************************************************************************************************/
/* this structure used to set up the chain of element and store the information */
typedef struct tagSt_SecWimElement 
{
    WE_INT32    iTargetID;      /* Target identifier */
    WE_INT32    iState;         /* state of operation */
    WE_HANDLE        hItype;/*add by bird 061113*/
    void        *pvPointer;     /* pointer to the data whic in memory */
    struct tagSt_SecWimElement *pstNext;    /* pointer to the next element */
} St_SecWimElement;

/***************************************************************************************************
*   Prototype Declare Section
***************************************************************************************************/
/*================================================================================================
 * Initialize the state of PKC response.
 *================================================================================================*/
void Sec_WimInit( WE_HANDLE hSecHandle );

/*================================================================================================
 * Get the specific element from the queue
 *================================================================================================*/
St_SecWimElement* Sec_WimGetSpecElement( WE_HANDLE hSecHandle, WE_INT32 iWid );

/*================================================================================================
 * Operation of adding the element to the queue
 *================================================================================================*/
void Sec_WimAddPreassignElement( WE_HANDLE hSecHandle, St_SecWimElement *pstElement );

/*================================================================================================
 * Evaluat the new element and add  to the queue
 *================================================================================================*/
WE_INT32 Sec_WimAddNewElement
(
    WE_HANDLE   hSecHandle, 
    WE_INT32    iTargetID, 
    WE_INT32    iState, 
    void        *pvPointer 
);

void Sec_WimDelElementById(WE_HANDLE hSecHandle);
void Sec_WimRemoveElement(WE_HANDLE hSecHandle);
void Sec_WimHandlePubKeyRSAEncryptResp
(
    WE_HANDLE                   hSecHandle, 
    St_SecCrptEncryptPkcResp    *pstParam 
);

 void Sec_WimHandleVerifyRSASignatureResp
(
    WE_HANDLE                       hSecHandle, 
    St_SecCrptVerifySignatureResp   *pstParam 
);

 void Sec_WimHandleComputeRSADigSignResp
(
    WE_HANDLE                       hSecHandle, 
    St_SecCrptComputeSignatureResp  *pstParam 
);

 void Sec_WimHandleGenerateRSAKeypairsResp
(
    WE_HANDLE                       hSecHandle, 
    St_SecCrptGenKeypairResp   *pstParam 
);


#endif /* endif SEC_SWMN_H */

/*
 * Copyright (C) Techfaith 2002-2005.
 * All rights reserved.
 *
 * This software is covered by the license agreement between
 * the end user and Techfaith, and may be 
 * used and copied only in accordance with the terms of the 
 * said agreement.
 *
 * Techfaith assumes no responsibility or 
 * liability for any errors or inaccuracies in this software, 
 * or any consequential, incidental or indirect damage arising
 * out of the use of the software.
 *
 */

/* !\file macomm.h
 *  Network communication handling.
 */

#ifndef MSA_COMM_H
#define MSA_COMM_H

/******************************************************************************
* Constants
*****************************************************************************/

/*! \enum MsaCommSignalId
 *  Signals for the COMM FSM */
typedef enum 
{
    /* ---------- Send message --------------------*/
    /*! Send a message
     * i_param  = signal
     * u_param1 = fsm
     * u_param2 = msgId
     */
    MSA_SIG_COMM_SEND_MESSAGE, /* Not used! Use Msa_SendMessage() instead! */
        
    MSA_SIG_COMM_SEND_NETWORK_STATUS_RSP,
        
    MSA_SIG_COMM_SEND_MESSAGE_INFO_RETRIEVED,
                    
    MSA_SIG_COMM_SEND_MESSAGE_SIZE_OK, 

    MSA_SIG_COMM_SEND_VERIFICATION_RSP,
        
    /*! Send message response from the MMS Service
     * u_param1 = result
     * u_param2 = msgId
     */
    MSA_SIG_COMM_SEND_MSG_RESPONSE,

    /*! Cancel an ongoing send operation.
     * u_param1 = msgId
     */
    MSA_SIG_COMM_SEND_CANCEL,

    /* ------------ Message retrieval -------------*/
    /*! Download a message.
     * i_param  = signal
     * u_param1 = fsm
     * u_param2 = msgId - the message to download 
     */
    MSA_SIG_COMM_DOWNLOAD,

    /* The reposne for the "get messge info" reguest
     *
     * p_param = The message info or NULL, see #MmsMessageFolderInfo.
     */
    MSA_SIG_COMM_DOWNLOAD_MSG_TYPE_RSP,

    /*! Network status information from registry has been updated.
     */
    MSA_SIG_COMM_DOWNLOAD_NETWORK_STATUS_RSP,

    /*! Read notification response.
     */
    MSA_SIG_COMM_DOWNLOAD_NOTIF_FILE_NOTIFY_RSP,

    /*! Retrieval modes and possibly user interaction is done.
     * u_param1 = result
     */
    MSA_SIG_COMM_DOWNLOAD_VERIFICATION_RSP,

    /*! The message download response from the MMS Service.
     * u_param1 = result
     * u_param2 = msgId
     */
    MSA_SIG_COMM_DOWNLOAD_RESPONSE,

    /*! Cancel message download
     * u_param1 = msgId the wid of the notification that are currently 
     *                  downloaded
     */
    MSA_SIG_COMM_DOWNLOAD_CANCEL,

    /*! Message send/download status sent from the MMS Service
     * p_param = The current send/download status, see MmsProgressStatus.
     */
    MSA_SIG_COMM_PROGRESS_STATUS,

    /*! User closed settings error dialog */
    MSA_SIG_COMM_SETTINGS_FAILED_DIALOG_RSP
}MsaCommSignalId;

/******************************************************************************
 * Prototype declarations 
 *****************************************************************************/

void Msa_CommInit(void); 
void Msa_CommTerminate(void);
void Msa_SendMessage(MsaStateMachine eFsm, unsigned int uiRetSig, MmsMsgId uiMsgId, WE_BOOL bShowWarning);
void Msa_DownloadMessage(MsaStateMachine eFsm, unsigned int uiRretSig, MmsMsgId uiMsgId);

WE_BOOL Msa_ShowDownloadErrorDialog(MmsResult eResult, MsaStateMachine eFsm, int iSignal);
WE_BOOL Msa_ShowSendDialog(MmsResult eResult, MsaStateMachine eFsm, int iSignal);
                      
#endif /* MSA_COMM_H */

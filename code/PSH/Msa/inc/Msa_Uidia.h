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

/*!\file mauidia.h
 * \brief Dialog handling.
 */

#ifndef _MAUIDIA_H_
#define _MAUIDIA_H_

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! \enum MsaDialogType
 *  Global dialog types */
typedef enum
{
    MSA_DIALOG_ALERT = 0x01, /* !< Alert dialog */
    MSA_DIALOG_WARNING,      /* !< Warning dialog */
    MSA_DIALOG_INFO,         /* !< Information dialog */
    MSA_DIALOG_ERROR,        /* !< Error dialog */
    MSA_DIALOG_CONFIRM       /* !< Confirm dialog */
}MsaDialogType;

/*! \enum MsaConfirmDialogResult
 *  Actions related to the confirmation dialog */
typedef enum
{
    MSA_CONFIRM_DIALOG_OK,     /* !< The End User presses ok */
    MSA_CONFIRM_DIALOG_CANCEL  /* !< The End USer presses cancel */
}MsaConfirmDialogResult;

/******************************************************************************
 * Prototypes
 *****************************************************************************/

WE_BOOL msaInitDia(void);
void msaTerminateDia(void);
WE_BOOL msaDiaHandleSignal(WE_UINT16 signal, void *p);
WE_BOOL msaShowConfirmDialog(WE_UINT32 textStrId, const char *str, 
    MsaStateMachine fsm, int signal, unsigned int wid);
WE_BOOL msaShowDialog(WeStringHandle strHandle, MsaDialogType dialogType);
WE_BOOL msaShowDialogWithCallback(WeStringHandle strHandle, 
    MsaDialogType dialogType, MsaStateMachine fsm, int signal);
WE_BOOL msaShowPanicDialog(void);
unsigned int msaGetDialogCount(void);
WE_BOOL msaCheckDialogs(void);

#endif

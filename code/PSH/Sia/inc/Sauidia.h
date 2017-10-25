/*
 * Copyright (C) Techfaith, 2002-2005.
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

/*!\file Sauidia.h
 * \brief Dialog handling.
 */

#ifndef SAUIDIA_H
#define SAUIDIA_H

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! \enum SiaDialogType
 *  Global dialog types */
typedef enum
{
    SIA_DIALOG_ALERT = 0x01, /* !< Alert dialog */
    SIA_DIALOG_WARNING,      /* !< Warning dialog */
    SIA_DIALOG_INFO,         /* !< Information dialog */
    SIA_DIALOG_ERROR,        /* !< Error dialog */
    SIA_DIALOG_CONFIRM       /* !< Confirm dialog */
}SiaDialogType;

/*! \enum SiaConfirmDialogResult
 *  Actions related to the confirmation dialog */
typedef enum
{
    SIA_CONFIRM_DIALOG_OK,     /* !< The End User presses ok */
    SIA_CONFIRM_DIALOG_CANCEL  /* !< The End USer presses cancel */
}SiaConfirmDialogResult;

/******************************************************************************
 * Prototypes
 *****************************************************************************/

WE_BOOL smaInitDia(void);
void smaTerminateDia(void);
WE_BOOL smaDiaHandleSignal(WE_UINT16 signal, void *p);
WE_BOOL smaShowConfirmDialog(WE_UINT32 textStrId, const char *str, 
    SiaStateMachine fsm, int signal, unsigned int id);
WE_BOOL smaShowDialog(WeStringHandle strHandle, SiaDialogType dialogType);
WE_BOOL smaShowDialogWithCallback(WeStringHandle strHandle, 
    SiaDialogType dialogType, SiaStateMachine fsm, int signal);
WE_BOOL smaShowPanicDialog(void);
unsigned int smaGetDialogCount(void);
WE_BOOL smaCheckDialogs(void);

#endif /* SAUIDIA_H */

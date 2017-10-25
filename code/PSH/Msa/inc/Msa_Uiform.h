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

/*!\file mauiform.h
 * \brief Common input forms.
 */

#ifndef _MAUIFORM_H_
#define _MAUIFORM_H_

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! \enum MsaInputType
 *  Input types */
typedef enum
{
    MSA_TEXT_INPUT,         /* !< Multiple lines of text */
    MSA_SINGLE_LINE_INPUT,  /* !< One line of text */
    MSA_RADIO_BUTTON_INPUT, /* !< A list of radio buttons */
    MSA_SELECT_INPUT        /* !< A list of items where one can be selected */
}MsaInputType;

/*! Callback for user actions */
typedef WE_BOOL (*MsaInputActionCallBack)(char *, unsigned int, void *, \
                                           unsigned int);

typedef void (*MsaInputActionBackCallBack)(void *, unsigned int);

/******************************************************************************
 * Data-types
 *****************************************************************************/

/* !\struct MsaInputItem 
 *  \brief Data-structure used when creating an input form 
 */
typedef struct
{
    MsaInputType type;              /* !< The type of input to create */

    WeTextType     textType;       /* !< The type of text to input */
    char            *text;          /* !< The initial value */
    unsigned int    maxTextSize;    /* !< The maximum size of input data */

    int              selectedItem;  /* !< The item that is selected */
    WeStringHandle  *items;        /* !< A list of strings handles to be
                                         displayed, use the handle 0 for the
                                         last element */
    void            *dataPtr;       /* !< A pointer provided in the callback */
    unsigned int    uintValue;      /* !< An integer value provided in the 
                                          callback */
    MsaInputActionCallBack callback; /*!< The OK callback function */
    MsaInputActionBackCallBack backCallback; /*!< The BACK callback function */
}MsaInputItem;

/******************************************************************************
 * Prototypes
 *****************************************************************************/

void msaInitForms(void);
void msaTerminateForms(void);
WE_BOOL msaCreateInputForm(const MsaInputItem *item, 
    WE_UINT32 titleStrHandle);

#endif

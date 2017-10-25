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

/*!\file mauime.h
 * \brief Definitions for the message editor UI.
 */

#ifndef _MAUIME_H_
#define _MAUIME_H_

/******************************************************************************
 * Constants
 *****************************************************************************/

/* The maximum length for a input value */
#define MSA_ME_MAX_ADDRESS_SIZE     30

/* !\enum MsaMeAddrListMenuItem
 */
typedef enum
{
    MSA_ME_ADDR_LIST_DELETE,
    MSA_ME_ADDR_LIST_BACK
}MsaMeAddrListMenuItem;

/******************************************************************************
 * Prototypes
 *****************************************************************************/

WE_BOOL msaMeCreatePropView(const MsaPropItem *propItem, 
    MsaMeStartupMode mode);
void     msaMeDeletePropView(void);
WE_BOOL msaMeSetPropertyValues(const MsaPropItem *propItem);
void     msaMeUiInit(void);

WE_BOOL msaMeCreateNavMenu(MsaNavMenuItem *items);
void     msaMeDeleteNavMenu(void);
char     *msaMeGetMessageText(void);

WE_BOOL msaMeCreateAddrInputView(MsaMeProperty type, const MsaAddrItem *items, 
    unsigned int editIndex);
void     msaMeDeleteAddrInputView(void);

WE_BOOL msaMeCreateAddrListView(MsaMeProperty type, MsaAddrItem **items);
void     msaMeDeleteAddrListView(void);
WE_BOOL msaMeUpdateAddrListView(const MsaAddrItem *items);

WE_BOOL msaMeCreateAddrListMenu(void);
void     msaMeDeleteAddrListMenu(void);
WE_BOOL msaUiMeIsPropertyViewActive(void);

#endif

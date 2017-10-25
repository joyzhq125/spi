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

/*!\file Msa_Addr.h
 * \brief Address handling.
 */

#ifndef MSA_ADDR_H
#define MSA_ADDR_H

#ifndef MSA_INTSIG_H
#error You need to include Msa_Intsig.h before Msa_Addr.h!
#endif

/******************************************************************************
 * Constants
 *****************************************************************************/

/******************************************************************************
 * Prototypes 
 *****************************************************************************/

void Msa_FreeMmsAddressList(MmsAddressList *pstAddrList);

WE_BOOL Msa_AddrItemInsert(MsaAddrItem **ppstCurrent, const char *pcName, 
    MsaAddrType eAddrType, const char *pcAddr);
void Msa_AddrListFree(MsaAddrItem **ppstCurrent);
unsigned int Msa_AddrItemCount(const MsaAddrItem *pstItems);
MsaAddrItem *Msa_AddrItemAppend(MsaAddrItem *pstFirst, MsaAddrItem *pstSecond);
void Msa_AddrItemMerge(MsaAddrItem *pstItems, const MsaAddrItem *pstLookupEntry);
void Msa_AddrItemDelete(MsaAddrItem **ppstItems, unsigned int uiIndex);
char *Msa_AddrItemToStr(MsaAddrItem *pstItem);
MsaAddrType Msa_MmsAddrTypeToMsa(MmsAddressType eMmsType);
MsaAddrType Msa_GetAddrType(const char *pcAddr);
WE_BOOL Msa_AddrListToMms(const MsaAddrItem *pstAddr, MmsAddressList **ppstMmsAddr);
MsaAddrItem *Msa_MmsToAddrList(const MmsAddressList *pstList, 
    const char *pstIgnoreAddr);
MsaAddrItem *Msa_MmsFromToAddrItem(const MmsAddress *pstFromAddr);
MsaAddrItem *Msa_AddrStrToList(char *pcStr);
void Msa_ChangeAddr(MsaAddrItem *pstItems, MsaAddrItem *pstNewItem, unsigned int uiIndex);
void Msa_NameLookup(MsaAddrItem *pstList, MsaStateMachine eFsm, int iSig);
#endif


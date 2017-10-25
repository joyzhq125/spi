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

/*!\file mautils.c
 * \brief Utility functions for MSA.
 */

/* WE */
#include "We_Mem.h"
#include "We_Def.h"
#include "We_Cmmn.h"
#include "We_Lib.h"
#include "We_Core.h"
#include "We_Log.h"

/* MMS */
#include "Mms_Def.h"

/* MSA */
#include "Msa_Cfg.h"
#include "Msa_Def.h"
#include "Msa_Env.h"
#include "Msa_Types.h"
#include "Msa_Mem.h"
#include "Msa_Intsig.h"
#include "Msa_Addr.h"
#include "Msa_Pbh.h"

/******************************************************************************
 * Data-types
 *****************************************************************************/

/******************************************************************************
 * Constants
 *****************************************************************************/

#define IS_PHONE_NO_CHARACTER(c)   ((c == '+') || (c == '-') || (c == '0') || \
                                    (c == '1') || (c == '2') || (c == '3') || \
                                    (c == '4') || (c == '5') || (c == '6') || \
                                    (c == '7') || (c == '8') || (c == '9'))


/*! Valid characters in a host name */
#define IS_HOST_LETTER(x) ( \
    (x) >= '0' && (x) <= '9' || \
    (x) >= 'a' && (x) <= 'z' || \
    (x) >= 'A' && (x) <= 'Z' || \
    (x) == '.' || (x) == '-' || (x) == '_')

/*! Valid characters in a email address */
#define IS_EMAIL_LETTER(x) (IS_HOST_LETTER(x) || (x) == '@' || (x) == '+')

/* Separator for multiple addresses */
#define MSA_ADDR_SEPARATOR          ','

/******************************************************************************
 * Prototypes
 *****************************************************************************/

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*! \brief De-allocates a list of addresses.
 *
 * \param addrList A list of addresses.
 *****************************************************************************/
void Msa_FreeMmsAddressList(MmsAddressList *pstAddrList)
{
    MmsAddressList *pstCur = pstAddrList;
    MmsAddressList *pstPrev = NULL;

    while (NULL != pstCur)
    {
        MSA_FREE(pstCur->current.address);
        pstCur->current.address = NULL;
        MSA_FREE(pstCur->current.name.text);
        pstCur->current.name.text = NULL;
        pstPrev = pstCur;
        pstCur = pstCur->next;
        MSA_FREE(pstPrev);
        pstPrev = NULL;
    }
}

/*!
 * \brief Validates if a string is a valid address and returns its type 
 *
 * \param addr A '\0'terminated string.
 * \return See #MsaAddrType
 *****************************************************************************/
MsaAddrType Msa_GetAddrType(const char *pcAddr)
{
    int iInteger = 0;
    MsaAddrType eRet;
    /* Check that the string is valid */
    if ((NULL == pcAddr) || (WE_EOS == pcAddr[0]))
    {
        return MSA_ADDR_TYPE_NONE;
    }
    /* Check if the string is a phone no */
    for(iInteger = 0; pcAddr[iInteger] != '\0'; iInteger++)
    {
        if (!IS_PHONE_NO_CHARACTER(pcAddr[iInteger]))
        {
            break;
        }
    }
    if ('\0' == pcAddr[iInteger])
    {
        return MSA_ADDR_TYPE_PLMN;
    }
    /* Check if it is an email address */
    for(iInteger = 0; pcAddr[iInteger] != '\0'; iInteger++)
    {
        if (!IS_EMAIL_LETTER(pcAddr[iInteger]))
        {
            return MSA_ADDR_TYPE_NONE;
        }
    }
	if(WE_EOS == pcAddr[iInteger])
	{
		eRet = MSA_ADDR_TYPE_EMAIL;

	}
	else
	{
		eRet = MSA_ADDR_TYPE_NONE;

	} /* if */
		
	return eRet;
}

/*!\brief Converts a MSA address list to a MMS address list.
 *
 * \param addr The current address list.
 * \param mmsAddr 
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL Msa_AddrListToMms
(
    const MsaAddrItem *pstAddr, 
    MmsAddressList **ppstMmsAddr
)
{
    MmsAddressList *pstMmsItem = NULL;
    MmsAddressList *pstPrev;
    /* Go through all the current addresses */
    pstPrev = NULL;
    while(NULL != pstAddr)
    {
        /* Create new item */
        pstMmsItem = MSA_ALLOC(sizeof(MmsAddressList));
        memset(pstMmsItem, 0, sizeof(MmsAddressList));
        /* Copy data */
        /* Address */
        if (NULL != pstAddr->address)
        {
            pstMmsItem->current.address = MSA_ALLOC(strlen(pstAddr->address) + 1);
            strcpy(pstMmsItem->current.address, pstAddr->address);
        }
        /* Name */
        if ((NULL != pstAddr->name) && (MSA_ADDR_TYPE_EMAIL == pstAddr->addrType))
        {
            pstMmsItem->current.name.text = MSA_ALLOC(strlen(pstAddr->name) + 1);
            strcpy(pstMmsItem->current.name.text, pstAddr->name);
        }
        pstMmsItem->current.name.charset = MMS_UTF8;
        /* Type */
        /*lint -e{788}*/
        switch(pstAddr->addrType)
        {
        case MSA_ADDR_TYPE_EMAIL:
            pstMmsItem->current.addrType = MMS_EMAIL;
            break;
        case MSA_ADDR_TYPE_PLMN:
            pstMmsItem->current.addrType = MMS_PLMN;
            break;
        default:
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
                "(%s) (%d) Erroneous address type = %d\n", __FILE__, __LINE__,
                pstAddr->addrType));
            return FALSE;
        }
        /* Insert the new item */
        if (NULL == *ppstMmsAddr)
        {
            *ppstMmsAddr = pstMmsItem;
        }
        else
        {
            /*lint -e{613} */
            pstPrev->next = pstMmsItem;
        }
        pstPrev = pstMmsItem;

        /* Next item */
        pstAddr = pstAddr->next;
    }
    return TRUE;
}

/*! \brief Converts a MMS address list to a #MsaAddrItem list
 *
 * \param list The list of MMS addresses, NULL if not used..
 * \param fromAddr The MMS from address, NULL if not used.
 * \param ignoreAddr This address is ignored when copying data from the src, 
 *                   use NULL for a full copy.
 * \return The address list or NULL.
 *****************************************************************************/
MsaAddrItem *Msa_MmsToAddrList
(
    const MmsAddressList *pstList,
    const char *pcIgnoreAddr
)
{
    MsaAddrItem *pstItems = NULL;
    MsaAddrItem *pstCurrent;
    MsaAddrItem *pstPrev = NULL;

    while(NULL != pstList)
    {
        pstCurrent = MSA_ALLOC(sizeof(MsaAddrItem));
        memset(pstCurrent, 0, sizeof(MsaAddrItem));
        if ((pstList->current.address) && !((NULL != pcIgnoreAddr) && 
            (0 == we_cmmn_strcmp_nc(pstList->current.address, pcIgnoreAddr))))
        {
            pstCurrent->address = MSA_ALLOC(strlen(pstList->current.address) + 1);
            strcpy(pstCurrent->address, pstList->current.address);
            pstCurrent->addrType = Msa_MmsAddrTypeToMsa(pstList->current.addrType);
            if ((NULL != pstList->current.name.text) && 
                (MMS_UTF8 == pstList->current.name.charset))
            {
                pstCurrent->name = MSA_ALLOC(strlen(pstList->current.name.text) + 1);
                strcpy(pstCurrent->name, pstList->current.name.text);
            }
            if (NULL == pstItems)
            {
                pstItems = pstCurrent;
            }
            else
            {
                /*lint -e{613} */
                pstPrev->next = pstCurrent;
            }
            pstPrev = pstCurrent;
        }
        pstList = pstList->next;
    }
    return pstItems;
}

/*! \brief Converts a #MmsAddress a #MsaAddrItem
 *
 * \param fromAddr The MMS from address, NULL if not used.
 * \return The address item or NULL.
 *****************************************************************************/
MsaAddrItem *Msa_MmsFromToAddrItem(const MmsAddress *pstFromAddr)
{
    MsaAddrItem *pstCurrent = NULL;

    if (pstFromAddr->address)
    {
        pstCurrent = MSA_ALLOC(sizeof(MsaAddrItem));
        memset(pstCurrent, 0, sizeof(MsaAddrItem));

        pstCurrent->address = MSA_ALLOC(strlen(pstFromAddr->address) + 1);
        strcpy(pstCurrent->address, pstFromAddr->address);
        pstCurrent->addrType = Msa_MmsAddrTypeToMsa(pstFromAddr->addrType);
        if ((NULL != pstFromAddr->name.text) && 
            (MMS_UTF8 == pstFromAddr->name.charset))
        {
            pstCurrent->name = MSA_ALLOC(strlen(pstFromAddr->name.text) + 1);
            strcpy(pstCurrent->name, pstFromAddr->name.text);
        }
    }
    return pstCurrent;
}

/*!\brief Inserts a address item first in the list of addresses.
 *
 * \param current The current address list.
 * \param name The name of the contact.
 * \param addrType The address type, #see #MsaAddrItem.
 * \param addr The address
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL Msa_AddrItemInsert
(
    MsaAddrItem **ppstCurrent,
    const char *pcName, 
    MsaAddrType eAddrType,
    const char *pcAddr
)
{
    MsaAddrItem *pstItem;
    if ((NULL == ppstCurrent) || ((NULL == pcAddr) && (NULL == pcName)))
    {
        return FALSE;
    }
    /* Allocate a new item */
    pstItem = MSA_ALLOC(sizeof(MsaAddrItem));
    memset(pstItem, 0, sizeof(MsaAddrItem));
    /* Address */
    if (NULL != pcAddr)
    {
        pstItem->address = MSA_ALLOC(strlen(pcAddr) + 1);
        strcpy(pstItem->address, pcAddr);
    }
    /* Name */
    if (NULL != pcName)
    {
        pstItem->name = MSA_ALLOC(strlen(pcName) + 1);
        strcpy(pstItem->name, pcName);
    }
    pstItem->addrType = eAddrType;
    /* Insert first in the list */
    if (NULL != *ppstCurrent)
    {
        pstItem->next = *ppstCurrent;
    }
    *ppstCurrent = pstItem;
    return TRUE;
}

/*! \brief Converts a MMS addres type to a MSA address type
 *
 * \param mmsType The MMS address type.
 * \return The corresponding MSA addres type.
 *****************************************************************************/
MsaAddrType Msa_MmsAddrTypeToMsa(MmsAddressType eMmsType)
{
    /*lint -e{788} */
    switch(eMmsType)
    {
    case MMS_PLMN:
        return MSA_ADDR_TYPE_PLMN;
    case MMS_EMAIL:
        return MSA_ADDR_TYPE_EMAIL;
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) unsupported address type = %d\n", __FILE__, __LINE__, 
            eMmsType));
        return MSA_ADDR_TYPE_NONE;
    }
}

/*!\brief De-allocates a list of addresses.
 *
 * \param current The address list to de-allocate.
 *****************************************************************************/
void Msa_AddrListFree(MsaAddrItem **ppstCurrent)
{
    MsaAddrItem *pstItem;
    while(NULL != *ppstCurrent)
    {
        pstItem = *ppstCurrent;
        /* De-allocate structure */
        MSA_FREE(pstItem->name);
        MSA_FREE(pstItem->address);
        /* Next item */
        *ppstCurrent = pstItem->next;
        MSA_FREE(pstItem);
    }
    *ppstCurrent = NULL;
}

/*!\brief Counts the number of addresses in a list.
 *
 * \param items The list of items to count
 * \return The number of items.
 *****************************************************************************/
unsigned int Msa_AddrItemCount(const MsaAddrItem *pstItems)
{
    unsigned int uiCount = 0;

    while(NULL != pstItems)
    {
        ++uiCount;
        pstItems = pstItems->next;
    }
    return uiCount;
}

/*! \brief Replaces/inserts the name entry of all items that have a 
 *         corresponding address as the lookuEntry
 *
 * \param items A list of addresses.
 * \param lookupEntry The entry to merge with the list items.
 *****************************************************************************/
void Msa_AddrItemMerge
(
    MsaAddrItem *pstItems, 
    const MsaAddrItem *pstLookupEntry
)
{
    char *pcStr = NULL;
    if (NULL == pstLookupEntry->name)
    {
        return;
    }
    while(NULL != pstItems)
    {
        if ((pstLookupEntry->addrType == pstItems->addrType) &&
            (0 == we_cmmn_strcmp_nc(pstLookupEntry->address, pstItems->address)))
        {
            if (NULL != (pcStr = MSA_ALLOC(strlen(pstLookupEntry->name) + 1)))
            {
                MSA_FREE(pstItems->name);
                pstItems->name = pcStr;
                strcpy(pstItems->name, pstLookupEntry->name);
            }
        }
        pstItems = pstItems->next;
    }
}

/*! \brief Delets an address item from the list
 *
 * \param items The list of items.
 * \param index The index to delete.
 *****************************************************************************/
void Msa_AddrItemDelete
(
    MsaAddrItem **ppstItems, 
    unsigned int uiIndex
)
{
    unsigned int uiIndexTmp = 0;
    MsaAddrItem *pstCurrent;
    MsaAddrItem *pstPrev;
    if (NULL != *ppstItems)
    {
        if (0 == uiIndex)
        {
            pstCurrent = *ppstItems;
            *ppstItems = (*ppstItems)->next;
            MSA_FREE(pstCurrent->address);
            MSA_FREE(pstCurrent->name);
            MSA_FREE(pstCurrent);
            return;
        }
        pstCurrent = *ppstItems;
        pstPrev = NULL;
        while(NULL != pstCurrent)
        {
            if (uiIndexTmp == uiIndex)
            {
                /*lint -e{613} */
                pstPrev->next = pstCurrent->next;
                MSA_FREE(pstCurrent->address);
                MSA_FREE(pstCurrent->name);
                MSA_FREE(pstCurrent);
                return;
            }
            pstPrev = pstCurrent;
            pstCurrent = pstCurrent->next;
            uiIndexTmp++;
        }
    }
}

/*! \brief Appends two list of addresses
 *
 * \param first The first list
 * \param second The second list (appended on the first one)
 * \return The new list.
 *****************************************************************************/
MsaAddrItem *Msa_AddrItemAppend
(
    MsaAddrItem *pstFirst, 
    MsaAddrItem *pstSecond
)
{
    MsaAddrItem *pstTmp;
    if (NULL == pstFirst)
    {
        return pstSecond;
    }
    pstTmp = pstFirst;
    while(NULL != pstTmp->next)
    {
        pstTmp = pstTmp->next;
    }
    pstTmp->next = pstSecond;
    return pstFirst;
}

/*! \brief Converts a address item to a string
 *
 * \param item A list of addresses
 *****************************************************************************/
char *Msa_AddrItemToStr(MsaAddrItem *pstItem)
{
    MsaAddrItem *pstTmp;
    unsigned int uiStrLen = 0;
    char *pcStr = NULL;
    char *pcStrPos;
    pstTmp = pstItem;
    /* Count string length */
    while(NULL != pstTmp)
    {
        if (MSA_ADDR_TYPE_EMAIL == pstTmp->addrType)
        {
            if (NULL != pstTmp->name)
            {
                uiStrLen += strlen(pstTmp->name);
                uiStrLen += sizeof(MSA_CFG_EMAIL_BEGIN_STR) + 
                sizeof(MSA_CFG_EMAIL_END_STR);
            }
            if (NULL != pstTmp->address)
            {
                uiStrLen += strlen(pstTmp->address);
            }
        }
        else if (MSA_ADDR_TYPE_PLMN == pstTmp->addrType)
        {
            if (NULL != pstTmp->name)
            {
                uiStrLen += strlen(pstTmp->name);
            }
            else if (NULL != pstTmp->address)
            {
                uiStrLen += strlen(pstTmp->address);
            }
        }
        uiStrLen += sizeof(MSA_CFG_MMS_ADDRESS_SEPARATOR);
        pstTmp = pstTmp->next;
    }
    uiStrLen += sizeof(WE_EOS);
    /* Allocate memory for the new string */
    pcStr = MSA_ALLOC(uiStrLen);
    /* Make the address string */
    pstTmp = pstItem;
    pcStrPos = pcStr;
    pcStrPos[0] = WE_EOS;
    while(NULL != pstTmp)
    {
        if (MSA_ADDR_TYPE_EMAIL == pstTmp->addrType)
        {
            if ((NULL != pstTmp->name) && (NULL != pstTmp->address))
            {
                sprintf(pcStrPos, "%s%s%s%s", pstTmp->name, MSA_CFG_EMAIL_BEGIN_STR, 
                pstTmp->address, MSA_CFG_EMAIL_END_STR);
            }
            else if (NULL != pstTmp->address)
            {
                sprintf(pcStrPos, "%s", pstTmp->address);
            }
            else
            {
                continue;
            }
        }else if (MSA_ADDR_TYPE_PLMN == pstTmp->addrType)
        {
            if (NULL != pstTmp->name)
            {
                sprintf(pcStrPos, "%s", pstTmp->name);
            }
            else if (NULL != pstTmp->address)
            {
                sprintf(pcStrPos, "%s", pstTmp->address);
            }
            else
            {
                continue;
            }
        }
        else
        {
            continue;
        }
        /* Append the comma separator */
        if (NULL != pstTmp->next)
        {
            strcat(pcStrPos, MSA_CFG_MMS_ADDRESS_SEPARATOR);
        }
        /* Find the next position to add */
        pcStrPos = &(pcStr[strlen(pcStr)]);
        pstTmp = pstTmp->next;
    }
    return pcStr;
}

/*! \brief Converts a comma separated list in the a list of #MsaAddrItem.
 *
 * \param str A comma separated list of addresses
 *****************************************************************************/
MsaAddrItem *Msa_AddrStrToList(char *pcStr)
{
    MsaAddrItem *pstList = NULL;
    MsaAddrItem *pstCurrent;
    MsaAddrType eAddrType;
    char *pcCurrPos;
    char *pcPrevPos;
    if (NULL == pcStr)
    {
        return pstList;
    }
    pcCurrPos = NULL;
    pcPrevPos = pcStr;
    while(NULL != (pcCurrPos = strchr(pcPrevPos, MSA_ADDR_SEPARATOR)))
    {
        pcCurrPos[0] = WE_EOS;
        eAddrType = Msa_GetAddrType(pcPrevPos);
        if (MSA_ADDR_TYPE_NONE != eAddrType)
        {
            /* Insert the address */
            pstCurrent = MSA_ALLOC(sizeof(MsaAddrItem));
            memset(pstCurrent, 0, sizeof(MsaAddrItem));
            pstCurrent->addrType = eAddrType;
            pstCurrent->address = we_cmmn_strdup(WE_MODID_MSA, pcPrevPos);
            pstList = Msa_AddrItemAppend(pstList, pstCurrent);
        }
        pcCurrPos[0] = MSA_ADDR_SEPARATOR;
        pcPrevPos = pcCurrPos + 1;
    }
    /* Handle the first/last item in the list */
    eAddrType = Msa_GetAddrType(pcPrevPos);
    if (MSA_ADDR_TYPE_NONE != eAddrType)
    {
        /* Insert the address */
        pstCurrent = MSA_ALLOC(sizeof(MsaAddrItem));
        memset(pstCurrent, 0, sizeof(MsaAddrItem));
        pstCurrent->addrType = eAddrType;
        pstCurrent->address = we_cmmn_strdup(WE_MODID_MSA, pcPrevPos);
        pstList = Msa_AddrItemAppend(pstList, pstCurrent);
    }
    return pstList;
}

/*! \brief Change an address item
 *
 * \param items The list of addresses.
 * \param newItem The new item.
 * \param index The index of the item to change.
 *****************************************************************************/
void Msa_ChangeAddr
(
    MsaAddrItem *pstItems,
    MsaAddrItem *pstNewItem,
    unsigned int uiIndex
)
{
    unsigned int uiInteger;
    for(uiInteger = 0; uiIndex != uiInteger; uiInteger++)
    {
        pstItems = pstItems->next;
    }
    /* Delete old item */
    MSA_FREE(pstItems->name);
    MSA_FREE(pstItems->address);
    pstItems->name     = pstNewItem->name;
    pstItems->address  = pstNewItem->address;
    pstItems->addrType = pstNewItem->addrType;
    MSA_FREE(pstNewItem);
}

/*! \brief Lookup a list of names.
 *
 * \param list the list of current addresses
 *****************************************************************************/
void Msa_NameLookup
(
    MsaAddrItem *pstList,
    MsaStateMachine eFsm,
    int iSig
)
{
    while(NULL != pstList)
    {
        if (NULL == pstList->name)
        {
            msaPbhLookupName(eFsm, iSig, pstList->address, pstList->addrType);
        }
        pstList = pstList->next;
    }
}


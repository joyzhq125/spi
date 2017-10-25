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

/*!\file msa_int.c
 * Reference implementation for the msa_int.h adapter functions.
 */

/* WE */
#include "We_Int.h"
#include "We_Log.h"
#include "We_Mem.h"
#include "We_Lib.h"

/* MSA */
#include "Msa_Int.h"
#include "Msa_Env.h"
#include "Msa_Mem.h"

/*! \file msa_int.c
 *  \brief Reference implementation for the msa_int.h adaptor functions.
 */

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! \brief
 *
 *****************************************************************************/
void MSAa_pbLaunch(WE_UINT16 wid)
{
    /* This items are only for testing purposes */
    /*lint -e{1776} */
    static MsaPbItem item  = 
        {MSA_PB_TYPE_PLMN, "Homer Simpson", "+46123123", NULL};
    /*lint -e{1776} */
    static MsaPbItem item2 = 
        {MSA_PB_TYPE_PLMN, NULL, "+46456456", &item};
    /*lint -e{1776} */
    static MsaPbItem item3 = 
        {MSA_PB_TYPE_EMAIL, "Marge Simpson", "marge@s.com", &item2};
    /*lint -e{1776} */
    static MsaPbItem item4 = 
        {MSA_PB_TYPE_EMAIL, NULL, "moe@s.com", &item3};
    static int tmp = 0;
    /*lint -e{1776} */
    static MsaPbItem *items[] = {&item, &item2, &item3, &item4};
    
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
        "(%s) (%d) MSAa_pbLaunch wid = %d\n", __FILE__, __LINE__, wid));
    /* Note, this is only for testing purposes. Connectors shall not normally
       be called from adaptors */
    (void)MSAc_pbLaunchRsp(MSA_PB_LAUNCH_RESULT_OK, wid, items[++tmp%4]);
}

/*! \brief
 *
 *****************************************************************************/
void MSAa_pbCancel(void)
{
    /* Note, this is only for testing purposes. Connectors shall not normally
       be called from adaptors */
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
        "(%s) (%d) MSAa_phCancel\n", __FILE__, __LINE__));
}

/*! \brief
 *
 *****************************************************************************/
void MSAa_pbLookupName(WE_UINT16 instanceId, WE_UINT8 type, void *value)
{
    const char *name = NULL;
    MsaPbItem *item = NULL;
    WE_UINT8 result = MSA_PB_LOOKUP_RESULT_OK;
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
        "(%s) (%d) MSAa_pbLookupEntry instanceId = %d, type = %d, value = "
        "\"%s\"\n", __FILE__, __LINE__, instanceId, type, value));
    /* Note, this is only for testing purposes. Connectors shall NOT normally
       be called from adaptors */
    if ((0 == we_cmmn_strcmp_nc(value, "1")) && (type == MSA_PB_TYPE_PLMN))
    {
        name = "Homer Simpson - 1";
    }
    else if ((0 == we_cmmn_strcmp_nc(value, "2")) &&
        (type == MSA_PB_TYPE_PLMN))
    {
        name = "Lisa Simpson - 2";
    }
    else if ((0 == we_cmmn_strcmp_nc(value, "3")) &&
        (type == MSA_PB_TYPE_PLMN))
    {
        name = "Marge Simpson - 3";
    }
    else if ((0 == we_cmmn_strcmp_nc(value, "homer@s.com")) &&
        (type == MSA_PB_TYPE_EMAIL))
    {
        name = "Homer Simpson - homer@s.com";
    }
    else if ((0 == we_cmmn_strcmp_nc(value, "lisa@s.com")) &&
        (type == MSA_PB_TYPE_EMAIL))
    {
        name = "Lisa Simpson - lisa@s.com";
    }
    else if ((0 == we_cmmn_strcmp_nc(value, "marge@s.com")) &&
        (type == MSA_PB_TYPE_EMAIL))
    {
        name = "Marge Simpson - marge@s.com";
    }
    /* HENI's test names */
    else if ((0 == we_cmmn_strcmp_nc(value, "+46702077473")) &&
        (type == MSA_PB_TYPE_PLMN))
    {
        name = "P800";
    }
    else if ((0 == we_cmmn_strcmp_nc(value, "46702077473@mmsciot.comverse.com")) &&
        (type == MSA_PB_TYPE_EMAIL))
    {
        name = "P800";
    }
    else if ((0 == we_cmmn_strcmp_nc(value, "+46706987190")) &&
        (type == MSA_PB_TYPE_PLMN))
    {
        name = "HENI";
    }
    else if ((0 == we_cmmn_strcmp_nc(value, "46706987190@mmsciot.comverse.com")) &&
        (type == MSA_PB_TYPE_EMAIL))
    {
        name = "HENI";
    }
    else if ((0 == we_cmmn_strcmp_nc(value, "+46706666666")) &&
        (type == MSA_PB_TYPE_PLMN))
    {
        name = "HENITSS";
    }
    else if ((0 == we_cmmn_strcmp_nc(value, "heni@tss.teleca.se")) &&
        (type == MSA_PB_TYPE_EMAIL))
    {
        name = "HENITSS";
    }
    else if ((0 == we_cmmn_strcmp_nc(value, "+46706945327")) &&
        (type == MSA_PB_TYPE_PLMN))
    {
        name = "7650";
    }
    else if ((0 == we_cmmn_strcmp_nc(value, "46706945327@mmsciot.comverse.com")) &&
        (type == MSA_PB_TYPE_EMAIL))
    {
        name = "7650";
    }
    else if ((0 == we_cmmn_strcmp_nc(value, "+46705167228")) &&
        (type == MSA_PB_TYPE_PLMN))
    {
        name = "NGTU";
    }
    else if ((0 == we_cmmn_strcmp_nc(value, "+46705167228@mms.telia.com")) &&
        (type == MSA_PB_TYPE_EMAIL))
    {
        name = "NGTU";
    }
    else if ((0 == we_cmmn_strcmp_nc(value, "+46666666666")) &&
        (type == MSA_PB_TYPE_PLMN))
    {
        name = "NGTUTSS";
    }
    else if ((0 == we_cmmn_strcmp_nc(value, "ngtu@tss.teleca.se")) &&
        (type == MSA_PB_TYPE_EMAIL))
    {
        name = "NGTUTSS";
    }
    else
    {
        result = MSA_PB_LOOKUP_RESULT_NO_HIT;
    }

    if (name != NULL)
    {
        if (NULL == (item = MSA_ALLOC(sizeof(MsaPbItem))))
        {
            (void)MSAc_pbLookupNameRsp(MSA_PB_LOOKUP_RESULT_ERROR, instanceId, 
                item);
            return;
        }
        memset(item, 0, sizeof(MsaPbItem));
        /*lint -e{605} */
        item->name      = (char *)name;
        item->type      = (MsaPbAddrType)type;
        item->addr      = value;
    }
    (void)MSAc_pbLookupNameRsp(result, instanceId, item);
    if (NULL != item)
    {
        /*lint -e{774} */
        MSA_FREE(item);
    }
}

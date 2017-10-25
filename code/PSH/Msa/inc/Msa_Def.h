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

/*! \file msa_def.h
 *  \brief Definitions for the MSA module.
 */

#ifndef _MSA_DEF_H
#define _MSA_DEF_H

/*--- Definitions/Declarations ---*/

/* Registry declarations  */

/*! The path for the registry settings for external settings
 */
#define MSA_REG_PATH_EXT                    "/EXTERNAL"

/*! Key for the MSISDN.
 */
#define MSA_REG_KEY_MSISDN                  "MSISDN"

/*! The path for the registry settings for the MSA
 */
#define MSA_REG_PATH_MMS                    "/MSA/mms"

/*! The path for the alignment registry settings for the MSA CR_17749
 */
#define MSA_REG_PATH_ALIGNMENT              "/MSA/render"

/*! Key for setting the "Save on send" feature.
 */
#define MSA_REG_KEY_SAVE_ON_SEND            "SavOnSnd"

/*! Key for setting default value for read report.
 */
#define MSA_REG_KEY_READ_REPORT             "ReadRpt"

/*! Key for setting default value for delivery report.
 */
#define MSA_REG_KEY_DELIVERY_REPORT         "DelRpt"

/*! Key for setting default value for priority.
 */
#define MSA_REG_KEY_PRIORITY                "Priority"

/*! Key for setting default value for slide duration.
 */
#define MSA_REG_KEY_SLIDE_DUR               "SlideDur"

/*! Key for setting default value for expiry time.
 */
#define MSA_REG_KEY_EXP_TIME                "ExpTime"

/*! Key for setting default value for reply charging.
 */
#define MSA_REG_KEY_REPLY_CHARGING          "RepChrg"

/*! Key for setting default value for reply charging deadline 
 */
#define MSA_REG_KEY_REPLY_CHARGING_DEADLINE "RepChDln"

/*! Key for setting default value for reply charging size
 */
#define MSA_REG_KEY_REPLY_CHARGING_SIZE     "RepChSiz"

/*! Key for setting default value for signature
 */
#define MSA_REG_KEY_USE_SIGNATURE           "UseSig"

/*! Key for setting default value for signature
 */
#define MSA_REG_KEY_SIGNATURE               "Signature"

 /*! Key for setting default value for the MMSC version.
 *  Only for testing purposes.
 */
#define MSA_REG_KEY_MMSC_VERSION            "MmscVer"

/*! Key setting for alignment in MSA CR_17749
 */
#define MSA_REG_KEY_TEXT_ALIGNMENT          "TexAlign"

 /*! Key for setting default value for the Sender visibility.
 */
#define MSA_REG_KEY_SENDER_VISIBILITY       "SendVsbl"

/*! Key for setting default value for the Send read report
 */
#define MSA_REG_KEY_SEND_READ_REPORT        "SndRdRp"

/*! Key for setting default value for the Delivery Time.
 */
#define MSA_REG_KEY_DELIVERY_TIME           "DlvrTime"

/*! \brief Global expiry times in seconds */

/*! Expiry date  one h in seconds.
 */
#define MSA_EXPIRY_1_HOUR_IN_SEC            3600

/*! Expiry date 12h in seconds.
 */
#define MSA_EXPIRY_12_HOURS_IN_SEC          43200

/*! Expiry date one day in seconds.
 */
#define MSA_EXPIRY_1_DAY_IN_SEC             86400

/*! Expiry date one week in seconds.
 */
#define MSA_EXPIRY_1_WEEK_IN_SEC            604800

/*! Expiry date maximum (1 year) in seconds.
 */
#define MSA_EXPIRY_MAX_IN_SEC               31449600 /* A year as max */

/*--- Types ---*/

/* !\enum MsaTextAlignment
 *  \brief Supported text alignment
*/
typedef enum
{
	MSA_TEXT_ALIGNMENT_LEFT,
	MSA_TEXT_ALIGNMENT_CENTER,
	MSA_TEXT_ALIGNMENT_RIGHT
}MsaTextAlignment;

/*! \enum MsaExpiryTimeType
 *  \brief The Expiry time for a message
 */
typedef enum
{
    MSA_EXPIRY_1_HOUR,      /*!< One hour */
    MSA_EXPIRY_12_HOURS,    /*!< 12 hours */
    MSA_EXPIRY_1_DAY,       /*!< One day */
    MSA_EXPIRY_1_WEEK,      /*!< One week */
    MSA_EXPIRY_MAX          /*!< The maximum expiry time */
}MsaExpiryTimeType;

/* !\enum MsaPriorityType
 *  \brief The message priority
 */
typedef enum
{
    MSA_PRIORITY_LOW,       /*!< Low priority */
    MSA_PRIORITY_NORMAL,    /*!< Normal priority */
    MSA_PRIORITY_HIGH       /*!< High priority */
}MsaPriorityType;

/* !\enum MsaReplyChargingType
 *  \brief Reply charging setting for message creation
 */
typedef enum
{
    MSA_REPLY_CHARGING_OFF,       /*!< Reply charging off */
    MSA_REPLY_CHARGING_ON,        /*!< Reply charging on */
    MSA_REPLY_CHARGING_TEXT_ONLY  /*!< Reply charging for text messages only */
}MsaReplyChargingType;

/* !\enum MsaVisibilityType
 *  \brief Sender visibility setting
 */
typedef enum
{
    MSA_SENDER_SHOW,            /*!< Show sender */
    MSA_SENDER_HIDE             /*!< Hide sender */
}MsaVisibilityType;

/* !\enum MsaReplyChargingDeadlineType
 *  \brief Reply charging deadline setting for message creation
 */
typedef enum
{
    MSA_REPLY_CHARGING_DEADLINE_1_DAY  = 0, /*!< Reply ch. dl 1 day */
    MSA_REPLY_CHARGING_DEADLINE_1_WEEK,     /*!< Reply ch. dl 1 week */
    MSA_REPLY_CHARGING_DEADLINE_MAX         /*!< Max reply ch. time 1yr */    
}MsaReplyChargingDeadlineType;

/* !\enum MsaReplyChargingSizeType
 *  \brief Reply charging size setting for message creation
 */
typedef enum
{
    MSA_REPLY_CHARGING_SIZE_1_KB  =  0, /*!< Reply charging max size 1 KB */
    MSA_REPLY_CHARGING_SIZE_10_KB,      /*!< Reply charging max size 10 KB */
    MSA_REPLY_CHARGING_SIZE_MAX         /*!< Unlimited reply charging size */
}MsaReplyChargingSizeType;

/* !\enum MsaRetrievalModeType
 *  \brief Retrieval mode setting when roaming 
 */
typedef enum
{
    MSA_RETRIEVAL_MODE_AUTOMATIC  = 0,  /*! < Immediate retrieval mode */
    MSA_RETRIEVAL_MODE_ALWAYS_ASK = 1   /*! < Delayed retrieval mode */
}MsaRetrievalModeType;


/* !\enum MsaRetrievalModeRoamingType
 *  \brief Retrieval mode setting when roaming 
 */
typedef enum
{
    MSA_RETRIEVAL_MODE_ROAMING_AUTOMATIC  = 0,  /*! < Use same setting as home */
    MSA_RETRIEVAL_MODE_ROAMING_ALWAYS_ASK = 1,  /*! < Delayed retrieval mode */
    MSA_RETRIEVAL_MODE_ROAMING_RESTRICTED = 2   /*! < Restricted mode */
}MsaRetrievalModeRoamingType;

/*! The default setting for Send read report. 0=On request 1=Always 2=Never*/

/* !\enum MsaSendReadReportType
 *  \brief Setting for how read report requests should be handled
 */
typedef enum
{   
    MSA_SEND_READ_REPORT_ON_REQUEST = 0,    /*!< Send read report on request */
    MSA_SEND_READ_REPORT_ALWAYS,            /*!< Always send read report */
    MSA_SEND_READ_REPORT_NEVER              /*!< Never send read report */
}MsaSendReadReportType;

/* !\enum MsaDeliveryTimeType
 *  \brief Setting for delivery time
 */
typedef enum
{
    MSA_DELIVERY_TIME_IMMEDIATE = 0,    /*!< Immediate */
    MSA_DELIVERY_TIME_1_HOUR,           /*!< 1 hour */
    MSA_DELIVERY_TIME_12_HOURS,         /*!< 12 hours */
    MSA_DELIVERY_TIME_24_HOURS          /*!< 24 hours */
}MsaDeliveryTimeType;

#endif /* _MSA_DEF_H */

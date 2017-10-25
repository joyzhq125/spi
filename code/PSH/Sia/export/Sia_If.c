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

#include "We_Lib.h"    /* WE: ANSI Standard libs allowed to use */
#include "We_Def.h"    /* WE: Global definitions */ 
#include "We_Core.h"   /* WE: System core call */ 
#include "We_Dcvt.h"   /* WE: Type conversion routines */ 
#include "We_Mem.h"    /* WE: Memory handling */
#include "We_Log.h"    /* WE: Signal logging */ 
#include "Sia_if.h"     /* SMA: Interface functions for SMA */
#include "Sasig.h"

/**********************************************************************
 * API invoked from other modules
 **********************************************************************/

/*!
 * \brief Starts SMA if not previously started.
 * 
 *****************************************************************************/
void SIAif_startInterface(void)
{
    WE_SIGNAL_REG_FUNCTIONS (WE_MODID_SIA, Sia_convert, Sia_destruct);
} /* SMAif_startInterface */

/*!
 * \brief Play a SMIL in a MMS message
 * 
 * \param sender Module ID of the calling module.
 * \param msgId The message ID of an MM containing a SMIL
 * \param fileType The type of the message to handle: 
 *                 MMS_SUFFIX_SEND_REQ or MMS_SUFFIX_MSG.
 * \return The result of the operation.
 *****************************************************************************/
SiaResult SIAif_playSmil(WE_UINT8 sender, WE_UINT32 msgId, 
    WE_UINT32 fileType)
{
    we_dcvt_t      cvt;
    void           *sig;
    void           *sigData;
    WE_UINT16      length;
    SiaResult       result = SIA_RESULT_OK;

    we_dcvt_init (&cvt, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
    if (!we_dcvt_uint32(&cvt, &msgId))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA,
        "%s(%d) SIAif_playSmil: Module=%d, MsgId=%d\n", __FILE__, __LINE__,
        sender, msgId));
        return SIA_RESULT_ERROR;
    } /* if */ 
    
    if (!we_dcvt_uint32(&cvt, &fileType))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA,
        "%s(%d) SIAif_playSmil: Module=%d, fileType=%d\n", __FILE__, __LINE__,
        sender, fileType));
        return SIA_RESULT_ERROR;
    } /* if */ 

    length = (WE_UINT16)cvt.pos;

    sig = WE_SIGNAL_CREATE (SIA_SIG_PLAY_SMIL_REQ, sender, 
        WE_MODID_SIA, length);
    sigData = WE_SIGNAL_GET_USER_DATA (sig, &length);
    we_dcvt_init (&cvt, WE_DCVT_ENCODE, sigData, length, sender);
    
    if (we_dcvt_uint32(&cvt, &msgId) && we_dcvt_uint32(&cvt, &fileType))
    {
        WE_SIGNAL_SEND (sig);
        result = SIA_RESULT_OK;
    } 
    else
    {
        /* error converting data */
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA,
        "%s(%d) SIAif_playSmil: Module=%d, MsgId=%d\n", __FILE__, __LINE__,
        sender, msgId));
        result = SIA_RESULT_ERROR;
    }/* if */
    return result;
 
} /* SIAif_playSmil */

/*!
 * \brief Convert signals owned by the SMA module.
 *        Conversion is from signal buffer to signal structs.
 * 
 * \param module Module id of the module using the convert function
 * \param signal Signal to convert
 * \param buffer Buffer to convert
 * \return <whatever is returned>
 *****************************************************************************/
void *Sia_convert (WE_UINT8 module, WE_UINT16 signal, void* buffer)
{
    we_dcvt_t  cvt_obj;
    void       *user_data;
    WE_UINT16  length;
    
    if ( buffer == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA,
        "%s(%d) Sia_convert: Invalid parameter\n", __FILE__, __LINE__));
        return NULL;
    } /* if */
        
    
    user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    if ((user_data == NULL) || (length == 0))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA,
        "%s(%d) Sia_convert: No data received.", __FILE__, __LINE__));
        return NULL;
    } /* if */
        
    we_dcvt_init (&cvt_obj, WE_DCVT_DECODE, user_data, length, module);
    
    switch (signal)
    {
    case SIA_SIG_PLAY_SMIL_REQ:
        {
            SiaPlaySmilReq *data = WE_MEM_ALLOCTYPE (module, SiaPlaySmilReq);
			if (data == NULL)
            {
                /* error allocating memoryt */
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA,
                "%s(%d) Sia_convert: Unable to allocate memory.", __FILE__, __LINE__));
            }
            else 
			{	
				memset(data, 0x00, sizeof(SiaPlaySmilReq));
			
				if (!Sia_cvt_SiaPlaySmilReq(&cvt_obj, data))
				{
					/* error converting data */
					WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA,
					"%s(%d) Sia_convert: Unable convert data.", __FILE__, __LINE__));
					WE_MEM_FREE( module, data);
					data = NULL;
				} /* if */
			} /* if */
            return data;
        }
    case SIA_SIG_PLAY_SMIL_RSP:
        {
            SiaPlaySmilRsp *data = WE_MEM_ALLOCTYPE (module, SiaPlaySmilRsp);
                       
			if ( data == NULL)
            {
                /* error allocating memoryt */
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA,
                "%s(%d) Sia_convert: Unable to allocate memory.", __FILE__, __LINE__));
            }
			else
			{
				memset(data, 0x00, sizeof(SiaPlaySmilRsp));

				if (!Sia_cvt_SiaPlaySmilRsp(&cvt_obj, data))
				{
					WE_MEM_FREE( module, data);
					data = NULL;
				} /* if */
			}
            return data;
        }
    default:
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA,
                "%s(%d) Sia_convert: Received unknown signal.", __FILE__, __LINE__));
            return NULL;
        }
    } /* switch */
} /* Sia_convert */

/*!
 * \brief Deallocate signal buffer
 * 
 * \param module The calling module identity
 * \param signal Signal identity
 * \param p The signal buffer
 *****************************************************************************/
void Sia_destruct (WE_UINT8 module, WE_UINT16 signal, void* p)
{
    we_dcvt_t cvt_obj;

    if (p == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA,
        "%s(%d) Sia_destruct: no signal data \n", __FILE__, __LINE__));
        return;
    } /* if */
    
    we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, module);
    switch (signal)
    {
    case SIA_SIG_PLAY_SMIL_REQ:
        {
            if ( !Sia_cvt_SiaPlaySmilReq( &cvt_obj, p))
            {
                /* error converting data */
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA,
                "%s(%d) Sia_convert: Unable convert.", __FILE__, __LINE__));
            } /* if */
            break;
        }
    case SIA_SIG_PLAY_SMIL_RSP:
        {
            if (!Sia_cvt_SiaPlaySmilRsp( &cvt_obj, p))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA,
                "%s(%d) Sia_convert: Unable convert.", __FILE__, __LINE__));
            } /* if */
            break;
        }
    default:
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA,
                "%s(%d) Sia_convert: Received unknown signal.", __FILE__, __LINE__));
            return;
        }
    } /* switch */
  WE_MEM_FREE (module, p);
} /* Sia_destruct */

/*
 * Converts a parameters into a signal buffer for SiaPlaySmilReq
 *
 * Please note: Doxygen dokumentation, see file description in header file.
 *****************************************************************************/
int Sia_cvt_SiaPlaySmilReq(we_dcvt_t *obj, SiaPlaySmilReq *data)
{
    if (NULL == data)
    {
        return FALSE;
    }
    if (!we_dcvt_uint32(obj, &data->msgId) || 
        !we_dcvt_uint32(obj, &data->userData))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIA, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */

    return TRUE; 
} /* Sia_cvt_SiaPlaySmilReq */

/*
 * Converts a parameters into a signal buffer for SiaPlaySmilRsp
 *
 * Please note: Doxygen dokumentation, see file description in header file.
 *****************************************************************************/
int Sia_cvt_SiaPlaySmilRsp(we_dcvt_t *obj, SiaPlaySmilRsp *data)
{
    WE_UINT32 result;
    
    if (NULL == data)
    {
        return FALSE;
    }
    result = (WE_UINT32)data->result; 
    
    if (!we_dcvt_uint32(obj, &result) || !we_dcvt_uint32(obj, &data->userData))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIA, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } /* if */
    
    data->result = (SiaResult)result;
    return TRUE; 
} /* Sia_cvt_SiaPlaySmilRsp */


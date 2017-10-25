#ifndef GIF_TO_BMP_H
#define GIF_TO_BMP_H
/*==================================================================================================

    HEADER NAME : jif_to_bmp.h

    GENERAL DESCRIPTION
        This header file for the functions for unicode encoding string.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   ------------------------------------------------------
    03/24/2002   Andyli                         Initial Creation
    07/26/2003   lindawang        P000763       Fix some wap draw image problem.
    08/14/2003   Tree Zhan        P001241       Improve the speed of encoding
    08/27/3004   shifeng          P001388       move locally relative code to gif_to_bmp.c
    10/14/2003   linda wang       C001806       Add function of setting download image to desktop.

====================================================================================================
    INCLUDE FILES
==================================================================================================*/
#include    "OPUS_typedef.h"


/*==================================================================================================
    CONSTANTS
==================================================================================================*/
#define GIF_ERROR           -1
#define GIF_TERMINATED      0
#define GIF_SUCCESS         1
#define GIF_ONLY_ONE_FRAME  2


/*==================================================================================================
    MACROS
==================================================================================================*/

/*==================================================================================================
    ENUMERATIONS
==================================================================================================*/

/*==================================================================================================
    DATA STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
    GLOBAL VARIABLES DECLARATIONS
==================================================================================================*/

/*==================================================================================================
    FUNCTION PROTOTYPES
==================================================================================================*/



/*!
 * \brief 	 Get the GIF file header information.
 *
 * You should call this fuction firstly.
 *
 * \return  if anzlizing failed, return 0. other return the header size.
 */
OP_UINT32 GIF_AnalizeFileHeader
(
    OP_UINT32 handle,       /*!< a unique id to register the given gif file */
    OP_UINT8  *pFileContent,/*!< gif file data */
    OP_UINT32 fileSize,     /*!< size of gif file data */
    OP_UINT16 *colorPallet
);



/*!
 *  \brief 	 This function provide caller to get a frame of gif data.
 *
 *  \retval GIF_ERROR
 *  \retval GIF_TERMINATED
 *  \retval GIF_SUCCESS
 *  \retval GIF_ONLY_ONE_FRAME
 */
int GIF_LoadData
(
    OP_UINT32     handle,   /*!< a unique id to identified the given gif file */
    RM_BITMAP_T   *pLcdBuf, /*!< the destination data buffer */
    OP_UINT16     *delay    /*!< delay time of this frame of picture in millisecond */
);



/*!
 *  \brief   unregister the given gif file and release source allocated.
 */
void GIF_Terminate
(
    int       handle        /*!< a unique id to identified the given gif file */
);


/*================================================================================================*/

#endif /* GIF_TO_BMP_H */

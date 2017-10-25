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
/*
 * ubs_cfg.h
 *
 * Created by Klas Hermodsson, Thu May 22 15:50:11 2003.
 *
 * Revision history:
 * 2003-09-22 (MADE): removed define UBS_CONFIG_NOTIFY_SHELL
 *
 */
#ifndef UBS_CFG_H
#define UBS_CFG_H


/*
 * UBS_MSG_TYPE_OWNER_ARRAY defines an array where the msg type defines 
 * (see ubs_if.h UBS_MSG_TYPE_... and ubs_ecfg.h) is the index into the array.
 * The value at that index is the module wid of the owner module.
 * A value of -1 msans that the msg type is not part of the unified inbox (i.e. no owner).
 * A value of WE_MODID_UBS msans that UBS is the owner and that the msg type is 
 * stored in the UBS.
 *
 * NOTE that the number of entries in this array MUST be UBS_NBR_OF_MSG_TYPES!
 */
#define UBS_MSG_TYPE_OWNER_ARRAY { \
                    WE_MODID_UBS, \
                    WE_MODID_UBS, \
                    WE_MODID_MMS, \
                    -1, \
        }

/*
 *  The following defines are used in conditions which must be met to start 
 *  a defrag of .ndx files. 
 */
#define UBS_DEFRAG_CONSTANT_1     5
#define UBS_DEFRAG_CONSTANT_2     5
#define UBS_DEFRAG_TRIGG          10

/*
 *  The following defines are limits to the number of messages allowed in UBS 
 *  and to maximum index file size. When these limits are exceeded, the 
 *  integration will be informed about the error. 
 */
#define UBS_MAX_NBR_OF_MSGS       20
#define UBS_MAX_INDEX_FILE_SIZE   2000         /* in bytes */

#endif      /*UBS_CFG_H*/

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







 

#ifndef UBS_MSIN_H
#define UBS_MSIN_H

#ifndef UBS_SIG_H
#include "Ubs_Sig.h"
#endif










#define UBS_MSG_STORE_SIG_GET_NBR_OF_MSGS       (UBS_MODULE_MSG_STORE + 1)
#define UBS_MSG_STORE_SIG_GET_MSG_LIST          (UBS_MODULE_MSG_STORE + 2)
#define UBS_MSG_STORE_SIG_GET_MSG               (UBS_MODULE_MSG_STORE + 3)
#define UBS_MSG_STORE_SIG_GET_FULL_MSG          (UBS_MODULE_MSG_STORE + 4)
#define UBS_MSG_STORE_SIG_DELETE_MSG            (UBS_MODULE_MSG_STORE + 5)
#define UBS_MSG_STORE_SIG_CHANGE_MSG            (UBS_MODULE_MSG_STORE + 6)
#define UBS_MSG_STORE_SIG_CREATE_MSG            (UBS_MODULE_MSG_STORE + 7)
#define UBS_MSG_STORE_SIG_FILE_NOTIFY           (UBS_MODULE_MSG_STORE + 8)
#define UBS_MSG_STORE_SIG_SET_MSG_PROPERTY      (UBS_MODULE_MSG_STORE + 9)






#define UBS_MSG_STORE_MAX_PATH_LEN          64

#define UBS_MSG_STORE_MSG_DATA_FILE         "/ubs/%i/%lu.dat"


#define UBS_MSG_STORE_MSG_HIDDEN_PROP_FILE  "/ubs/%i/%lu.hpf"

#define UBS_MSG_STORE_FOLDER_MSG_TYPE_INDEX_FILE "/ubs/%i.ndx"


















void
ubs_msg_store_init (void);

void
ubs_msg_store_terminate (void);

void
ubs_msg_store_terminate_done (void);







#endif      /*UBS_MSIN_H*/

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
 * We_Afir.h 
 *
 * Attribute File Interface Refsrc
 * This file defines functions which are implemented in refsrc.
 *
 * Created by Bo-Göran Wallner
 *
 * Revision  history:
 */

#ifndef _we_afir_h
#define _we_afir_h

#ifndef _we_cfg_h
#include "We_Cfg.h"
#endif

#ifdef WE_CFG_AFI_REDIRECT_INDEX

#ifndef _we_def_h
#include "We_Def.h"
#endif


/**********************************************************************
 * Redirecton of index files (implementation in refsrc\We_Afir.c)
 **********************************************************************/
/* Redirect path for index files to a user defines path.
 *
 * path        The source path where the file is stored, typically
 *             a User Root Folder
 *
 * Returns:    The path where the belonging index.ix and index.fr
 *             are stored. This string must be freed by caller.
 *
 *             
 */
char * 
we_afir_idx_redirect_path(WE_UINT8 modId, const char *path);

#endif /* WE_CFG_AFI_REDIRECT_INDEX */

#endif

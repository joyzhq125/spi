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
 * We_Xmim.c
 * 
 * Additional Mime Type Support
 *
 *
 * Place in this file all additional Mime Types that should be
 * possible to recognise. As an example, two  additional 
 * mime types are included. These can be removed
 * if not required.
 * If no additional mime type support is required, this
 * whole file can be left empty.
 */
#include "We_Def.h"
#include "We_Xmim.h"


/*
 * This is the table that holds information about all additional
 * mime types that are supported.
 *
 * The first field, 'value', should hold a registered integer
 * value if such are available else a number int the range 0x010000 
 * to 0x7FFFFF and not in conflict with any other type defined 
 * in We_Hdr.h must be used.
 *
 * The second field, 'mime', should hold a string with the mime type.
 *
 * The third field, 'extension', should hold a string with the extension
 * to be associated with this mime type.
 *
 */
#if WE_NUMBER_OF_ADDITIONAL_MIME_TYPES > 0

const we_xmim_entry_t we_xmim_table[WE_NUMBER_OF_ADDITIONAL_MIME_TYPES] = {
  {0x010000, "mime_type", "extension"},
};

#endif

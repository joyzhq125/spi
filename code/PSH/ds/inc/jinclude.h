
/*==================================================================================================

    MODULE NAME : jinclude.h

    GENERAL DESCRIPTION
        functions for unicode encoding string.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    05/24/2004   penghaibo        p005629       Initial Creation(base on IJG 6.2B)
    06/16/2004   penghaibo        p006216       add more error handle use longjmp

    Self-documenting Code
    Describe/explain low-level design of this module and/or group of funtions and/or specific
    funtion that are hard to understand by reading code and thus requires detail description.
    Free format !
        
====================================================================================================*/


/* Include auto-config file to find out which system include files we need. */

#include "jconfig.h"		/* auto configuration options */
#define JCONFIG_INCLUDED	/* so that jpeglib.h doesn't do it again */
/*
 * We need the NULL macro and size_t typedef.
 * On an ANSI-conforming system it is sufficient to include <stddef.h>.
 * Otherwise, we get them from <stdlib.h> or <stdio.h>; we may have to
 * pull in <sys/types.h> as well.
 * Note that the core JPEG library does not require <stdio.h>;
 * only the default error handler and data source/destination modules do.
 * But we must pull it in because of the references to FILE in jpeglib.h.
 * You can remove those references if you want to compile without <stdio.h>.
 */

#ifdef WIN32
#include    "windows.h"
#include    "portab_new.h"
#else
#include    "portab.h"
#endif

#include    "rm_include.h"
#include    "SP_sysutils.h"

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef NEED_SYS_TYPES_H
#include <sys/types.h>
#endif

#include <stdio.h>
#include <setjmp.h>
/*
 * We need memory copying and zeroing functions, plus strncpy().
 * ANSI and System V implementations declare these in <string.h>.
 * BSD doesn't have the mem() functions, but it does have bcopy()/bzero().
 * Some systems may declare memset and memcpy in <memory.h>.
 *
 * NOTE: we assume the size parameters to these functions are of type size_t.
 * Change the casts in these macros if not!
 */

#ifdef NEED_BSD_STRINGS

#include <strings.h>
#define MEMZERO(target,size)	bzero((void *)(target), (size_t)(size))
#define MEMCOPY(dest,src,size)	bcopy((const void *)(src), (void *)(dest), (size_t)(size))

#else /* not BSD, assume ANSI/SysV string lib */

#define MEMZERO(target,size)	op_memset((void *)(target), 0, (size_t)(size))
#define MEMCOPY(dest,src,size)	op_memcpy((void *)(dest), (const void *)(src), (size_t)(size))

#endif

/*
 * In ANSI C, and indeed any rational implementation, size_t is also the
 * type returned by sizeof().  However, it seems there are some irrational
 * implementations out there, in which sizeof() returns an int even though
 * size_t is defined as long or unsigned long.  To ensure consistent results
 * we always use this SIZEOF() macro in place of using sizeof() directly.
 */

#define SIZEOF(object)	((size_t) sizeof(object))

/*
 * The modules that use fread() and fwrite() always invoke them through
 * these macros.  On some systems you may need to twiddle the argument casts.
 * CAUTION: argument order is different from underlying functions!
 */

#ifdef BACKSTORE_SUPPORTED
    #define JFREAD(file,buf,sizeofbuf)  \
      ((size_t) fread((void *) (buf), (size_t) 1, (size_t) (sizeofbuf), (file)))
    #define JFWRITE(file,buf,sizeofbuf)  \
      ((size_t) fwrite((const void *) (buf), (size_t) 1, (size_t) (sizeofbuf), (file)))
#endif
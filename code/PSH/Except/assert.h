/* $Id: H:/drh/idioms/book/RCS/except.doc,v 1.10 1997/02/21 19:43:55 drh Exp $ */
#undef assert
#ifdef NDEBUG
#define assert(e) ((void)0)
#else
#include "except.h"
extern void WE_assert(int e);
#define WE_assert(e) ((void)((e)||(WE_RAISE(Assert_Failed),0)))
#endif

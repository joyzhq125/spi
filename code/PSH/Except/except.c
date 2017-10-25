#ifdef WIN32
#include <stdlib.h>
#include <stdio.h>
#endif
#include "assert.h"
#include "except.h"

#define T WE_Except_T

#ifdef WIN32
__declspec(thread)
#endif

#ifndef NULL
#define NULL (void*)0
#endif

#ifdef WIN32
#define WE_fprintf fprintf
#else
#define WE_fprintf
#endif
WE_Except_Frame *Except_stack = NULL;
void WE_Except_raise(const T *e, const char *file,	int line)
{
	WE_Except_Frame *p = Except_stack;
	WE_assert(e);
	if (p == NULL)
	{
		WE_fprintf(stderr, "Uncaught exception");
		if (e->reason)
		{
			WE_fprintf(stderr, " %s", e->reason);
		}
		else
		{
			WE_fprintf(stderr, " at 0x%p", e);
		}
		if (file && line > 0)
		{
			WE_fprintf(stderr, " raised at %s:%d\n", file, line);
		}
		WE_fprintf(stderr, "aborting...\n");
		fflush(stderr);
		abort();
	}
	p->exception = e;
	p->file = file;
	p->line = line;
	Except_stack = Except_stack->prev;
	longjmp(p->env, Except_raised);
}

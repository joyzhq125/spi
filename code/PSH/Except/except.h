/* $Id: H:/drh/idioms/book/RCS/except.doc,v 1.10 1997/02/21 19:43:55 drh Exp $ */
#ifndef WE_EXCEPT_INCLUDED
#define WE_EXCEPT_INCLUDED
#include <setjmp.h>
#define T WE_Except_T
typedef struct T {
	char *reason;
} T;
typedef struct WE_Except_Frame WE_Except_Frame;
struct WE_Except_Frame {
	WE_Except_Frame *prev;
	jmp_buf env;
	const char *file;
	int line;
	const T *exception;
};
enum { Except_entered=0, Except_raised,
       Except_handled,   Except_finalized };
#ifdef WIN32
__declspec(thread)
#endif
extern WE_Except_Frame *Except_stack;
extern const WE_Except_T Assert_Failed;
void WE_Except_raise(const T *e, const char *file,int line);
#define WE_RAISE(e) WE_Except_raise(&(e), __FILE__, __LINE__)
#define WE_RERAISE WE_Except_raise(Except_frame.exception, \
	Except_frame.file, Except_frame.line)
#define RETURN switch (Except_stack = Except_stack->prev,0) default: return
#define WE_TRY \
do\
{\
	volatile int Except_flag;\
	WE_Except_Frame Except_frame;\
	Except_frame.prev = Except_stack;\
	Except_stack = &Except_frame;\
	Except_flag = setjmp(Except_frame.env);\
	if(Except_flag == Except_entered)\
	{

#define WE_EXCEPT(e) \
		if (Except_flag == Except_entered)\
		{\
			Except_stack = Except_stack->prev; \
		}\
	}\
	else if (Except_frame.exception == &(e)) \
	{ \
		Except_flag = Except_handled;

#define WE_ELSE \
		if (Except_flag == Except_entered)\
		{\
			Except_stack = Except_stack->prev; \
		}\
	}\
	else\
	{\
		Except_flag = Except_handled;

#define WE_FINALLY \
		if (Except_flag == Except_entered)\
		{\
			Except_stack = Except_stack->prev; \
		}\
	}\
	{\
		if (Except_flag == Except_entered) \
		{\
			Except_flag = Except_finalized;\
		}

#define WE_END_TRY \
		if (Except_flag == Except_entered) \
		{\
			Except_stack = Except_stack->prev; \
		}\
	}\
	if (Except_flag == Except_raised) \
	{\
		WE_RERAISE; \
	}\
} while (0)

#undef T

#endif

/*
 *	Co-routine library
 *	Copyright
 *		(C) 2006 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

/* A stack */

#define STACK_SIZE 32768 /* libcoro multiplies this by sizeof(void*) for us */

#include "libcoro.h"

struct stack {
	struct stack *next;			/* Next free stack */
	Coroutine *chain;			/* Resume this coroutine in place of caller when
	                                           the coroutine associated with the stack returns.  The chained
	                                           coroutine will eventually continue caller. */
	Coroutine *caller;			/* Most recent coroutine which continued us (the
	                                           calling co-routine if this is the first execution) */
	int (*func)(va_list args);		/* Function to call */
	va_list args;

	/* initial PC, SP to use for call to func */
	coro_context cothread;
};

/* A suspended co-routine */

struct coroutine {
	struct stack *stack;		/* To restore current stack */
	Obj saved_obj_stack;		/* To restore obj stack */
	int override;			/* To use override_val instead of normal return value */
	int override_val;
};

/* Call a function as a co-routine (it runs with its own stack).  co_call
 * returns when the specified function returns (in which case the function's
 * return value is returned) or when the co-routine yields (in which case
 * the second argument to co_yield() is returned.
 */

int co_call(int (*func)(va_list args), ...);

/* Suspend current co-routine and return to calling co-routine with
 * specified return value.  t points to a Coroutine structure which gets
 * filled in with data about the suspended Coroutine.  The address of this
 * structure is used by co_resume to resume the suspended co-routine. */

int co_yield(Coroutine *t, int val);

/* Suspend the current co-routine and resume a previously suspended
 * co-routine.  co_resume() returns (and the co-routine which called
 * co_resume() is resumed) when the specified co-routine either completes
 * (the function specified in co_call() returns) or yields (it calls
 * co_yield).  The return value is the second argument to co_yield or the
 * return value of the function specified in co_call().
 */

int co_resume(Coroutine *t, int val);

/* Suspend current task: that is, suspend current co-routines and its chain
 * chain of invoking co-routines all the way back to the top level.  The top
 * level is resumed with the given return value.  The task is saved in t. 
 * The task can later be scheduled for continuation.
 */

int co_suspend(Coroutine *t, int val);

/* Schedule a task to resume after the current task completes: in other
 * words, the task is continued with the given return value after the next
 * time we return to the top level.  This function returns immediately:
 * it just schedules the task for execution.
 */

void co_sched(Coroutine *t, int val);

/* Suspend current task and stick a pointer to it in 'u', which should point
 * to a previously yielded co-routine.  The top level is resumed with the
 * return value given in 'val'.  The task will be scheduled when the
 * co-routine in 'u' returns.  It will be resumed with u's return value.
 * Note that the scheduling happens on return, and not on yield.
 *
 * This function is for macro query suspend, where a dialog is to get all of
 * its user input from the user and not from a macro.  When the dialog
 * function is done (which is indicated by the co-routine u returning) the
 * macro player is scheduled to continue.
 */

int co_query_suspend(Coroutine *u, int val);

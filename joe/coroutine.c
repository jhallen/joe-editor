/*
 *	Co-routine library
 *	Copyright
 *		(C) 2006 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */
#include "types.h"

static struct stack *current_stack;	/* Current stack */
static struct stack *free_stacks;	/* Free stacks */

#ifndef USE_UCONTEXT

static jmp_buf create_stk;	/* longjmp here to create another stack */
static jmp_buf rtn_create_stk;	/* Return address for stack creation longjmp call */

/* Record current stack location */

static void make_space();

static void record_stack()
{
	int create = 1;
	int rtn = 0;
	for (;;) {
		/* Save PC and SP address in jmp_buf */
		if (setjmp(current_stack->go)) {
			/* Run scheduler in this stack.  It will return with a task to continue */
			rtn = current_stack->func(current_stack->args); /* Call function... */
			if (current_stack->chain) {
				co_sched(current_stack->chain, rtn);
				current_stack->chain = 0;
			}
		} else if (!create) {
			Coroutine *t;
			/* Free this stack */
			t = current_stack->caller;
			if (t->override)
				rtn = t->override_val;
			current_stack->next = free_stacks;
			free_stacks = current_stack;
			/* Continue task */
			current_stack = t->stack;
			/* Adding 10 because we want to be able to pass 0 and
			   -1 to user, but 0 has special meaning for setjmp */
			longjmp(t->cont, rtn + 10);
		} else {
			create = 0;
			/* Create the stack (doesn't return) */
			make_space();
		}
	}
}

/* Make space on the stack */

static void make_space()
{
	char buf[STACK_SIZE];
	/* Make sure compiler does not optimize out buf */
	current_stack->stk = buf;
	/* We've created the space, jump back to caller. */
	if (!setjmp(create_stk)) {
		longjmp(rtn_create_stk, 1);
	} else {
		/* When someone longjmps to create_stk, create another stack... */
		record_stack();
	}
}

#else

/* Execute function and resume calling co-routine */

static int rtval;

static void call_it()
{
	Coroutine *t;
	/* Execute function */
	rtval = current_stack->func(current_stack->args);
	/* There's a chain... */
	if (current_stack->chain) {
		/* Schedule it... */
		co_sched(current_stack->chain, rtval);
		current_stack->chain = 0;
	}

	/* Free stack and resume caller */
	t = current_stack->caller;
	if (t->override)
		rtval = t->override_val;
	current_stack->next = free_stacks;
	free_stacks = current_stack;
	current_stack = t->stack;
	setcontext(t->uc);
}

#endif

/* Allocate a stack */

/* Only some versions of solaris have makecontext bugs... */
/* This fix is from sthreads.c of Michal Trojnara's stunnel program */

/* This fix doesn't appear to work with gcc, as these symbols aren't defined */
#if defined(CPU_SPARC) && ( \
  defined(OS_SOLARIS2_0) || \
  defined(OS_SOLARIS2_1) || \
  defined(OS_SOLARIS2_2) || \
  defined(OS_SOLARIS2_3) || \
  defined(OS_SOLARIS2_4) || \
  defined(OS_SOLARIS2_5) || \
  defined(OS_SOLARIS2_6) || \
  defined(OS_SOLARIS2_7) || \
  defined(OS_SOLARIS2_8))

#define SUN_BUG 1

#else

#define SUN_BUG 0

#endif

static struct stack *mkstack()
{
	struct stack *stack;
	if (free_stacks) {
		stack = free_stacks;
		free_stacks = stack->next;
		stack->caller = 0;
		stack->chain = 0;
#ifdef USE_UCONTEXT
		makecontext(stack->uc, call_it, SUN_BUG);
#endif
		return stack;
	}
	stack = (struct stack *)malloc(sizeof(struct stack));
	stack->caller = 0;
	stack->chain = 0;
#ifdef USE_UCONTEXT
	getcontext(stack->uc);
	stack->uc->uc_link = 0;
	stack->uc->uc_stack.ss_size = STACK_SIZE;
	stack->uc->uc_stack.ss_sp = malloc(STACK_SIZE);
#if defined(__sgi) || SUN_BUG==1
	stack->uc->uc_stack.ss_sp = (char *)stack->uc->uc_stack.ss_sp + STACK_SIZE - 8;
#endif
	stack->uc->uc_stack.ss_flags = 0;
	makecontext(stack->uc, call_it, SUN_BUG);
	return stack;
#else
	current_stack = stack;
	if (!setjmp(rtn_create_stk))
		longjmp(create_stk, 1);
	else {
		return stack;
	}
#endif
}

/* Suspend current co-routine and return to caller */

int co_yield(Coroutine *t, int val)
{
#ifdef USE_UCONTEXT
	Coroutine *n;

	/* Save current stack */
	t->override = 0;
	t->override_val = 0;
	t->stack = current_stack;

	/* Save object stack: create one which gets immediately destroyed
	   by return to co_call or co_resume. */
	t->saved_obj_stack = get_obj_stack();

	/* Return to creator */
	n = current_stack->caller;
	current_stack->caller = 0;

	current_stack = n->stack;

	/* Give return value to creator */
	if (n->override)
		rtval = n->override_val;
	else
		rtval = val;

	/* Switch */
	swapcontext(t->uc, n->uc);

	/* Somebody continued us... */
	set_obj_stack(t->saved_obj_stack);

	return rtval;
#else
	int rtn;

	t->override = 0;
	t->override_val = 0;
	/* Save current stack */
	t->stack = current_stack;

	/* Save object stack */
	t->saved_obj_stack = get_obj_stack();

	/* Save current context */
	if ((rtn = setjmp(t->cont))) {
		/* Somebody continued us... */
		set_obj_stack(t->saved_obj_stack);
		return rtn - 10; /* 10 added to longjmp */
	} else {
		/* Return to creator */
		t = current_stack->caller;
		/* This co-routine can not jump back ever again */
		current_stack->caller = 0;

		current_stack = t->stack;
		if (t->override)
			val = t->override_val;

		/* Go */
		longjmp(t->cont, val + 10);
	}
#endif
}

/* Suspend current co-routine and resume another */

int co_resume(Coroutine *t,int val)
{
	struct stack *i;
	Coroutine self[1];
	self->override = 0;
	self->override_val = 0;
#ifdef USE_UCONTEXT
	/* Save current stack */
	self->stack = current_stack;

	/* Save object stack.  Create new one which is detroyed by return
	   to co_yield. */
	self->saved_obj_stack = get_obj_stack();

	/* Resume specified coroutine */
	current_stack = t->stack;

	/* Who to resume when coroutine returns */
	for (i = current_stack; i->caller; i = i->caller->stack);
	i->caller = self;
	/* This is OK if nobody resumes a task: current_stack->caller = self; */

	/* Give return value to co_yield() */
	if (t->override)
		rtval = t->override_val;
	else
		rtval = val;

	/* Switch */
	swapcontext(self->uc, t->uc);

	/* Somebody continued us... */
	set_obj_stack(self->saved_obj_stack);

	return rtval;
#else
	int rtn;

	/* Save current stack */
	self->stack = current_stack;

	/* Save object stack */
	self->saved_obj_stack = get_obj_stack();

	/* Save current context */
	if ((rtn = setjmp(self->cont))) {
		/* Somebody continued us... */
		set_obj_stack(self->saved_obj_stack);
		return rtn - 10; /* 10 added to longjmp */
	} else {
		/* Continue specified */
		current_stack = t->stack;
		
		/* Who to return to */
		for (i = current_stack; i->caller; i = i->caller->stack);
		i->caller = self;
		/* This is OK if nobody resumes a task: current_stack->caller = self; */

		if (t->override)
			val = t->override_val;

		/* Go */
		longjmp(t->cont, val + 10);
	}
#endif
}

/* Suspend current co-routine and yield to top */

int co_suspend(Coroutine *t,int val)
{
	Coroutine *v, *n;

#ifdef USE_UCONTEXT
	/* Save current stack */
	t->stack = current_stack;
	t->override = 0;
	t->override_val = 0;

	/* Save object stack.  Create new one which is detroyed by return
	   to co_yield. */
	t->saved_obj_stack = get_obj_stack();

	/* Find top level */
	for (v = t; v->stack->caller->stack->caller; v = v->stack->caller);
	n = v->stack->caller; /* n points to top-most coroutine */
	v->stack->caller = 0;

	/* Resume specified coroutine */
	current_stack = n->stack;

	/* Give return value to co_yield() */
	rtval = val;

	/* Switch */
	swapcontext(t->uc, n->uc);

	/* Somebody continued us... */
	set_obj_stack(t->saved_obj_stack);

	return rtval;
#else
	int rtn;

	/* Save current stack */
	t->stack = current_stack;
	t->override = 0;
	t->override_val = 0;

	/* Save object stack */
	t->saved_obj_stack = get_obj_stack();

	/* Save current context */
	if ((rtn = setjmp(t->cont))) {
		/* Somebody continued us... */
		set_obj_stack(t->saved_obj_stack);
		return rtn - 10; /* 10 added to longjmp */
	} else {
		/* Find top level */
		for (v = t; v->stack->caller->stack->caller; v = v->stack->caller);
		n = v->stack->caller; /* n points to top-most coroutine */
		v->stack->caller = 0;

		/* Continue specified */
		current_stack = n->stack;

		/* Go */
		longjmp(n->cont, rtn + 10);
	}
#endif
}

/* Schedule a task to resume after this one completes */

void co_sched(Coroutine *t, int val)
{
	Coroutine *u;
	Coroutine *v;
	Coroutine self[1];

	/* Find top-level */
	self->stack = current_stack;
	for (v = self; v->stack->caller->stack->caller; v = v->stack->caller);
	/* v->stack->caller is top-most coroutine */

	/* Find end of t */
	for (u = t; u->stack->caller; u = u->stack->caller);

	/* Insert t just before top */
	u->stack->caller = v->stack->caller;
	v->stack->caller = t;
	t->override = 1;
	t->override_val = val;
}

/* Suspend current co-routine and resume top */

int co_query_suspend(Coroutine *u,int val)
{
	Coroutine *t, *v;
	Coroutine self[1];
	self->override = 0;
	self->override_val =0;

#ifdef USE_UCONTEXT
	/* Save current stack */
	self->stack = current_stack;

	/* Save object stack.  Create new one which is detroyed by return
	   to co_yield. */
	self->saved_obj_stack = get_obj_stack();

	for (v = self; v->stack->caller->stack->caller; v = v->stack->caller);
	t = v->stack->caller; /* t points to top-most coroutine */
	v->stack->caller = 0;

	/* Resume specified coroutine */
	current_stack = t->stack;

	/* u resumes us when he's done. */
	u->stack->chain = self;

	/* Give return value to co_yield() */
	rtval = val;

	/* Switch */
	swapcontext(self->uc, t->uc);

	/* Somebody continued us... */
	set_obj_stack(self->saved_obj_stack);

	return rtval;
#else
	int rtn;

	/* Save current stack */
	self->stack = current_stack;

	/* Save object stack */
	self->saved_obj_stack = get_obj_stack();

	for (v = self; v->stack->caller->stack->caller; v = v->stack->caller);
	t = v->stack->caller; /* t points to top-most coroutine */
	v->stack->caller = 0;

	/* Save current context */
	if ((rtn = setjmp(self->cont))) {
		/* Somebody continued us... */
		set_obj_stack(self->saved_obj_stack);
		return rtn - 10; /* 10 added to longjmp */
	} else {
		/* Continue specified */
		current_stack = t->stack;
		
		/* u resumes us when he's done */
		u->stack->chain = self;

		/* Go */
		longjmp(t->cont, rtn + 10);
	}
#endif
}

/* Call a function as a co-routine */

int co_call(int (*func)(va_list args), ...)
{
	Coroutine self[1];
	va_list ap;
	self->override = 0;
	self->override_val = 0;

#ifdef USE_UCONTEXT
	va_start(ap, func);

	if (!current_stack) {
		current_stack = (struct stack *)malloc(sizeof(struct stack));
		current_stack->caller = 0;
		current_stack->chain = 0;
	}

	/* Save current stack */
	self->stack = current_stack;

	/* Save object stack, create new one for this call */
	self->saved_obj_stack = get_obj_stack();

	/* Allocate stack for co-routine */
	current_stack = mkstack();

	/* Set function to call in stack */
	current_stack->func = func;
#ifdef va_copy
	va_copy(current_stack->args, ap);
#else
	current_stack->args = ap;
#endif

	/* Who to resume when function returns */
	current_stack->caller = self;

	/* Switch */
	swapcontext(self->uc, current_stack->uc);

	/* Somebody continued us... */
	/* Free object stack we created above, restore original. */
	set_obj_stack(self->saved_obj_stack);

	va_end(ap);

	return rtval;
#else
	int rtn;
	va_start(ap, func);

	if (!current_stack) {
		/* Startup... record main stack */
		current_stack = (struct stack *)malloc(sizeof(struct stack));
		current_stack->caller = 0;
		current_stack->chain = 0;
		if (!setjmp(rtn_create_stk))
			record_stack();
	}

	self->stack = current_stack;
	self->saved_obj_stack = get_obj_stack();

	/* Allocate stack for co-routine */
	current_stack = mkstack();

	/* Set function to call in stack */
	current_stack->func = func;
#ifdef va_copy
	va_copy(current_stack->args, ap);
#else
	current_stack->args = ap;
#endif

	/* Set where we return to */
	current_stack->caller = self;

	/* Save current context */
	if ((rtn = setjmp(self->cont))) {
		/* Somebody continued us... */
		set_obj_stack(self->saved_obj_stack);
		va_end(ap);
		return rtn - 10; /* 10 added to longjmp */
	} else {
		/* Go */
		longjmp(current_stack->go, 1);
	}
#endif
}

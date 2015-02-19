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

/* Execute function and resume calling co-routine */

static int rtval;

static void call_it()
{
	for (;;) {
		Coroutine *t;
		
		rtval = current_stack->func(current_stack->args);
		if (current_stack->chain) {
			co_sched(current_stack->chain, rtval);
			current_stack->chain = 0;
		}
		
		t = current_stack->caller;
		if (t->override) {
			rtval = t->override_val;
		}
		
		current_stack->next = free_stacks;
		free_stacks = current_stack;
		current_stack = t->stack;
		coro_transfer(&free_stacks->cothread, &t->stack->cothread);
	}
}

/* Allocate a stack */

static struct stack *mkstack()
{
	struct stack *stack;
	struct coro_stack stkparams;
	
	if (free_stacks) {
		stack = free_stacks;
		free_stacks = stack->next;
		stack->caller = 0;
		stack->chain = 0;
		return stack;
	}
	
	stack = (struct stack *)malloc(sizeof(struct stack));
	stack->caller = 0;
	stack->chain = 0;
	
	coro_stack_alloc(&stkparams, STACK_SIZE);
	coro_create(&stack->cothread, call_it, NULL, stkparams.sptr, stkparams.ssze);
	
	return stack;
}

/* Suspend current co-routine and return to caller */

int co_yield(Coroutine *t, int val)
{
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
	coro_transfer(&t->stack->cothread, &n->stack->cothread);

	/* Somebody continued us... */
	set_obj_stack(t->saved_obj_stack);

	return rtval;
}

/* Suspend current co-routine and resume another */

int co_resume(Coroutine *t,int val)
{
	struct stack *i;
	Coroutine self[1];
	self->override = 0;
	self->override_val = 0;
	
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
	coro_transfer(&self->stack->cothread, &t->stack->cothread);

	/* Somebody continued us... */
	set_obj_stack(self->saved_obj_stack);

	return rtval;
}

/* Suspend current co-routine and yield to top */

int co_suspend(Coroutine *t,int val)
{
	Coroutine *v, *n;

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
	coro_transfer(&t->stack->cothread, &n->stack->cothread);

	/* Somebody continued us... */
	set_obj_stack(t->saved_obj_stack);

	return rtval;
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
	coro_transfer(&self->stack->cothread, &t->stack->cothread);

	/* Somebody continued us... */
	set_obj_stack(self->saved_obj_stack);

	return rtval;
}

/* Call a function as a co-routine */

int co_call(int (*func)(va_list args), ...)
{
	Coroutine self[1];
	va_list ap;
	self->override = 0;
	self->override_val = 0;

	va_start(ap, func);

	if (!current_stack) {
		current_stack = (struct stack *)malloc(sizeof(struct stack));
		current_stack->caller = 0;
		current_stack->chain = 0;
		coro_create(&current_stack->cothread, NULL, NULL, NULL, 0);
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
	coro_transfer(&self->stack->cothread, &current_stack->cothread);

	/* Somebody continued us... */
	/* Free object stack we created above, restore original. */
	set_obj_stack(self->saved_obj_stack);

	va_end(ap);

	return rtval;
}

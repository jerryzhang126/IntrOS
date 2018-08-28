/******************************************************************************

    @file    IntrOS: ostask.c
    @author  Rajmund Szymanski
    @date    28.08.2018
    @brief   This file provides set of functions for IntrOS.

 ******************************************************************************

   Copyright (c) 2018 Rajmund Szymanski. All rights reserved.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

 ******************************************************************************/

#include "inc/ostask.h"
#include "inc/oscriticalsection.h"

/* -------------------------------------------------------------------------- */
void tsk_init( tsk_t *tsk, fun_t *state, stk_t *stack, unsigned size )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);
	assert(state);
	assert(stack);
	assert(size);

	sys_lock();
	{
		memset(tsk, 0, sizeof(tsk_t));

		tsk->id    = ID_STOPPED;
		tsk->state = state;
		tsk->stack = stack;
		tsk->size  = size;

		core_ctx_init(tsk);
		core_tsk_insert(tsk);
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_start( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);
	assert(tsk->state);

	sys_lock();
	{
		if (tsk->id == ID_STOPPED)
		{
			core_ctx_init(tsk);
			core_tsk_insert(tsk);
		}
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_startFrom( tsk_t *tsk, fun_t *state )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);
	assert(state);

	sys_lock();
	{
		if (tsk->id == ID_STOPPED)
		{
			tsk->state = state;

			core_ctx_init(tsk);
			core_tsk_insert(tsk);
		}
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_stop( void )
/* -------------------------------------------------------------------------- */
{
	port_set_lock();

	System.cur->id = ID_STOPPED;
	core_tsk_switch();
}

/* -------------------------------------------------------------------------- */
void tsk_kill( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);

	sys_lock();
	{
		tsk->id = ID_STOPPED;
		if (tsk == System.cur)
			core_ctx_switch();
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
void tsk_join( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);

	while (tsk->id != ID_STOPPED)
		core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
void tsk_flip( fun_t *state )
/* -------------------------------------------------------------------------- */
{
	assert(state);

	System.cur->state = state;

	core_ctx_init(System.cur);
	core_tsk_switch();
}

/* -------------------------------------------------------------------------- */
static
unsigned priv_tsk_sleep( tsk_t *cur )
/* -------------------------------------------------------------------------- */
{
	cur->id = ID_DELAYED;
	core_ctx_switch();

	return cur->event;
}

/* -------------------------------------------------------------------------- */
unsigned tsk_sleepFor( cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	tsk_t *cur = System.cur;

	sys_lock();
	{
		cur->start = core_sys_time();
		cur->delay = delay;
	}
	sys_unlock();

	return priv_tsk_sleep(cur);
}

/* -------------------------------------------------------------------------- */
unsigned tsk_sleepNext( cnt_t delay )
/* -------------------------------------------------------------------------- */
{
	tsk_t *cur = System.cur;

	cur->delay = delay;

	return priv_tsk_sleep(cur);
}

/* -------------------------------------------------------------------------- */
unsigned tsk_sleepUntil( cnt_t time )
/* -------------------------------------------------------------------------- */
{
	tsk_t *cur = System.cur;

	sys_lock();
	{
		cur->start = core_sys_time();
		cur->delay = time - cur->start;
		if (cur->delay > ((CNT_MAX)>>1))
			cur->delay = 0;
	}
	sys_unlock();

	return priv_tsk_sleep(cur);
}

/* -------------------------------------------------------------------------- */
void tsk_wait( unsigned flags )
/* -------------------------------------------------------------------------- */
{
	System.cur->event = flags;
	while (System.cur->event) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
void tsk_give( tsk_t *tsk, unsigned flags )
/* -------------------------------------------------------------------------- */
{
	assert(tsk);

	sys_lock();
	{
		if (tsk->id == ID_READY)
		{
			tsk->event &= ~flags;
		}
	}
	sys_unlock();
}

/* -------------------------------------------------------------------------- */
unsigned tsk_suspend( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(tsk);

	if (tsk->id == ID_READY)
	{
		tsk->delay = INFINITE;
		tsk->id = ID_DELAYED;
		if (tsk == System.cur)
			core_ctx_switch();
		event = E_SUCCESS;
	}

	return event;
}

/* -------------------------------------------------------------------------- */
unsigned tsk_resume( tsk_t *tsk )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(tsk);

	if (tsk->id == ID_DELAYED)
	{
		tsk->event = E_FAILURE;
		tsk->id = ID_READY;
		event = E_SUCCESS;
	}

	return event;
}

/* -------------------------------------------------------------------------- */

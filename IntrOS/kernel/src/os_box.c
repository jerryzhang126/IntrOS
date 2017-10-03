/******************************************************************************

    @file    IntrOS: os_box.c
    @author  Rajmund Szymanski
    @date    03.10.2017
    @brief   This file provides set of functions for IntrOS.

 ******************************************************************************

    IntrOS - Copyright (C) 2013 Rajmund Szymanski.

    This file is part of IntrOS distribution.

    IntrOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation; either version 3 of the License,
    or (at your option) any later version.

    IntrOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#include "inc/os_box.h"

/* -------------------------------------------------------------------------- */
void box_init( box_t *box, unsigned limit, unsigned size, void *data )
/* -------------------------------------------------------------------------- */
{
	assert(box);
	assert(limit);
	assert(size);
	assert(data);

	port_sys_lock();

	memset(box, 0, sizeof(box_t));
	
	box->limit = limit;
	box->size  = size;
	box->data  = data;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */
static
void priv_box_get( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	unsigned i;
	char*buf = box->data + box->size * box->first;

	for (i = 0; i < box->size; i++) ((char*)data)[i] = buf[i];

	box->first = (box->first + 1) % box->limit;
	box->count--;
}

/* -------------------------------------------------------------------------- */
static
void priv_box_put( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	unsigned i;
	char*buf = box->data + box->size * box->next;

	for (i = 0; i < box->size; i++) buf[i] = ((char*)data)[i];

	box->next = (box->next + 1) % box->limit;
	box->count++;
}

/* -------------------------------------------------------------------------- */
unsigned box_take( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(box);
	assert(data);

	port_sys_lock();

	if (box->count > 0)
	{
		priv_box_get(box, data);

		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void box_wait( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	while (box_take(box, data) != E_SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
unsigned box_give( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	unsigned event = E_FAILURE;

	assert(box);
	assert(data);

	port_sys_lock();

	if (box->count < box->limit)
	{
		priv_box_put(box, data);

		event = E_SUCCESS;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void box_send( box_t *box, void *data )
/* -------------------------------------------------------------------------- */
{
	while (box_give(box, data) != E_SUCCESS) core_ctx_switch();
}

/* -------------------------------------------------------------------------- */
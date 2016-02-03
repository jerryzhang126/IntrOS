/******************************************************************************

    @file    IntrOS: os_flg.c
    @author  Rajmund Szymanski
    @date    03.02.2016
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

#include <os.h>

/* -------------------------------------------------------------------------- */
unsigned flg_take( flg_id flg, unsigned flags, unsigned mode )
/* -------------------------------------------------------------------------- */
{
	unsigned event = flags;

	port_sys_lock();

	if (flags & flg->flags)
	{
		event = (mode & flgAll) ? flags & ~flg->flags : 0;

		flg->flags &= ~flags | flg->mask;
	}

	port_sys_unlock();

	return event;
}

/* -------------------------------------------------------------------------- */
void flg_wait( flg_id flg, unsigned flags, unsigned mode )
/* -------------------------------------------------------------------------- */
{
	while ((flags = flg_take(flg, flags, mode)) != 0) tsk_yield();
}

/* -------------------------------------------------------------------------- */
void flg_give( flg_id flg, unsigned flags )
/* -------------------------------------------------------------------------- */
{
	port_sys_lock();

	flg->flags |= flags;

	port_sys_unlock();
}

/* -------------------------------------------------------------------------- */

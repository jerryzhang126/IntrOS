/******************************************************************************

    @file    IntrOS: os_bar.h
    @author  Rajmund Szymanski
    @date    01.03.2016
    @brief   This file contains definitions for IntrOS.

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

#pragma once

#include <oskernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : barrier                                                                                        *
 *                                                                                                                    *
 **********************************************************************************************************************/

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : OS_BAR                                                                                         *
 *                                                                                                                    *
 * Description       : define and initilize a barrier object                                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   bar             : name of a pointer to barrier object                                                            *
 *   limit           : number of tasks that must call bar_wait[Until|For] function to release the barrier object      *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define             OS_BAR( bar, limit )                     \
                       bar_t bar##__bar = _BAR_INIT( limit ); \
                       bar_id bar = & bar##__bar

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : static_BAR                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a static barrier object                                                   *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   bar             : name of a pointer to barrier object                                                            *
 *   limit           : number of tasks that must call bar_wait[Until|For] function to release the barrier object      *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define         static_BAR( bar, limit )                     \
                static bar_t bar##__bar = _BAR_INIT( limit ); \
                static bar_id bar = & bar##__bar

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : BAR_INIT                                                                                       *
 *                                                                                                                    *
 * Description       : define and initilize a barrier object                                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : number of tasks that must call bar_wait[Until|For] function to release the barrier object      *
 *                                                                                                                    *
 * Return            : barrier object                                                                                 *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define                BAR_INIT( limit ) \
                      _BAR_INIT( limit )

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : BAR_CREATE                                                                                     *
 *                                                                                                                    *
 * Description       : define and initilize a barrier object                                                          *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   limit           : number of tasks that must call bar_wait[Until|For] function to release the barrier object      *
 *                                                                                                                    *
 * Return            : pointer to barrier object                                                                      *
 *                                                                                                                    *
 * Note              : use only in 'C' code                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

#ifndef __cplusplus
#define                BAR_CREATE( limit ) \
               &(bar_t)BAR_INIT( limit )
#endif

/**********************************************************************************************************************
 *                                                                                                                    *
 * Name              : bar_wait                                                                                       *
 *                                                                                                                    *
 * Description       : wait indefinitly until the barrier object has been released                                    *
 *                                                                                                                    *
 * Parameters                                                                                                         *
 *   bar             : pointer to barrier object                                                                      *
 *                                                                                                                    *
 * Return            : none                                                                                           *
 *                                                                                                                    *
 **********************************************************************************************************************/

              void     bar_wait( bar_id bar );

#ifdef __cplusplus
}
#endif

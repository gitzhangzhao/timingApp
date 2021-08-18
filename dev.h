/**
 * dev.h
 * Copyright (c) 2021 zhangzhao <zhangzhao@ihep.ac.cn>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DEV_H
#define DEV_H

#define EVR_FPMAP_MAX 0x003f

/* Globally reserved event numbers */
#define	ER_EVENT_NULL			   0x00  /* NULL event */
#define ER_EVENT_END			   0x7f	 /* Event sequence end */
#define ER_EVENT_FREEZE			   0x7e	 /* Freeze the event sequence */
#define ER_EVENT_RESET_TICK		   0x7d	 /* Reset the tick counter */
#define ER_EVENT_TICK			   0x7c	 /* Add 1 to the tick counter */
#define ER_EVENT_RESET_PRESCALERS  0x7b
#define ER_EVENT_HEARTBEAT		   0x7a

#endif

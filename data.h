/* TWDC - a client/server application for the Tumbleweed Developer's Contest
 * Copyright (C) 2005 Georgi D. Sotirov
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

/* Title: Data manipulation
 * Description: Data manipulation routines
 * File: data.h
 * ---
 * Written by Georgi D. Sotirov <gdsotirov@gmail.com>
 */

#ifndef __TWDC_DATA_H__
#define __TWDC_DATA_H__

#include <stdint.h>
#include <sys/types.h>

int read_data(int fd, uint8_t * buf, size_t * buf_len);
int write_data(int fd, uint8_t * buf, size_t * buf_len);
int rcv_data(int sock, uint8_t * buf, size_t buf_len, int flags);
int snd_data(int sock, uint8_t * buf, size_t buf_len, int flags);

#endif /* __TWDC_DATA_H__ */


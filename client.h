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

/* Title: Client
 * Description: Client config and definitions
 * File: client.h
 * ---
 * Written by George D. Sotirov <gdsotirov@dir.bg>
 * $Id: client.h,v 1.1 2005/04/26 16:39:23 gsotirov Exp $
 */

#ifndef __TWDC_CLIENT_H__
#define __TWDC_CLIENT_H__

/* client error codes */
#define ERR_FNAME_TOO_LONG   -100
#define ERR_HOSTNM_TOO_LONG  -101

/* client error messages */
#define ERR_FNAME_TOO_LONG_STR  "File name too long. The length should not exceed %d characters."
#define ERR_HOSTNM_TOO_LONG_STR "Host name too long. The length should not exceed %d characters."

#endif /* __TWDC_CLIENT_H__ */


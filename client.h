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
 * $Id: client.h,v 1.2 2005/04/26 18:17:56 gsotirov Exp $
 */

#ifndef __TWDC_CLIENT_H__
#define __TWDC_CLIENT_H__

/* client error codes */
#define ERR_FNAME_TOO_LONG   -100
#define ERR_HOSTNM_TOO_LONG  -101
#define ERR_CNT_RSLVE_HOST   -102
#define ERR_INVLD_AF         -103
#define ERR_CNT_OPEN_SOCK    -201
#define ERR_CNT_CNNCT_HOST   -202

/* client warning codes */
#define WARN_ZERO_FILE        100

/* client error messages */
#define ERR_FNAME_TOO_LONG_STR  "File name too long. The length should not exceed %d characters."
#define ERR_HOSTNM_TOO_LONG_STR "Host name too long. The length should not exceed %d characters."
#define ERR_CNT_RSLVE_HOST_STR "Can not resolve host '%s'"
#define ERR_INVLD_AF_STR       "Client does not support the address family of the local host"
#define ERR_CNT_OPEN_SOCK_STR  "Can not open socket"
#define ERR_CNT_CNNCT_HOST_STR "Can not connect to host '%s:%d'."

/* client warning messages */
#define WARN_ZERO_FILE_STR "The transfered file is with zero size."

#endif /* __TWDC_CLIENT_H__ */


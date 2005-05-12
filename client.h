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
 * $Id: client.h,v 1.5 2005/05/12 17:57:09 gsotirov Exp $
 */

#ifndef __TWDC_CLIENT_H__
#define __TWDC_CLIENT_H__

/* client error codes */
/* NOTE: If you add code here do not forget to add it in the switch operator
 *       of print_error function.
 */
#define ERR_FNAME_TOO_LONG   -100
#define ERR_HOSTNM_TOO_LONG  -101
#define ERR_NO_FILE_GIVEN    -102
#define ERR_CNT_RSLVE_HOST   -103
#define ERR_INVLD_AF         -104
#define ERR_CNT_RD_FILE      -201
#define ERR_CNT_GET_FILE_SZ  -202
#define ERR_CNT_OPEN_SOCK    -203
#define ERR_CNT_CNNCT_HOST   -204
#define ERR_SND_DATA         -205
#define ERR_RCV_DATA         -206
#define ERR_SRV_UNKNWN       -301
#define ERR_SRV_PROTO_VER    -302
#define ERR_SRV_UNEXPCTD     -303
#define ERR_SRV_FILE_SZ      -304

/* client warning codes */
#define WARN_ZERO_FILE        100

/* client error messages */
#define ERR_FNAME_TOO_LONG_STR  "File name too long. The length should not exceed %d characters"
#define ERR_HOSTNM_TOO_LONG_STR "Host name too long. The length should not exceed %d characters"
#define ERR_NO_FILE_GIVEN_STR   "Please, specify file to upload"
#define ERR_CNT_RSLVE_HOST_STR  "Can not resolve host '%s'"
#define ERR_INVLD_AF_STR        "Client does not support the address family of the local host"
#define ERR_CNT_RD_FILE_STR     "Can not read from file '%s'"
#define ERR_CNT_GET_FILE_SZ_STR "Can not get the size of file '%s'"
#define ERR_CNT_OPEN_SOCK_STR   "Can not open socket"
#define ERR_CNT_CNNCT_HOST_STR  "Can not connect to %s:%hd (%s:%hd)"
#define ERR_SND_DATA_STR        "While sending data to '%s:%hd'"
#define ERR_RCV_DATA_STR        "While receiving data from '%s:%hd'"
#define ERR_SRV_UNKNWN_STR      "Server: Unknown error"
#define ERR_SRV_PROTO_VER_STR   "Server: Your client protocol version %d.%d is not supported. Protocol version should be <= %d.%d"
#define ERR_SRV_UNEXPCTD_STR    "Server: Unexpected message"
#define ERR_SRV_FILE_SZ_STR     "Server: Sorry. File size too long. It should not exceed %d Bytes (%s)"

/* client warning messages */
#define WARN_ZERO_FILE_STR      "The transfered file is with zero size."

#endif /* __TWDC_CLIENT_H__ */


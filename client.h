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
 * $Id: client.h,v 1.9 2005/05/17 20:14:44 gsotirov Exp $
 */

#ifndef __TWDC_CLIENT_H__
#define __TWDC_CLIENT_H__

#include "protocol.h"

/* client error codes */
/* NOTE: If you add code here do not forget to add it in the switch operator
 *       of print_error function.
 */
#define ERR_OK              0
#define ERR_FPATH_ERR       -101
#define ERR_FNAME_ERR       -102
#define ERR_HOSTNM_ERR      -103
#define ERR_NO_FILE_GIVEN   -104
#define ERR_INVLD_FILE_NM   -105
#define ERR_NO_HOST_GIVEN   -106
#define ERR_CNT_RSLVE_HOST  -107
#define ERR_INVLD_AF        -108
#define ERR_CNT_RD_FILE     -201
#define ERR_CNT_GET_FILE_SZ -202
#define ERR_CNT_OPEN_SOCK   -203
#define ERR_CNT_CNNCT_HOST  -204
#define ERR_SND_DATA        -205
#define ERR_RCV_DATA        -206
#define ERR_WR_DATA         -207
#define ERR_RD_DATA         -208
#define ERR_PIPE            -209
#define ERR_CHILD           -210
#define ERR_ZLIB            -211
#define ERR_SRV_UNKNWN      -301
#define ERR_SRV_PROTO_VER   -302
#define ERR_SRV_FILE_SZ     -303
#define ERR_SRV_FILE_NM     -304
#define ERR_SRV_SYS         -305

/* client warning codes */
#define WARN_ZERO_FILE        100

/* client error messages */
#define ERR_FPATH_ERR_STR       "Either no path to file given or it is too long. The length should not exceed %d characters"
#define ERR_FNAME_ERR_STR       "Either no filename given or it is too long. The length should not exceed %d characters"
#define ERR_HOSTNM_ERR_STR      "Either no host name given or it is too long. The length should not exceed %d characters"
#define ERR_NO_FILE_GIVEN_STR   "Please, specify file to upload"
#define ERR_INVLD_FILE_NM_STR   "Invalid file name '%s'"
#define ERR_NO_HOST_GIVEN_STR   "Please, specify server host"
#define ERR_CNT_RSLVE_HOST_STR  "Can not resolve host '%s'"
#define ERR_INVLD_AF_STR        "Client does not support the address family of the local host"
#define ERR_CNT_RD_FILE_STR     "Cannot read from file '%s'"
#define ERR_CNT_GET_FILE_SZ_STR "Cannot get the size of file '%s'"
#define ERR_CNT_OPEN_SOCK_STR   "Cannot open socket"
#define ERR_CNT_CNNCT_HOST_STR  "Cannot connect to %s:%hd (%s:%hd)"
#define ERR_SND_DATA_STR        "While sending data to '%s:%hd'"
#define ERR_RCV_DATA_STR        "While receiving data from '%s:%hd'"
#define ERR_WR_DATA_STR         "While writting data"
#define ERR_RD_DATA_STR         "While reading data"
#define ERR_PIPE_STR            "Cannot create pipe"
#define ERR_CHILD_STR           "Cannot create child process"
#define ERR_ZLIB_STR            "While compressing data"
#define ERR_SRV_UNKNWN_STR      "Unknown error code %d"
#define ERR_SRV_PROTO_VER_STR   TWDC_ERR_PROTO_VER_STR
#define ERR_SRV_FILE_SZ_STR     TWDC_ERR_FILE_SZ_STR
#define ERR_SRV_FILE_NM_STR     TWDC_ERR_FILE_NM_STR
#define ERR_SRV_SYS_STR         TWDC_ERR_PROTO_VER_STR

/* client warning messages */
#define WARN_ZERO_FILE_STR      "The transfered file is with zero size."

#endif /* __TWDC_CLIENT_H__ */


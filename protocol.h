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

/* Title: Protocol definition
 * Description: This file define protocol specific constants and the
 *              protocol message structure
 * File: protocol.h
 * ---
 * Written by George D. Sotirov <gdsotirov@dir.bg>
 * $Id: protocol.h,v 1.3 2005/05/02 19:22:14 gsotirov Exp $
 */

#ifndef __TWDC_PROTOCOL_H__
#define __TWDC_PROTOCOL_H__

/* The communication port */
#define PORT 9919

/* Protocol message length */
#define PMSG_LNGTH 512

/* Error codes */
#define TWDC_ERR_SUCCESS 0

/* Message types */
#define TWDC_MSG_PROTO    1
#define TWDC_MSG_ERROR    2
#define TWDC_MSG_FILE_REQ 3
#define TWDC_MSG_DATA     4

/* Protocol message */
struct twdc_msg_head {
  char err_code;
  char msg_type;
};

struct twdc_msg_file {
  struct twdc_msg_head header;
  char fname[256];
};

struct twdc_msg_data {
  struct twdc_msg_head header;
  char data[PMSG_LNGTH];
};

#endif /* __TWDC_PROTOCOL_H__ */


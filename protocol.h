/* TWT_Protocol - a client/server protocol for the Tumbleweed Developer's Task
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

/* Title: Protocol for the Tumbleweed Developer's Contest Task
 * Description: Server configuraiton
 * File: protocol.h
 * ---
 * Written by George D. Sotirov <gdsotirov@dir.bg>
 * $Id: protocol.h,v 1.1 2005/04/26 04:50:57 gsotirov Exp $
 */

#ifndef __TWDC_PROTOCOL_H__
#define __TWDC_PROTOCOL_H__

/* The communication port */
#define PORT 9919

/* Error codes */
#define TWDC_ERR_SUCCESS 0

/* Message types */
#define TWDC_MSG_ERROR    100
#define TWDC_MSG_FILE_REQ 101
#define TWDC_MSG_DATA     102

/* Protocol message */
struct twdc_msg {
  char err_code;
  char msg_type;
  char data[512];
};

#endif /* __TWDC_PROTOCOL_H__ */


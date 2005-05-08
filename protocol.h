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
 * Description: Definition of protocol specific constants, the protocol
 *              messages and interface
 * File: protocol.h
 * ---
 * Written by George D. Sotirov <gdsotirov@dir.bg>
 * $Id: protocol.h,v 1.5 2005/05/08 15:42:17 gsotirov Exp $
 */

#ifndef __TWDC_PROTOCOL_H__
#define __TWDC_PROTOCOL_H__

/* Protocol version */
#define TWDC_PROTO_VER_MAJOR 0
#define TWDC_PROTO_VER_MINOR 1

/* The communication port */
#define PORT 9919

/* Protocol message length */
#define PMSG_LNGTH 512

/* Message types */
#define TWDC_MSG_RSRVD    1
#define TWDC_MSG_ERROR    2
#define TWDC_MSG_FILE_REQ 3
#define TWDC_MSG_DATA     4

/* Error codes */
#define TWDC_ERR_SUCCESS      0
#define TWDC_ERR_UNEXPCTD_MSG -101
#define TWDC_ERR_FILE_SZ      -102

/* Protocol message */
struct twdc_msg_head {
  char msg_type;
  char ver_major;
  char ver_minor;
};

/* Error message */
struct twdc_msg_err {
  char err_code;
  char data[4];
};

/* File request message - file name + file size */
struct twdc_msg_file {
  char     fname[256];
  long int fsize;
};

/* Data message - header + data */
struct twdc_msg_data {
  char buf[PMSG_LNGTH];
};

/* Message body type */
union twdc_data {
  struct twdc_msg_err  error;
  struct twdc_msg_file file;
  struct twdc_msg_data data;
};

/* General message */
struct twdc_msg {
  struct twdc_msg_head header;
  union  twdc_data     body;
};

/* Protocol interface */
void make_err_msg(struct twdc_msg * msg, char err_cd);
void make_file_msg(struct twdc_msg * msg, char * fname, long int fsize);
void make_data_msg(struct twdc_msg * msg, char * buf, long int buf_sz);

#endif /* __TWDC_PROTOCOL_H__ */


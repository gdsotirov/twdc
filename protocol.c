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

/* Title: Protocol implementation
 * Description: Protocol interface implementation
 * File: protocol.c
 * ---
 * Written by George D. Sotirov <gdsotirov@dir.bg>
 * $Id: protocol.c,v 1.1 2005/05/08 15:42:17 gsotirov Exp $
 */

#include <string.h>

#include "protocol.h"

/* Function    : set_version
 * Description : Set protocol version
 */
void set_version(struct twdc_msg_head * head) {
  head->ver_major = TWDC_PROTO_VER_MAJOR;
  head->ver_minor = TWDC_PROTO_VER_MINOR;
}

/* Function    : make_err_msg
 * Description : Init an error message
 */
void make_err_msg(struct twdc_msg * msg, char err_cd) {
  msg->header.msg_type = TWDC_MSG_ERROR;
  set_version(&msg->header);
  msg->body.error.err_code = err_cd;
}

/* Function    : make_file_msg
 * Description : Init an file request message
 */
void make_file_msg(struct twdc_msg * msg, char * fname, long int fsize) {
  msg->header.msg_type = TWDC_MSG_FILE_REQ;
  set_version(&msg->header);
  strcpy(msg->body.file.fname, fname);
  msg->body.file.fsize = fsize;
}

/* Function    : make_data_msg
 * Description : Init an data message
 */
void make_data_msg(struct twdc_msg * msg, char * buf, long int buf_sz) {
  msg->header.msg_type = TWDC_MSG_DATA;
  set_version(&msg->header);
  memcpy(msg->body.data.buf, buf, buf_sz);
}


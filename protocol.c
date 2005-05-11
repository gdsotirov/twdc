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
 * $Id: protocol.c,v 1.2 2005/05/11 21:23:56 gsotirov Exp $
 */

#include <string.h>
#include <stdarg.h>
#include <netinet/in.h>

#include "protocol.h"

int get_msg_type(struct twdc_msg_head * msg) {
  if ( msg != NULL )
    return msg->msg_type;
  return 0;
}

/* Function    : set_version
 * Description : Set protocol version
 */
void set_version(struct twdc_msg_head * head) {
  if ( head != NULL ) {
    head->ver_major = TWDC_PROTO_VER_MAJOR;
    head->ver_minor = TWDC_PROTO_VER_MINOR;
  }
}

/* Function    : check_version_maj
 * Description : Check the major version of a protocol message
 * Input       : msg - mesage header
 *               ver_major - the major version to check
 *               check_type - the type of the check, one of CT_EXACT, CT_GT,
 *                            CT_GTOREQ, CT_LT, CT_LTOREQ
 */
int check_version_maj(struct twdc_msg_head * msg, uint8_t ver_major, int check_type) {
  if ( msg != NULL ) {
    if ( check_type == CT_EXACT )
      return (ver_major == msg->ver_major);
    else if ( check_type == CT_GT )
      return (ver_major > msg->ver_major);
    else if ( check_type == CT_GTOREQ )
      return (ver_major >= msg->ver_major);
    else if ( check_type == CT_LT )
      return (ver_major < msg->ver_major);
    else
      return (ver_major <= msg->ver_major);
  }
  return 0; /* false */
}

/* Function    : check_version_min
 * Description : Check the minor version of a protocol message
 * Input       : msg - mesage header
 *               ver_minor - the minor version to check
 *               check_type - the type of the check, one of CT_EXACT, CT_GT,
 *                            CT_GTOREQ, CT_LT, CT_LTOREQ
 */
int check_version_min(struct twdc_msg_head * msg, uint8_t ver_minor, int check_type) {
  if ( msg != NULL ) {
    if ( check_type == CT_EXACT )
      return (ver_minor == msg->ver_minor);
    else if ( check_type == CT_GT )
      return (ver_minor > msg->ver_minor);
    else if ( check_type == CT_GTOREQ )
      return (ver_minor >= msg->ver_minor);
    else if ( check_type == CT_LT )
      return (ver_minor < msg->ver_minor);
    else
      return (ver_minor <= msg->ver_minor);
  }
  return 0; /* false */
}

/* Function    : make_err_msg
 * Description : Init an error message
 */
void make_err_msg(struct twdc_msg * msg, int8_t err_cd, ...) {
  if ( msg != NULL ) {
    va_list vl;

    msg->header.msg_type = TWDC_MSG_ERROR;
    set_version(&msg->header);
    msg->body.error.err_code = err_cd;

    va_start(vl, err_cd);
    switch ( err_cd ) {
      case TWDC_ERR_FILE_SZ: {
        uint32_t max_sz = va_arg(vl, uint32_t);
        memcpy(msg->body.error.data, (void *)htonl(max_sz), sizeof(msg->body.error.data));

      }
      break;
    default: break;
    }
    va_end(vl);
  }
}

/* Function    : make_file_msg
 * Description : Init an file request message
 */
void make_file_msg(struct twdc_msg * msg, char * fname, size_t fsize) {
  if ( msg != NULL ) {
    msg->header.msg_type = TWDC_MSG_FILE_REQ;
    set_version(&msg->header);
    if ( fname != NULL )
      strncpy(msg->body.file.fname, fname, sizeof(msg->body.file.fname));
    msg->body.file.fsize = htonl(fsize);
  }
}

/* Function    : make_data_msg
 * Description : Init an data message
 */
void make_data_msg(struct twdc_msg * msg, char * buf, size_t buf_sz) {
  if ( msg != NULL ) {
    msg->header.msg_type = TWDC_MSG_DATA;
    set_version(&msg->header);
    if ( buf != NULL )
      memcpy(msg->body.data.buf, buf, buf_sz);
  }
}

/* Function    : read_err_msg
 * Description : Retrive information from an error message
 */
void read_err_msg(struct twdc_msg * msg, int8_t * err_cd, ...) {
  if ( msg != NULL ) {
    va_list vl;

    if ( err_cd != NULL )
      *err_cd = msg->body.error.err_code;

    va_start(vl, err_cd);
    switch ( *err_cd ) {
      case TWDC_ERR_FILE_SZ: {
          uint32_t * max_sz = NULL;
          if ( (max_sz = va_arg(vl, uint32_t *)) != NULL )
            memcpy(max_sz, (void *)ntohl((uint32_t)msg->body.error.data), sizeof(max_sz));
        }
        break;
      default: break;
    }
    va_end(vl);
  }
}

/* Function    : read_file_msg
 * Description : Retrive information from an file request message
 */
void read_file_msg(struct twdc_msg * msg, char * fname, size_t fname_sz, size_t * fsize) {
  if ( msg != NULL ) {
    if ( fname != NULL )
      strncpy(fname, msg->body.file.fname, fname_sz);
    if ( fsize != NULL )
      *fsize = ntohl(msg->body.file.fsize);
  }
}



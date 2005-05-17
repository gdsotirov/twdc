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
 * $Id: protocol.c,v 1.6 2005/05/17 20:26:17 gsotirov Exp $
 */

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <netinet/in.h>

#include "protocol.h"

int get_msg_type(const struct twdc_msg_head * msg) {
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
int check_version_maj(const struct twdc_msg_head * msg, const uint8_t ver_major, const int check_type) {
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
int check_version_min(const struct twdc_msg_head * msg, const uint8_t ver_minor, const int check_type) {
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

/* Function    : get_ver_info
 * Description : Extract protocol version info for a message
 */
void get_ver_info(const struct twdc_msg_head * msg, uint8_t * ver_maj, uint8_t * ver_min) {
  *ver_maj = msg->ver_major;
  *ver_min = msg->ver_minor;
}

/* Function    : make_err_msg
 * Description : Init an error message
 */
void make_err_msg(struct twdc_msg * msg, const int8_t err_cd, ...) {
  if ( msg != NULL ) {
    va_list vl;

    msg->header.msg_type = TWDC_MSG_ERROR;
    set_version(&msg->header);
    msg->body.error.err_code = err_cd;

    va_start(vl, err_cd);
    switch ( err_cd ) {
      /* The extra information to the file size error message will be
       * the max file size which the server can accept.
       */
      case TWDC_ERR_FILE_SZ:
        msg->body.error.data.max_sz = htonl(va_arg(vl, size_t));
        break;
    default: break;
    }
    va_end(vl);
  }
}

/* Function    : make_file_msg
 * Description : Init an file request message
 */
void make_file_msg(struct twdc_msg * msg, const uint8_t * fname, const size_t fsize) {
  if ( msg != NULL ) {
    msg->header.msg_type = TWDC_MSG_FILE_REQ;
    set_version(&msg->header);
    if ( fname != NULL )
      memcpy(msg->body.file.fname, fname, strlen((char *)fname) + 1);
    msg->body.file.fsize = htonl(fsize);
  }
}

/* Function    : make_data_msg
 * Description : Init an data message
 */
void make_data_msg(struct twdc_msg * msg, const void * buf, const size_t buf_sz) {
  if ( msg != NULL ) {
    msg->header.msg_type = TWDC_MSG_DATA;
    set_version(&msg->header);
    if ( buf != NULL ) {
      memcpy(msg->body.data.buf, buf, buf_sz);
      msg->body.data.size = buf_sz;
    }
  }
}

/* Function    : get_err_code
 * Description : Retrieve error code from an error message
 */
int8_t get_err_code(const struct twdc_msg * msg) {
  if ( msg != NULL )
    return msg->body.error.err_code;
  return 0;
}

/* Function    : read_err_msg
 * Description : Retrive information from an error message
 */
void read_err_msg(const struct twdc_msg * msg, ...) {
  int8_t err_cd = 0;

  if ( msg != NULL ) {
    va_list vl;

    err_cd = msg->body.error.err_code;
    va_start(vl, msg);
    switch ( err_cd ) {
      case TWDC_ERR_FILE_SZ: {
          size_t * max_sz = va_arg(vl, size_t *);
          if ( max_sz != NULL )
            *max_sz = ntohl(msg->body.error.data.max_sz);
        }
        break;
      default: break;
    }
    va_end(vl);
  }
}

/* Function    : read_file_msg
 * Description : Retrieve information from a file request message
 * Input       : msg - pointer to a message buffer
 *               fname - pointer to a buffer where to copy the file name
 *                       if it points to nothing (NULL) the function will
 *                       allocate enought memory to copy the file name
 *               fname_len - the length of the fname buffer if it point to
 *                           something diferent than NULL
 *               fsize - the size of the file
 *
 *
 */
void read_file_msg(const struct twdc_msg * msg, uint8_t * fname, const size_t fname_len, size_t * fsize) {
  if ( msg != NULL ) {
    if ( fname != NULL )
      strncpy((char *)fname, (char *)msg->body.file.fname, fname_len);
    else {
      size_t fname_len = strlen((char *)msg->body.file.fname);
      if ( (fname = malloc(fname_len + 1)) != NULL )
        memcpy(fname, msg->body.file.fname, fname_len + 1);
    }
    if ( fsize != NULL )
      *fsize = ntohl(msg->body.file.fsize);
  }
}



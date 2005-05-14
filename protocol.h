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
 * $Id: protocol.h,v 1.9 2005/05/14 22:22:54 gsotirov Exp $
 */

#ifndef __TWDC_PROTOCOL_H__
#define __TWDC_PROTOCOL_H__

#include <stdint.h>
#include <sys/param.h>

/* Protocol version */
#define TWDC_PROTO_VER_MAJOR 0
#define TWDC_PROTO_VER_MINOR 1

/* Check types */
#define CT_EXACT  0x0
#define CT_GT     0x1
#define CT_GTOREQ 0x2
#define CT_LT     0x3
#define CT_LTOREQ 0x4

/* The communication port */
#define PORT 9919

/* Protocol message length */
#define PDATA_MAX 512

/* Message types */
#define TWDC_MSG_RSRVD    1
#define TWDC_MSG_ERROR    2
#define TWDC_MSG_FILE_REQ 3
#define TWDC_MSG_DATA     4

/* Error codes */
#define TWDC_ERR_OK        0
#define TWDC_ERR_RSRVD     -11
#define TWDC_ERR_PROTO_VER -12
#define TWDC_ERR_FILE_SZ   -13
#define TWDC_ERR_FILE_NM   -14
#define TWDC_ERR_SYS       -51

/* Error strings */
#define TWDC_ERR_OK_STR        ""
#define TWDC_ERR_RSRVD_STR     "Reserved"
#define TWDC_ERR_PROTO_VER_STR "Your protocol version %d.%d is not supported. Supported protocol versions are prior or equal to %d.%d"
#define TWDC_ERR_FILE_SZ_STR   "Sorry. File size too long. It should not exceed %d Bytes (%s)"
#define TWDC_ERR_FILE_NM_STR   "No file name specified"
#define TWDC_ERR_SYS_STR       "A system error occured while in operation"

#ifdef __GNUC__
#define DI_PACKET __attribute__ ((packed, aligned(1)))
#else
#define DI_PACKET
#endif

/* Pack and align all data structures on the byte boundary to minimize memory usage and transfered data */
#if defined(_MSC_VER) || defined(__WATCOMC__) || ( defined(__BORLANDC__) && __BORLANDC__ > 0x520 )
#pragma pack(1)
#endif

#if defined(__BORLANDC__) && __BORLANDC__ <= 0x520
#pragma option -a1
#endif

/* Protocol message */
struct twdc_msg_head {
  uint8_t msg_type;
  uint8_t ver_major;
  uint8_t ver_minor;
} DI_PACKET;

#define TWDC_MSG_HEAD_SZ sizeof(struct twdc_msg_head)

/* Error message */
struct twdc_msg_err {
  int8_t err_code;
  union error_data {
    size_t max_sz;
  } data;
} DI_PACKET;

#define TWDC_MSG_ERR_SZ sizeof(struct twdc_msg_err)

/* File request message - file name + file size */
struct twdc_msg_file {
  char   fname[NAME_MAX];
  size_t fsize;
} DI_PACKET;

#define TWDC_MSG_FILE_SZ sizeof(struct twdc_msg_file)

/* Data message - header + data */
struct twdc_msg_data {
  char buf[PDATA_MAX];
} DI_PACKET;

#define TWDC_MSG_DATA_SZ sizeof(struct twdc_msg_data)

/* Message body type */
union twdc_msg_body {
  struct twdc_msg_err  error DI_PACKET;
  struct twdc_msg_file file  DI_PACKET;
  struct twdc_msg_data data  DI_PACKET;
};

#define TWDC_MSG_BODY_SZ sizeof(struct twdc_msg_body)

/* General message */
struct twdc_msg {
  struct twdc_msg_head header DI_PACKET;
  union  twdc_msg_body body   DI_PACKET;
};

#define TWDC_MSG_SZ sizeof(struct twdc_msg)
#define TWDC_MSG_ERR_FULL_SZ  TWDC_MSG_HEAD_SZ + TWDC_MSG_ERR_SZ
#define TWDC_MSG_FILE_FULL_SZ TWDC_MSG_HEAD_SZ + TWDC_MSG_FILE_SZ
#define TWDC_MSG_DATA_FULL_SZ TWDC_MSG_HEAD_SZ + TWDC_MSG_DATA_SZ

#if defined(_MSC_VER) || defined(__WATCOMC__) || ( defined(__BORLANDC__) && __BORLANDC__ > 0x520 ) || defined(__INTELC__)
#pragma pack()
#endif

#if defined(__BORLANDC__) && __BORLANDC__ <= 0x520
#pragma option -a.
#endif

/* Protocol interface */
int get_msg_type(const struct twdc_msg_head * msg);
void get_ver_info(const struct twdc_msg_head * msg, int8_t * ver_maj, int8_t * ver_min);

int check_version_maj(const struct twdc_msg_head * msg, const uint8_t ver_major, const int check_type);
int check_version_min(const struct twdc_msg_head * msg, const uint8_t ver_minor, const int check_type);

void make_err_msg(struct twdc_msg * msg, const int8_t err_cd, ...);
void make_file_msg(struct twdc_msg * msg, const char * fname, const size_t fsize);
void make_data_msg(struct twdc_msg * msg, const char * buf, const size_t buf_sz);

int8_t get_err_code(const struct twdc_msg * msg);
void read_err_msg(const struct twdc_msg * msg, ...);
void read_file_msg(const struct twdc_msg * msg, char * fname, const size_t fname_len, size_t * fsize);
void read_data_msg(const struct twdc_msg * msg, char * buf, size_t * buf_sz);

#endif /* __TWDC_PROTOCOL_H__ */


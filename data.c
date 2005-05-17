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

/* Title: Data manipulation
 * Description: Data manipulation routines
 * File: data.c
 * ---
 * Written by George D. Sotirov <gdsotirov@dir.bg>
 * $Id: data.c,v 1.3 2005/05/17 20:17:47 gsotirov Exp $
 */

#include <unistd.h>
#include <sys/socket.h>

#include "data.h"

/* Function    : read_data
 * Description : Read data from a file until the passed buffer gets full, eof or error occured
 * Input       : fd - file descriptor
 *               buf - the buffer to read in
 *               buf_len - the length of the buffer
 */
int read_data(int fd, uint8_t * buf, size_t * buf_len) {
  size_t tr = 0; /* total read */
  size_t br = 0; /* bytes read at once */

  while ( tr < *buf_len ) {
    if ( (br = read(fd, buf, *buf_len)) > 0 ) {
      tr += br;
      buf += br;
    }
    else if ( br == 0 ) { /* EOF */
      *buf_len = tr;
      break;
    }
    else
      return -1;
  }

  return 0;
}

/* Function    : write_data
 * Description : Write all data from a buffer to a file or as much as possible
 * Input       : fd - file descriptor
 *               buf - the buffer to write
 *               buf_len - the length of the buffer
 */
int write_data(int fd, uint8_t * buf, size_t * buf_len) {
  size_t tw = 0; /* total written */
  size_t bw = 0; /* bytes written at once */

  while ( tw < *buf_len ) {
    if ( (bw = write(fd, buf, *buf_len)) > 0 ) {
      tw += bw;
      buf += bw;
    }
    else if ( bw == 0 ) {
      *buf_len = tw;
      break;
    }
    else if ( bw < 0 )
      return -1;
  }

  return 0;
}

/* Function    : rcv_data
 * Description : Receive data from a stream socket until the passed buffer gets full
 * Input       : sock - socket descriptor
 *               buf - the buffer to read in
 *               buf_len - the length of the buffer
 *               flags - same flags as in recv(2) function
 */
int rcv_data(int sock, uint8_t * buf, size_t buf_len, int flags) {
  size_t tr = 0; /* total received */
  size_t br = 0; /* bytes received at once */

  while ( tr < buf_len) {
    if ( (br = recv(sock, buf, buf_len, flags)) > 0 ) {
      tr += br;
      buf += br;
    }
    else if ( br < 0 )
      return -1;
  }

  return tr;
}

/* Function    : snd_data
 * Description : Send all data from a buffer
 * Input       : sock - socket descriptor
 *               buf - the buffer to send
 *               buf_len - the length of the buffer
 *               flags - same flags as in recv(2) function
 */
int snd_data(int sock, uint8_t * buf, size_t buf_len, int flags) {
  size_t ts = 0; /* total send */
  size_t bs = 0; /* bytes send at once */

  while ( ts < buf_len) {
    if ( (bs = send(sock, buf, buf_len, flags)) > 0 ) {
      ts += bs;
      buf += bs;
    }
    else if ( bs < 0 )
      return -1;
  }

  return ts;
}


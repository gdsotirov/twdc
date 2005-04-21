/* TWT_Server - a client programm for the Tumbleweed Developer's Task
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

/* Title: Server for the Tumbleweed Developer's Contest Task
 * Description: Server main programm
 * File: server.c
 * ---
 * Written by George D. Sotirov <gdsotirov@dir.bg>
 * $Id: server.c,v 1.2 2005/04/21 19:24:22 gsotirov Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "server.h"

int main(int argc, char * argv[]) {
  int sock = 0;
  int tcp_proto_num = 0;
  struct protoent * tcp_protoent = NULL;
  char * prog_name = argv[0];

  if ( (tcp_protoent = getprotobyname("tcp")) != NULL )
    tcp_proto_num = tcp_protoent->p_proto;
  else {
    printf("%s: Error: TCP protocol unknown.\n", prog_name);
    exit(-1);
  }

  if ( (sock = socket(PF_INET, SOCK_STREAM, tcp_proto_num)) == -1) {
    printf("%s: Error: Can not open socket.\n", prog_name);
    exit(-1);
  }

  if ( listen(sock, BACKLOG) == -1 ) {
    printf("%s: Error: Can not start listening on scoket.\n", prog_name);
    exit(-1);
  }

  /* TODO: Continue from here */

  return 0;
}


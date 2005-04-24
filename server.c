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
 * $Id: server.c,v 1.3 2005/04/24 19:38:18 gsotirov Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <netdb.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "server.h"

int writelog(int err_num, char * msg);

int main(int argc, char * argv[]) {
  int sock = 0;
  int tcp_proto_num = 0;
  struct protoent * tcp_protoent = NULL;

  /* Become a daemon */
  /* - close standard files */
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  /* - fork a child to start a new session */
  switch ( fork() ) {
    case -1: writelog(errno, "Error: Can not fork a child");
             exit(-1);
    case  0: /* TODO: first child */
    default: exit(0);
  }

  signal(SIGCHLD, SIG_IGN);

  if ( (tcp_protoent = getprotobyname("tcp")) != NULL )
    tcp_proto_num = tcp_protoent->p_proto;
  else {
    writelog(errno, "Error: TCP protocol unknown");
    exit(-1);
  }

  /* TODO: Bind the socket here */

  if ( (sock = socket(PF_INET, SOCK_STREAM, tcp_proto_num)) == -1) {
    writelog(errno, "Error: Can not open socket");
    exit(-1);
  }

  if ( listen(sock, BACKLOG) == -1 ) {
    writelog(errno, "Error: Can not start listening on socket.");
    exit(-1);
  }

  /* TODO: Continue from here */

  return 0;
}

int writelog(int err_num, char * msg) {
  FILE * logfp = NULL;
  time_t now;
  struct tm * now_tm = NULL;
  pid_t pid = 0;
  char * sys_err = NULL;
  char dt_fmt[64] = {0};

  if ( (logfp = fopen(LOGFILE, "a")) == NULL ) {
    fclose(logfp);
    return -1;
  }

  time(&now);
  now_tm = localtime(&now);
  strftime(dt_fmt, sizeof(dt_fmt), "%Y-%m-%d %H:%M:%S", now_tm);
  sys_err = strerror(err_num);

  fprintf(logfp, "%s [%d] %s. System error: %s ", dt_fmt, pid, msg, sys_err);

  fclose(logfp);

  return 0;
}


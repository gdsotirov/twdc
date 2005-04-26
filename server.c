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
 * $Id: server.c,v 1.5 2005/04/26 04:52:07 gsotirov Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include "server.h"
#include "protocol.h"

int writelog(int err_num, char * msg);
int init_addr(struct sockaddr_in * addr);

int main(int argc, char * argv[]) {
  int tcp_proto_num = 0;
  struct protoent * tcp_protoent = NULL;
  int sock = 0;
  struct sockaddr_in sock_addr;

  /* Become a daemon */
  /* - close standard files */
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  /* - fork a child to start a new session */
  switch ( fork() ) {
    case -1: writelog(errno, "Error: Can not fork a child");
             exit(-1);
  /* - become session leader */
    case  0: if ( setsid() == -1 ) {
               writelog(errno, "Error: Can not become session leader");
               exit(-1);
             }

             switch ( fork() ) {
               case -1: writelog(errno, "Error: Can not fork a child");
                        exit(-1);
               case  0: umask(0);
                        break;
               default: exit(0);
             }
             break;
    default: exit(0);
  }

  /* Child process */
  signal(SIGCHLD, SIG_IGN);

  if ( (tcp_protoent = getprotobyname("tcp")) != NULL )
    tcp_proto_num = tcp_protoent->p_proto;
  else {
    writelog(errno, "Error: TCP protocol unknown");
    exit(-1);
  }

  if ( (sock = socket(PF_INET, SOCK_STREAM, tcp_proto_num)) == -1) {
    writelog(errno, "Error: Can not open socket");
    exit(-1);
  }

  if ( init_addr(&sock_addr) != 0 )
    exit(-1);

  if ( bind(sock, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) != 0 ) {
    writelog(errno, "Error: Can not bind socket");
    close(sock);
    exit(-1);
  }

  if ( listen(sock, BACKLOG) == -1 ) {
    writelog(errno, "Error: Can not start listening on socket.");
    close(sock);
    exit(-1);
  }

  while (1) ;

  close(sock);

  return 0;
}

/* Function    : writelog
 * Description : Append error information to the server log.
 * Input       : err_num - the global variable errno
 *               msg     - custom error message to prepend befor the sys error
 */
int writelog(int err_num, char * msg) {
  char log_fname[256] = {0};
  FILE * logfp = NULL;
  time_t now;
  struct tm * now_tm = NULL;
  pid_t pid = getpid();
  char * sys_err = NULL;
  char dt_fmt[64] = {0};

  sprintf(log_fname, LOGFILE, pid);
  if ( (logfp = fopen(log_fname, "a+")) == NULL ) {
    fclose(logfp);
    return -1;
  }

  time(&now);
  now_tm = localtime(&now);
  strftime(dt_fmt, sizeof(dt_fmt), "%Y-%m-%d %H:%M:%S", now_tm);
  sys_err = strerror(err_num);

  fprintf(logfp, "%s [%d] %s. System error: %s\n", dt_fmt, pid, msg, sys_err);

  fclose(logfp);

  return 0;
}

/* Function    : init_addr
 * Description : Initialize server address structure
 * Input       : Pointer to a sockaddr_in structure
 * Return      : On sucess the function return 0. Any other value indicates
 *               error.
 */
int init_addr(struct sockaddr_in * addr) {
  char host_nm[256] = {0};
  struct hostent * host_ent = NULL;

  if ( addr == NULL )
    return -1;

  if ( gethostname(host_nm, sizeof(host_nm)) == -1 ) {
    writelog(errno, "Erorr: Can not get host name");
    return -1;
  }

  if ( (host_ent = gethostbyname(host_nm)) == NULL ) {
    writelog(errno, "Error: Can not retrieve host address information");
    return -1;
  }

  addr->sin_family = (sa_family_t)host_ent->h_addrtype;
  addr->sin_port = (in_port_t)htons((uint16_t)PORT);
  addr->sin_addr.s_addr = (in_addr_t)host_ent->h_addr_list[0];

  return 0;
}


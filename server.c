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

/* Title: Server
 * Description: Server main programm
 * File: server.c
 * ---
 * Written by George D. Sotirov <gdsotirov@dir.bg>
 * $Id: server.c,v 1.10 2005/05/08 15:43:35 gsotirov Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "globals.h"
#include "protocol.h"
#include "data.h"
#include "server.h"

int writelog(int err_num, char * cl_msg_fmt, ...);
int init_addr(char * host_nm, size_t host_nm_sz, struct sockaddr_in * addr);
int service(int cl_sock, struct sockaddr_in * cl_addr);

int main(int argc, char * argv[]) {
  int sock = 0;
  struct sockaddr_in sock_addr;
  char hostnm[256] = {0};
  char srv_addr_str[16] = {0};

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

  if ( (sock = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
    writelog(errno, "Error: Can not open socket");
    close(sock);
    exit(-1);
  }

  /* TODO: Configure the socket here */

  if ( init_addr(hostnm, sizeof(hostnm), &sock_addr) != 0 )
    exit(-1);

  strncpy(srv_addr_str, inet_ntoa(sock_addr.sin_addr), sizeof(srv_addr_str));

  if ( bind(sock, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) != 0 ) {
    writelog(errno, "Error: Can not bind socket"); close(sock);
    exit(-1);
  }

  if ( listen(sock, BACKLOG) == -1 ) {
    writelog(errno, "Error: Can not start listening on socket.");
    close(sock);
    exit(-1);
  }

  writelog(0, "Info: Start listening on all interfaces of %s (%s:%d)", hostnm, srv_addr_str, PORT);

  while (1) {
    int cl_sock = 0;
    struct sockaddr_in cl_addr;
    socklen_t cl_addr_len = sizeof(cl_addr);

    if ( (cl_sock = accept(sock, (struct sockaddr *)&cl_addr, &cl_addr_len)) == -1) {
      writelog(errno, "Error: Can not accept connection from client '%s'", inet_ntoa(cl_addr.sin_addr));
      close(cl_sock);
    }

    switch ( fork() ) {
      case -1: writelog(errno, "Error: Can not fork a child");
               break;
      case  0: {
                 int err = service(cl_sock, &cl_addr);
                 shutdown(cl_sock, SHUT_RDWR);
                 close(cl_sock);
                 exit(err);
               }
      default: break;
    }

    sleep(1);
  }

  close(sock);

  return 0;
}

/* Function    : writelog
 * Description : Append error information to the server log.
 * Input       : err_num - the global variable errno
 *               msg     - custom error message to prepend befor the sys error
 */
int writelog(int err_num, char * cl_msg_fmt, ...) {
  FILE * logfp = NULL;
  static char log_fname[256] = {0};
  time_t now;
  struct tm * now_tm = NULL;
  char dt_fmt[64] = {0};
  char cl_msg[256] = {0};
  va_list v_lst;
  char * sys_err = NULL;

  if (strlen(log_fname) == 0 ) 
    sprintf(log_fname, LOGFILE, getpid());

  if ( (logfp = fopen(log_fname, "a+")) == NULL ) {
    fclose(logfp);
    return -1;
  }

  time(&now);
  now_tm = localtime(&now);
  strftime(dt_fmt, sizeof(dt_fmt), "%Y-%m-%d %H:%M:%S", now_tm);
  va_start(v_lst, cl_msg_fmt);
  vsprintf(cl_msg, cl_msg_fmt, v_lst);
  sys_err = strerror(err_num);

  if ( err_num != 0 )
    fprintf(logfp, "%s [%d] %s. System error: %s\n", dt_fmt, getpid(), cl_msg, sys_err);
  else
    fprintf(logfp, "%s [%d] %s\n", dt_fmt, getpid(), cl_msg);

  fclose(logfp);

  return 0;
}

/* Function    : init_addr
 * Description : Initialize server address structure
 * Input       : Pointer to a sockaddr_in structure
 * Return      : On sucess the function return 0. Any other value indicates
 *               error.
 */
int init_addr(char * host_nm, size_t host_nm_sz, struct sockaddr_in * addr) {
  struct hostent * host_ent = NULL;

  if ( addr == NULL )
    return -1;

  if ( gethostname(host_nm, host_nm_sz) == -1 ) {
    writelog(errno, "Erorr: Can not get host name");
    return -1;
  }

  if ( (host_ent = gethostbyname(host_nm)) == NULL ) {
    writelog(errno, "Error: Can not retrieve host address information");
    return -1;
  }

  addr->sin_family = (sa_family_t)host_ent->h_addrtype;
  addr->sin_port = (in_port_t)htons((uint16_t)PORT);
  addr->sin_addr.s_addr = htons(INADDR_ANY);

  return 0;
}

/* Function    : service
 * Description : Service a client
 * Return      : On success the function will return zero.
 */
int service(int cl_sock, struct sockaddr_in * cl_addr) {
  /*struct twdc_msg_file file_req;
  struct twdc_msg_data rcvbuf;
  int rcvbuf_len = 0;
  struct twdc_msg msg;
  
  * File request *
  if ( rcv_data(cl_sock, (char *)&file_req, sizeof(file_req)) != 0 ) {
    msg.header.error.err_code = TWDC_ERR_UNEXPCTD_MSG;
    msg.header.msg_type = TWDC_MSG_ERROR;
    snd_data(cl_sock, (char *)&msg, sizeof(msg));
    return TWDC_ERR_UNEXPCTD_MSG;
  }

  * Deny upload of files bigger than the given limit *
  if ( file_req.fsize > MAXFILESIZE ) {
    int max_sz = MAXFILESIZE;
    writelog(0, "Error: Client '%s' tryied to upload file with size %d Bytes", inet_ntoa(cl_addr->sin_addr), file_req.fsize);
    msg.header.err_code = TWDC_ERR_FILE_SZ;
    msg.header.msg_type = TWDC_MSG_ERROR;
    memcpy(msg.data, &max_sz, sizeof(msg.data));
    snd_data(cl_sock, (char *)&msg, sizeof(msg));
    return TWDC_ERR_FILE_SZ;
  }
  else {
    msg.header.err_code = TWDC_ERR_SUCCESS;
    msg.header.msg_type = TWDC_MSG_ERROR;
    snd_data(cl_sock, (char *)&msg, sizeof(msg));
  }

  writelog(0, "Info: Accepted file '%s' (%d bytes) from '%s'", file_req.fname, file_req.fsize, inet_ntoa(cl_addr->sin_addr));

  * Receive and unzip the file *
  while ( rcvbuf_len = recv(cl_sock, (char *)&rcvbuf, sizeof(rcvbuf), 0x0) ) {
    if ( rcvbuf_len > 0 ) {
      * TODO: receive data, check for data consistency *
    }
    else if ( rcvbuf_len < 0 ) {
      * TODO: Print error message *
    }
    else {
      * TODO: End of transfer? Check if filesize is what the client send. *
    }
  }*/

  return 0;
}


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
 * $Id: server.c,v 1.17 2005/05/17 20:25:55 gsotirov Exp $
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/* Third party library includes */
#include <zlib.h>

#include "globals.h"
#include "protocol.h"
#include "data.h"
#include "server.h"

#define CHUNK 16384

int writelog(const int err_num, const char * cl_msg_fmt, ...);
int init_addr(char * host_nm, const size_t host_nm_sz, struct sockaddr_in * addr);
int service(int cl_sock, struct sockaddr_in * cl_addr);
int decompress_writefile(int dest, void * data, size_t data_sz);

int main(int argc, char * argv[]) {
  int sock = 0;
  struct sockaddr_in sock_addr;
  char hostnm[HOST_NAME_MAX] = {0};
  char srv_addr_str[INET_ADDRSTRLEN] = {0};

  /* Become a daemon */
  /* - close standard files */
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  /* - fork a child to start a new session */
  switch ( fork() ) {
    case -1: writelog(errno, "Error: Can not create a child");
             exit(-1);
  /* - become session leader */
    case  0: if ( setsid() == -1 ) {
               writelog(errno, "Error: Can not become session leader");
               exit(-1);
             }

             switch ( fork() ) {
               case -1: writelog(errno, "Error: Can not create a child");
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

  if ( init_addr(hostnm, HOST_NAME_MAX, &sock_addr) != 0 )
    exit(-1);

  strncpy(srv_addr_str, inet_ntoa(sock_addr.sin_addr), sizeof(srv_addr_str));

  if ( bind(sock, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) != 0 ) {
    writelog(errno, "Error: Can not bind socket"); close(sock);
    exit(-1);
  }

  if ( listen(sock, BACK_LOG) == -1 ) {
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
int writelog(const int err_num, const char * cl_msg_fmt, ...) {
  FILE * logfp = NULL;
  static char log_fname[256] = {0};
  time_t now;
  struct tm * now_tm = NULL;
  char dt_fmt[64] = {0};
  char cl_msg[256] = {0};
  va_list v_lst;
  char * sys_err = NULL;

  if (strlen(log_fname) == 0 )
    sprintf(log_fname, LOG_FILE, getpid());

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
int init_addr(char * host_nm, const size_t host_nm_sz, struct sockaddr_in * addr) {
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
  char cl_addr_str[INET_ADDRSTRLEN] = {0};
  char end_comm = 0;
  char end_with_error = 1; /* true by default */
  int filed = 0;
  char * fname = NULL;
  char * fname_full = NULL;
  size_t fsize = 0;
  struct twdc_msg msg;

  strncpy(cl_addr_str, inet_ntoa(cl_addr->sin_addr), INET_ADDRSTRLEN);
  do {
    /* Read a message header */
    if ( rcv_data(cl_sock, (uint8_t *)&msg, TWDC_MSG_HEAD_SZ, MSG_PEEK) != 0 ) {
      end_comm = 1;
      break;
    }

    /* Unsupported protocol version */
    if ( !check_version_maj((struct twdc_msg_head *)&msg, TWDC_PROTO_VER_MAJOR, CT_GTOREQ) ) {
      uint8_t ver_maj = 0;
      uint8_t ver_min = 0;

      end_comm = 1;
      get_ver_info((struct twdc_msg_head *)&msg, &ver_maj, &ver_min);
      writelog(0, "Error: Rejected client '%s' because of unsuported protocol version %d.%d", cl_addr_str, ver_maj, ver_min);
      make_err_msg(&msg, TWDC_ERR_PROTO_VER, TWDC_PROTO_VER_MAJOR, TWDC_PROTO_VER_MINOR);
      snd_data(cl_sock, (uint8_t *)&msg, TWDC_MSG_ERR_FULL_SZ, 0x0);
      break;
    }

    switch ( get_msg_type(&msg.header) ) {
      /*
       * Process File Request Message
       */
      case TWDC_MSG_FILE_REQ:
        if ( rcv_data(cl_sock, (uint8_t *)&msg, TWDC_MSG_FILE_SZ, MSG_WAITALL) != 0 ) {
          end_comm = 1;
          break;
        }
        read_file_msg(&msg, (uint8_t *)fname, NAME_MAX, &fsize);
        /* Deny upload of files bigger than the hardcoded limit */
        if ( fsize > MAX_FILE_SIZE ) {
          end_comm = 1;
          writelog(0, "Error: Rejected file with size %d Bytes from '%s'", fsize, cl_addr_str);
          make_err_msg(&msg, TWDC_ERR_FILE_SZ, MAX_FILE_SIZE);
          snd_data(cl_sock, (uint8_t *)&msg, TWDC_MSG_ERR_FULL_SZ, 0x0);
        }

        if ( strlen(fname) > 0 ) {
          size_t fname_full_len = strlen(fname) + strlen(STORE_DIR) + 1;

          if ( (fname_full = malloc(fname_full_len)) != NULL ) {
            snprintf(fname_full, fname_full_len, "%s/%s", STORE_DIR, fname);

            if ( (filed = open(fname_full, O_CREAT, S_IRWXU | S_IRGRP | S_IROTH)) == -1 ) {
              writelog(errno, "Error: Cannot create file '%s' by request of client '%s'", fname_full, cl_addr_str);
              make_err_msg(&msg, TWDC_ERR_SYS);
              snd_data(cl_sock, (uint8_t *)&msg, TWDC_MSG_ERR_FULL_SZ, 0x0);
            }
            else {
              writelog(0, "Info: Accepted file '%s' (%d bytes) from '%s'", fname, fsize, cl_addr_str);
              make_err_msg(&msg, TWDC_ERR_OK);
              snd_data(cl_sock, (uint8_t *)&msg, TWDC_MSG_ERR_FULL_SZ, 0x0);
            }
          }
          else {
            writelog(errno, "Error: Can not allocate memory");
            make_err_msg(&msg, TWDC_ERR_SYS);
            snd_data(cl_sock, (uint8_t *)&msg, TWDC_MSG_ERR_FULL_SZ, 0x0);
          }
        }
        else {
          writelog(0, "Warning: Client '%s' did not specify file name", cl_addr_str);
          make_err_msg(&msg, TWDC_ERR_FILE_NM);
          snd_data(cl_sock, (uint8_t *)&msg, TWDC_MSG_ERR_FULL_SZ, 0x0);
        }
        break;
      /*
       * Process Error Message
       */
      case TWDC_MSG_ERROR: {
          int8_t err_cd = 0;

          rcv_data(cl_sock, (uint8_t *)&msg, TWDC_MSG_ERR_FULL_SZ, MSG_WAITALL);
          err_cd = get_err_code(&msg);
          if ( err_cd != TWDC_ERR_OK )
            writelog(0, "Info: Error %d received from client '%s'", err_cd, cl_addr_str);
        }
        break;
      /*
       * Process Data Message
       */
      case TWDC_MSG_DATA:
        rcv_data(cl_sock, (uint8_t *)&msg, TWDC_MSG_DATA_FULL_SZ, 0x0);
        decompress_writefile(filed, msg.body.data.buf, msg.body.data.size);
        writelog(0, "Info: Data transfer");
        break;
    }
  } while ( end_comm == 0 );

  /* Clean up on exit */
  if ( end_with_error ) {
    if ( filed > 0 )
      close(filed);
  }

  if ( fname != NULL )
    free(fname);
  if ( fname_full != NULL )
    free(fname_full);

  return 0;
}

/* Function    : decompress_writefile
 * Description : Decompress data from buffer and write it to a file
 */
int decompress_writefile(int dest, void * data, size_t data_sz) {
  static Bytef in_buf[CHUNK];
  static Bytef out_buf[CHUNK];

  /* TODO */

  return 0;
}


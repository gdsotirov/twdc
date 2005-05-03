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

/* Title: Client
 * Description: Client main programm
 * File: client.c
 * ---
 * Written by George D. Sotirov <gdsotirov@dir.bg>
 * $Id: client.c,v 1.7 2005/05/03 18:51:21 gsotirov Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <libgen.h>

/*#include <zlib.h>*/

#include "globals.h"
#include "protocol.h"
#include "client.h"

static char * progname = NULL;

/* prototypes */
void help(void);
void version(void);
void print_error(int errcd, int syserr, ...);
int print_response(struct twdc_msg_status * msg_stat);
void hr_size(int size, char * hr_str, size_t hr_str_len);

int main(int argc, char * argv[]) {
  int c = 0;
  int sock = 0;
  struct hostent * host_ent = NULL;
  struct sockaddr_in sock_addr;
  /* options */
  char fname[FNAME_LNGTH] = {0};
  struct stat fstat;
  char hostnm[HOSTNM_LNGTH] = {0};
  char hostaddr_str[16] = {0};
  unsigned short port = PORT;
  char verbose = 0; /* false */
  /* available options */
  struct option cl_optns[] = {
    {"file",    1, 0, 'f'},
    {"host",    1, 0, 'h'},
    {"port",    2, 0, 'p'},
    {"verbose", 0, 0, 'v'},
    {"version", 0, 0, 'x'},
    {"help",    0, 0, 'y'},
    {0,         0, 0,  0 }
  };
  int cl_pid = getpid();
  int filetosend = 0;
  struct twdc_msg_file file_req;
  struct twdc_msg_status msg_stat;

  progname = argv[0];

  while (1) {
    c = getopt_long(argc, argv, "f:h:p::vxy", cl_optns, NULL);
    
    if ( c == -1 )
      break;

    switch ( c ) {
      case 'f': if ( strlen(optarg) <= (FNAME_LNGTH - 1) )
                  strncpy(fname, optarg, sizeof(fname));
                else {
                  print_error(ERR_FNAME_TOO_LONG, 0, FNAME_LNGTH);
                  exit(ERR_FNAME_TOO_LONG);
                }
                break;
      case 'h': if ( strlen(optarg) <= (HOSTNM_LNGTH - 1) )
                  strncpy(hostnm, optarg, sizeof(hostnm));
                else {
                  print_error(ERR_HOSTNM_TOO_LONG, 0, HOSTNM_LNGTH);
                  exit(ERR_HOSTNM_TOO_LONG);
                }
                break;
      case 'p': if ( optarg != NULL )
                  port = (unsigned short)atoi(optarg);
                break;
      case 'v': verbose = TRUE;
                break;
      case 'x': version();
                exit(0);
                break;
      case 'y': help();
                exit(0);
                break;
      default : fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
                exit(0);
                break;
    }
  }

  if ( strlen(fname) == 0 ) {
    print_error(ERR_NO_FILE_GIVEN, 0);
    help();
    exit(ERR_NO_FILE_GIVEN);
  }

  if ((host_ent = gethostbyname(hostnm)) == NULL ) {
    print_error(ERR_CNT_RSLVE_HOST, errno, hostnm);
    exit(ERR_CNT_RSLVE_HOST);
  }

  if ( (filetosend = open(fname, O_RDONLY)) == -1 ) {
    print_error(ERR_CNT_RD_FILE, errno, fname); 
    exit(ERR_CNT_RD_FILE);
  }

  /* Get file stats */
  if ( stat(fname, &fstat) != 0 ) {
    print_error(ERR_CNT_GET_FILE_SZ, errno, fname);
    close(filetosend);
  }

  if ( (sock = socket(host_ent->h_addrtype, SOCK_STREAM, 0)) == -1 ) {
    print_error(ERR_CNT_OPEN_SOCK, errno);
    close(sock);
    exit(ERR_CNT_OPEN_SOCK);
  }

  sock_addr.sin_family = (sa_family_t)host_ent->h_addrtype;
  sock_addr.sin_port = (in_port_t)htons((uint16_t)port);
  sock_addr.sin_addr.s_addr = *(in_addr_t *)host_ent->h_addr_list[0];
  strncpy(hostaddr_str, inet_ntoa(sock_addr.sin_addr), sizeof(hostaddr_str));

  if ( verbose )
    printf("%s[%d]: Connecting to '%s:%hd' (%s:%hd)...\n", progname, cl_pid, hostnm, port, hostaddr_str, port);

  if ( connect(sock, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) != 0 ) {
    print_error(ERR_CNT_CNNCT_HOST, errno, hostnm, port, inet_ntoa(sock_addr.sin_addr), port);
    shutdown(sock, SHUT_RDWR);
    close(sock);
    exit(ERR_CNT_CNNCT_HOST);
  }

  if ( verbose )
    printf("%s[%d]: Connected to '%s:%hd' (%s:%hd)\n", progname, cl_pid, hostnm, port, hostaddr_str, port);

  /* Request file upload to the server */
  file_req.header.err_code = 0;
  file_req.header.msg_type = TWDC_MSG_FILE_REQ;
  strncpy(file_req.fname, basename(fname), sizeof(file_req.fname)); 
  file_req.fsize = fstat.st_size;
  
  if ( verbose ) {
    char hr_fsize_str[10] = {0};
    hr_size(file_req.fsize, hr_fsize_str, sizeof(hr_fsize_str));
    printf("%s[%d]: Requesting upload of file '%s' with size %d Bytes (%s)...\n", progname, cl_pid, fname, (int)fstat.st_size, hr_fsize_str);
  }

  if ( send(sock, &file_req, sizeof(file_req), 0x0) != sizeof(file_req)) {
    print_error(ERR_SND_DATA, errno, hostnm, port);
    shutdown(sock, SHUT_RDWR);
    close(sock);
    exit(ERR_SND_DATA);
  }
  
  /* Get and analyze the server response */
  if ( recv(sock, &msg_stat, sizeof(msg_stat), 0x0) != sizeof(msg_stat)) {
    print_error(ERR_RCV_DATA, errno, hostnm, port);
    shutdown(sock, SHUT_RDWR);
    close(sock);
    exit(ERR_RCV_DATA);
  }

  if ( msg_stat.header.msg_type == TWDC_MSG_ERROR ) {
    shutdown(sock, SHUT_RDWR);
    close(sock);
    exit(print_response(&msg_stat)); 
  }
  else {
    if ( verbose ) 
      printf("%s[%d]: Server '%s:%hd' accepted file '%s'.\n", progname, cl_pid, hostnm, port, fname);

    /* TODO: Send file here */

  }
  
  shutdown(sock, SHUT_RDWR);
  close(sock);

  if ( verbose )
    printf("%s[%d]: Connection to '%s:%hd' (%s:%hd) closed.\n", progname, cl_pid, hostnm, port, hostaddr_str, port);

  return 0;
}

/* Function    : help
 * Description : Print help information
 */
void help(void) {
  printf("Usage: %s [OPTIONS]\n", progname);
  printf("Client application for the TWDC server\n\n");
  printf("Options:\n");
  printf("  -f, --file=FILE  required, file to upload to the server\n");
  printf("  -h, --host=HOST  required, server host\n");
  printf("  -p, --port=PORT  optional, if not specified the default port %d will be used\n", PORT);
  printf("  -v, --verbose    optional, print more verbose messages, by default don't print\n");
  printf("      --version    print version information and exit\n");
  printf("      --help       print this help information and exit\n");
  printf("\n");
  printf("Report bugs to %s <%s>\n", AUTHOR_NAME, AUTHOR_EMAIL);
}

/* Function    : version
 * Description : Print version information
 */
void version(void) {
  printf("Client application for the Tumbleweed Developer's Contest\n");
  printf("Version %s (%s %s)\n", VERSION, __DATE__, __TIME__);
}

/* Function    : print_error
 * Description : General routine for print error messages
 */
void print_error(int errcd, int syserr, ...) {
  char errcd_fmt[256] = {0};
  char msg_fmt[512] = {0};
  va_list v_lst;

  switch ( errcd ) {
    /* Errors */
    case ERR_FNAME_TOO_LONG  :
      strncpy(errcd_fmt, ERR_FNAME_TOO_LONG_STR, sizeof(errcd_fmt));
      break;
    case ERR_HOSTNM_TOO_LONG :
      strncpy(errcd_fmt, ERR_HOSTNM_TOO_LONG_STR, sizeof(errcd_fmt));
      break;
    case ERR_NO_FILE_GIVEN :
      strncpy(errcd_fmt, ERR_NO_FILE_GIVEN_STR, sizeof(errcd_fmt));
      break;
    case ERR_CNT_RSLVE_HOST :
      strncpy(errcd_fmt, ERR_CNT_RSLVE_HOST_STR, sizeof(errcd_fmt));
      break;
    case ERR_INVLD_AF :
      strncpy(errcd_fmt, ERR_INVLD_AF_STR, sizeof(errcd_fmt));
      break;
    case ERR_CNT_RD_FILE :
      strncpy(errcd_fmt, ERR_CNT_RD_FILE_STR, sizeof(errcd_fmt));
      break;
    case ERR_CNT_GET_FILE_SZ :
      strncpy(errcd_fmt, ERR_CNT_GET_FILE_SZ_STR, sizeof(errcd_fmt));
      break;
    case ERR_CNT_OPEN_SOCK :
      strncpy(errcd_fmt, ERR_CNT_OPEN_SOCK_STR, sizeof(errcd_fmt));
      break;
    case ERR_CNT_CNNCT_HOST :
      strncpy(errcd_fmt, ERR_CNT_CNNCT_HOST_STR, sizeof(errcd_fmt));
      break;
    case ERR_SND_DATA :
      strncpy(errcd_fmt, ERR_SND_DATA_STR, sizeof(errcd_fmt));
      break;
    case ERR_RCV_DATA :
      strncpy(errcd_fmt, ERR_RCV_DATA_STR, sizeof(errcd_fmt));
      break;
    case ERR_SRV_UNKNWN :
      strncpy(errcd_fmt, ERR_SRV_UNKNWN_STR, sizeof(errcd_fmt));
      break;
    case ERR_SRV_UNEXPCTD :
      strncpy(errcd_fmt, ERR_SRV_UNEXPCTD_STR, sizeof(errcd_fmt));
      break;
    case ERR_SRV_FILE_SZ :
      strncpy(errcd_fmt, ERR_SRV_FILE_SZ_STR, sizeof(errcd_fmt));
      break;
    /* Warnings */
    case WARN_ZERO_FILE :
      strncpy(errcd_fmt, WARN_ZERO_FILE_STR, sizeof(errcd_fmt));
      break;
    default: break;
  }

  if ( errcd < 0 )
    sprintf(msg_fmt, "%s[%d]: Error: %s ", progname, getpid(), errcd_fmt);
  else if ( errcd > 0 ) 
    sprintf(msg_fmt, "%s[%d]: Warning: %s ", progname, getpid(), errcd_fmt);

  va_start(v_lst, syserr);
  vfprintf(stderr, msg_fmt, v_lst);

  if ( errno != 0 )
    perror("System error");
  else
    fprintf(stderr, "\n");
}

/* Function    : print_response
 * Description : Print the server response to the client terminal
 */
int print_response(struct twdc_msg_status * msg_stat) {
  switch ( msg_stat->header.err_code ) {
    case TWDC_ERR_UNEXPCTD_MSG : 
      print_error(ERR_SRV_UNEXPCTD, 0);
      return ERR_SRV_UNEXPCTD;
    case TWDC_ERR_FILE_SZ : {
        int max_sz = *(int *)&msg_stat->data;
        char max_sz_str[10] = {0};

        hr_size(max_sz, max_sz_str, sizeof(max_sz_str));
        print_error(ERR_SRV_FILE_SZ, 0, max_sz, max_sz_str);
      }
      return ERR_SRV_FILE_SZ;
    default:
      print_error(ERR_SRV_UNKNWN, 0);
      return ERR_SRV_UNKNWN;
  }
}

/* Function    : hr_size
 * Description : Human readable representation of file size
 */
void hr_size(int size, char * hr_str, size_t hr_str_len) {
  char * sz_arr[] = {"B ", "KB", "MB", "GB", "TB", "PB", "EB"};
  double sz = size;
  int i = 0;
  
  while ( sz > 1000 ) {
    ++i;
    sz /= 1024;
  }
  snprintf(hr_str, hr_str_len, "%1.2f %s", sz, sz_arr[i]);
}


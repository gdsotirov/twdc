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
 * $Id: client.c,v 1.4 2005/04/26 16:42:13 gsotirov Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>

#include "globals.h"
#include "protocol.h"
#include "client.h"

static char * progname = NULL;

/* prototypes */
void help(void);
void version(void);
void print_error(int errcd, int syserr, ...);

int main(int argc, char * argv[]) {
  int c = 0;
  /* option values */
  char fname[FNAME_LNGTH] = {0};
  char host[HOSTNM_LNGTH] = {0};
  int port = 0;
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
                  exit(-1);
                }
                break;
      case 'h': if ( strlen(optarg) <= (HOSTNM_LNGTH - 1) )
                  strncpy(host, optarg, sizeof(host));
                else {
                  print_error(ERR_HOSTNM_TOO_LONG, 0, HOSTNM_LNGTH);
                  exit(-1);
                }
                break;
      case 'p': if ( optarg != NULL )
                  port = atoi(optarg);
                else
                  port = PORT;
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

  printf("Filename = %s\n", fname);
  printf("Server:Port = %s:%d\n", host, port);
  printf("Verbose = %s\n", verbose?"true":"false");

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
    case ERR_FNAME_TOO_LONG  :
      strncpy(errcd_fmt, ERR_FNAME_TOO_LONG_STR, sizeof(errcd_fmt));
      break;
    case ERR_HOSTNM_TOO_LONG :
      strncpy(errcd_fmt, ERR_HOSTNM_TOO_LONG_STR, sizeof(errcd_fmt));
      break;
    default: break;
  }

  if ( errcd < 0 )
    sprintf(msg_fmt, "%s[%d]: Error: %s\n", progname, getpid(), errcd_fmt);
  else if ( errcd > 0 ) 
     sprintf(msg_fmt, "%s[%d]: Warning: %s\n", progname, getpid(), errcd_fmt);

  va_start(v_lst, syserr);
  vfprintf(stderr, msg_fmt, v_lst);

  if ( errno != 0 ) {
    perror("System error: ");
  }
}


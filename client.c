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
 * $Id: client.c,v 1.15 2005/05/17 20:15:34 gsotirov Exp $
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
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <libgen.h>

/* Third party library includes */
#include <zlib.h>

#include "globals.h"
#include "protocol.h"
#include "data.h"
#include "client.h"

#define CHUNK 16384

static char * progname = NULL;

/* prototypes */
void help(void);
void version(void);
void print_error(const int errcd, const int syserr, ...);
int print_response(const struct twdc_msg * msg_stat);
void hr_size(const int size, char * hr_str, const size_t hr_str_len);
int compress_sendfile(int source_fd, int dest_fd);
double calc_duration(struct timeval * end, struct timeval * begin);

int main(int argc, char * argv[]) {
  int c = 0;
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
  /* option values */
  char * fname = NULL;
  char * fbname = NULL;
  char * hostnm = NULL;
  unsigned short port = PORT;
  char verbose = FALSE;
  /* globals */
  struct hostent * host_ent = NULL;
  struct stat fstat;
  int filetosend = 0;
  int sock = 0;
  struct sockaddr_in sock_addr;
  char hostaddr_str[INET_ADDRSTRLEN] = {0};
  int cl_pid = getpid();
  struct twdc_msg msg;
  int child_pid = 0;
  int pipe_fd[2] = {0};
  int transfered = 0;
  struct timeval transf_beg;
  struct timeval transf_end;

  progname = argv[0];

  while (1) {
    c = getopt_long(argc, argv, "f:h:p::vxy", cl_optns, NULL);

    if ( c == -1 )
      break;

    switch ( c ) {
      case 'f': {
          size_t fname_len = strlen(optarg);
          size_t fbname_len = 0;

          if ( fname_len > 0 && fname_len < MAXPATHLEN ) {
            if ( (fname = malloc(fname_len + 1)) != NULL )
              strncpy(fname, optarg, fname_len + 1);
          }
          else {
            print_error(ERR_FPATH_ERR, 0, MAXPATHLEN);
            exit(ERR_FPATH_ERR);
          }

          fbname_len = strlen(basename(fname));
          if ( fbname_len > 0 && fbname_len < NAME_MAX ) {
            if ( (fbname = malloc(fbname_len + 1)) != NULL )
              strncpy(fbname, basename(fname), fbname_len + 1);
          }
          else {
            print_error(ERR_FNAME_ERR, 0, NAME_MAX);
            exit(ERR_FNAME_ERR);
          }
        }
        break;
      case 'h': {
          size_t hostnm_len = strlen(optarg);

          if ( hostnm_len > 0 && hostnm_len < MAXHOSTNAMELEN ) {
            if ( (hostnm = malloc(hostnm_len + 1)) != NULL )
              strncpy(hostnm, optarg, hostnm_len + 1);
          }
          else {
            print_error(ERR_HOSTNM_ERR, 0, MAXHOSTNAMELEN);
            exit(ERR_HOSTNM_ERR);
          }
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
      default:  fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
                exit(0);
                break;
    }
  }

  if ( fname == NULL || strlen(fname) == 0 ) {
      print_error(ERR_NO_FILE_GIVEN, 0);
      help();
      exit(ERR_NO_FILE_GIVEN);
  }

  if ( fbname == NULL || !strcmp(fbname, "/") || !strcmp(fbname, ".") || !strcmp(fbname,"..") ) {
    print_error(ERR_INVLD_FILE_NM, 0, fbname);
    exit(ERR_INVLD_FILE_NM);
  }

  if ( hostnm == NULL || strlen(hostnm) == 0 ) {
    print_error(ERR_NO_HOST_GIVEN, 0);
    help();
    exit(ERR_NO_HOST_GIVEN);
  }

  if ((host_ent = gethostbyname(hostnm)) == NULL ) {
    print_error(ERR_CNT_RSLVE_HOST, h_errno, hostnm);
    exit(ERR_CNT_RSLVE_HOST);
  }

  if ( stat(fname, &fstat) != 0 ) {
    print_error(ERR_CNT_GET_FILE_SZ, errno, fname);
    close(filetosend);
    exit(ERR_CNT_GET_FILE_SZ);
  }

  if ( fstat.st_size == 0 )
    print_error(WARN_ZERO_FILE, 0);

  if ( (filetosend = open(fname, O_RDONLY)) == -1 ) {
    print_error(ERR_CNT_RD_FILE, errno, fname);
    exit(ERR_CNT_RD_FILE);
  }

  if ( (sock = socket(host_ent->h_addrtype, SOCK_STREAM, 0)) == -1 ) {
    print_error(ERR_CNT_OPEN_SOCK, errno);
    close(filetosend);
    close(sock);
    exit(ERR_CNT_OPEN_SOCK);
  }

  sock_addr.sin_family = (sa_family_t)host_ent->h_addrtype;
  sock_addr.sin_port = (in_port_t)htons((uint16_t)port);
  sock_addr.sin_addr.s_addr = *(in_addr_t *)host_ent->h_addr_list[0];
  strncpy(hostaddr_str, (char *)inet_ntoa(sock_addr.sin_addr), sizeof(hostaddr_str));

  if ( verbose )
    printf("%s[%d]: Connecting to '%s:%hd' (%s:%hd)...\n", progname, cl_pid, hostnm, port, hostaddr_str, port);

  if ( connect(sock, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) != 0 ) {
    print_error(ERR_CNT_CNNCT_HOST, errno, hostnm, port, inet_ntoa(sock_addr.sin_addr), port);
    shutdown(sock, SHUT_RDWR);
    close(filetosend);
    close(sock);
    exit(ERR_CNT_CNNCT_HOST);
  }

  if ( verbose )
    printf("%s[%d]: Connected to '%s:%hd' (%s:%hd)\n", progname, cl_pid, hostnm, port, hostaddr_str, port);

  /* Request file upload to the server */
  make_file_msg(&msg, (uint8_t *)fbname, fstat.st_size);

  if ( verbose ) {
    char hr_fsize_str[10] = {0};
    hr_size(fstat.st_size, hr_fsize_str, sizeof(hr_fsize_str));
    printf("%s[%d]: Requesting upload of file '%s' with size %d Bytes (%s)...\n", progname, cl_pid, fname, (size_t)fstat.st_size, hr_fsize_str);
  }

  if ( snd_data(sock, (uint8_t *)&msg, TWDC_MSG_FILE_FULL_SZ, 0x0) != 0 ) {
    print_error(ERR_SND_DATA, errno, hostnm, port);
    shutdown(sock, SHUT_RDWR);
    close(filetosend);
    close(sock);
    exit(ERR_SND_DATA);
  }

  /* Get and analyze the server response */
  if ( rcv_data(sock, (uint8_t *)&msg, TWDC_MSG_ERR_FULL_SZ, 0x0) != 0 ) {
    print_error(ERR_RCV_DATA, errno, hostnm, port);
    shutdown(sock, SHUT_RDWR);
    close(filetosend);
    close(sock);
    exit(ERR_RCV_DATA);
  }

  if ( get_msg_type((struct twdc_msg_head *)&msg) == TWDC_MSG_ERROR ) {
    if ( get_err_code(&msg) == TWDC_ERR_OK ) {
      if ( verbose )
        printf("%s[%d]: Server '%s:%hd' accepted file '%s'.\n", progname, cl_pid, hostnm, port, fname);
    }
    else {
      shutdown(sock, SHUT_RDWR);
      close(filetosend);
      close(sock);
      exit(print_response(&msg));
    }
  }

  if ( pipe(pipe_fd) ) {
    print_error(ERR_PIPE, errno);
    close(filetosend);
    shutdown(sock, SHUT_RDWR);
    close(sock);
    exit(ERR_PIPE);
  }

  /* The child process will read data from the file, compress it and send it
   * through pipe to the parent process.
   * The parent process will read compressed data from the pipe and will send
   * to over the network to the server.
   */
  child_pid = fork();
  switch ( child_pid ) {
    case -1:
        print_error(ERR_CHILD, errno);
        close(filetosend);
        shutdown(sock, SHUT_RDWR);
        close(sock);
        exit(ERR_CHILD);
    case  0: {
        int ret = ERR_OK;

        close(pipe_fd[0]); /* the child will only write to the pipe */

        ret = compress_sendfile(filetosend, pipe_fd[1]);
        if ( ret != ERR_OK ) {
          if ( ret == ERR_ZLIB )
            print_error(ret, Z_ERRNO);
          else
            print_error(ret, errno);
        }

        close(pipe_fd[1]);
        exit(ret);
      }
    default: {
        int status = 0;
        uint8_t data[TWDC_DATA_MAX];
        size_t br = TWDC_DATA_MAX; /* bytes read */

        close(pipe_fd[1]); /* the parent will only read from the pipe */

        gettimeofday(&transf_beg, NULL);
        do {
          if ( read_data(pipe_fd[0], data, &br) != 0 ) {
            print_error(ERR_RD_DATA, errno);
            make_err_msg(&msg, TWDC_ERR_SYS);
            snd_data(sock, (uint8_t *)&msg, TWDC_MSG_ERR_FULL_SZ, 0x0);
            break;
          }

          /* pack the compressed data and send it */
          make_data_msg(&msg, data, br);
          if ( snd_data(sock, (uint8_t *)&msg, TWDC_MSG_FILE_FULL_SZ, 0x0) != 0 ) {
            print_error(ERR_SND_DATA, errno, hostnm, port);
            make_err_msg(&msg, TWDC_ERR_SYS);
            snd_data(sock, (uint8_t *)&msg, TWDC_MSG_ERR_FULL_SZ, 0x0);
            break;
          }

          transfered += br;

          if ( br < TWDC_DATA_MAX || br == 0 )
            break;
          else
            br = TWDC_DATA_MAX;
        } while ( 1 );

        gettimeofday(&transf_end, NULL);
        waitpid(child_pid, &status, 0x0);
        close(pipe_fd[0]);
      }
      break;
  }

  /* print information messages on successful transfer */
  {
  char size_str[10] = {0};
  double dur = calc_duration(&transf_end, &transf_beg);
  double cmpr_rate = transfered/fstat.st_size * 100;
  double speed =

  printf("%s[%d]: Successful transfer\n", progname, cl_pid);
  printf("%s[%d]: File: %s\n", progname, cl_pid, fname);
  hr_size(fstat.st_size, size_str, sizeof(size_str));
  printf("%s[%d]: Size: %d Bytes (%s)\n", progname, cl_pid, (size_t)fstat.st_size, size_str);
  hr_size(transfered, size_str, sizeof(size_str));
  printf("%s[%d]: Transfered: %d Bytes (%s)\n", progname, cl_pid, transfered, size_str);
  printf("%s[%d]: Duration: %1.3f seconds\n", progname, cl_pid, dur);
  hr_size(speed, size_str, sizeof(size_str));
  printf("%s[%d]: Transfer speed: %s/s\n", progname, cl_pid, size_str);
  printf("%s[%d]: Compression rate: %1.2f %%\n", progname, cl_pid, cmpr_rate);
  }

  close(filetosend);
  shutdown(sock, SHUT_RDWR);
  close(sock);

  if ( verbose )
    printf("%s[%d]: Connection to '%s:%hd' (%s:%hd) closed.\n", progname, cl_pid, hostnm, port, hostaddr_str, port);

  if ( hostnm != NULL )
    free(hostnm);
  if ( fname != NULL )
    free(fname);
  if ( fbname != NULL )
    free(fbname);

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
void print_error(const int errcd, const int syserr, ...) {
  char errcd_fmt[256] = {0};
  char msg_fmt[512] = {0};
  va_list v_lst;

  switch ( errcd ) {
    /* Errors */
    case ERR_FPATH_ERR:
      strncpy(errcd_fmt, ERR_FPATH_ERR_STR, sizeof(errcd_fmt));
      break;
    case ERR_FNAME_ERR:
      strncpy(errcd_fmt, ERR_FNAME_ERR_STR, sizeof(errcd_fmt));
      break;
    case ERR_HOSTNM_ERR:
      strncpy(errcd_fmt, ERR_HOSTNM_ERR_STR, sizeof(errcd_fmt));
      break;
    case ERR_NO_FILE_GIVEN:
      strncpy(errcd_fmt, ERR_NO_FILE_GIVEN_STR, sizeof(errcd_fmt));
      break;
    case ERR_INVLD_FILE_NM:
      strncpy(errcd_fmt, ERR_INVLD_FILE_NM_STR, sizeof(errcd_fmt));
      break;
    case ERR_NO_HOST_GIVEN:
      strncpy(errcd_fmt, ERR_NO_HOST_GIVEN_STR, sizeof(errcd_fmt));
      break;
    case ERR_CNT_RSLVE_HOST:
      strncpy(errcd_fmt, ERR_CNT_RSLVE_HOST_STR, sizeof(errcd_fmt));
      break;
    case ERR_INVLD_AF:
      strncpy(errcd_fmt, ERR_INVLD_AF_STR, sizeof(errcd_fmt));
      break;
    case ERR_CNT_RD_FILE:
      strncpy(errcd_fmt, ERR_CNT_RD_FILE_STR, sizeof(errcd_fmt));
      break;
    case ERR_CNT_GET_FILE_SZ:
      strncpy(errcd_fmt, ERR_CNT_GET_FILE_SZ_STR, sizeof(errcd_fmt));
      break;
    case ERR_CNT_OPEN_SOCK:
      strncpy(errcd_fmt, ERR_CNT_OPEN_SOCK_STR, sizeof(errcd_fmt));
      break;
    case ERR_CNT_CNNCT_HOST:
      strncpy(errcd_fmt, ERR_CNT_CNNCT_HOST_STR, sizeof(errcd_fmt));
      break;
    case ERR_SND_DATA:
      strncpy(errcd_fmt, ERR_SND_DATA_STR, sizeof(errcd_fmt));
      break;
    case ERR_RCV_DATA:
      strncpy(errcd_fmt, ERR_RCV_DATA_STR, sizeof(errcd_fmt));
      break;
    case ERR_WR_DATA:
      strncpy(errcd_fmt, ERR_WR_DATA_STR, sizeof(errcd_fmt));
      break;
    case ERR_RD_DATA:
      strncpy(errcd_fmt, ERR_RD_DATA_STR, sizeof(errcd_fmt));
      break;
    case ERR_PIPE:
      strncpy(errcd_fmt, ERR_PIPE_STR, sizeof(errcd_fmt));
      break;
    case ERR_CHILD:
      strncpy(errcd_fmt, ERR_CHILD_STR, sizeof(errcd_fmt));
      break;
    case ERR_ZLIB:
      strncpy(errcd_fmt, ERR_ZLIB_STR, sizeof(errcd_fmt));
      break;
    /* Protocol (server) errors */
    case ERR_SRV_UNKNWN:
      strncpy(errcd_fmt, ERR_SRV_UNKNWN_STR, sizeof(errcd_fmt));
      break;
    case ERR_SRV_PROTO_VER:
      strncpy(errcd_fmt, ERR_SRV_PROTO_VER_STR, sizeof(errcd_fmt));
      break;
    case ERR_SRV_FILE_SZ:
      strncpy(errcd_fmt, ERR_SRV_FILE_SZ_STR, sizeof(errcd_fmt));
      break;
    case ERR_SRV_FILE_NM:
      strncpy(errcd_fmt, ERR_SRV_FILE_NM_STR, sizeof(errcd_fmt));
      break;
    case ERR_SRV_SYS:
      strncpy(errcd_fmt, ERR_SRV_SYS_STR, sizeof(errcd_fmt));
      break;
    /* Warnings */
    case WARN_ZERO_FILE:
      strncpy(errcd_fmt, WARN_ZERO_FILE_STR, sizeof(errcd_fmt));
      break;
    case 0: return;
    default: break;
  }

  if ( errcd < 0 ) {
    if ( errcd < -300 )
      sprintf(msg_fmt, "%s[%d]: Error: Server: %s", progname, getpid(), errcd_fmt);
    else
      sprintf(msg_fmt, "%s[%d]: Error: %s", progname, getpid(), errcd_fmt);
  }
  else if ( errcd > 0 )
    sprintf(msg_fmt, "%s[%d]: Warning: %s", progname, getpid(), errcd_fmt);

  va_start(v_lst, syserr);
  vfprintf(stderr, msg_fmt, v_lst);
  va_end(v_lst);

  if ( syserr != 0 ) {
    switch ( errcd ) {
      case ERR_CNT_RSLVE_HOST:
        herror("; Resolve error");
        break;
      case ERR_ZLIB:
        fprintf(stderr, "; Zlib error: %s\n", zError(Z_ERRNO));
        break;
      default:
        perror("; System error");
        break;
    }
  }
  else
    fprintf(stderr, "\n");
}

/* Function    : print_response
 * Description : Print the server response to the client terminal
 * Parameters  : msg_err - error code of the message
 * Return      : The value of the passed msg_err
 */
int print_response(const struct twdc_msg * msg_err) {
  int err_cd = ERR_OK;

  switch ( err_cd = get_err_code(msg_err) ) {
    case TWDC_ERR_PROTO_VER: {
        uint8_t srv_ver_maj = 0;
        uint8_t srv_ver_min = 0;

        get_ver_info((struct twdc_msg_head *)msg_err, &srv_ver_maj, &srv_ver_min);
        print_error(ERR_SRV_PROTO_VER, 0, TWDC_PROTO_VER_MAJOR, TWDC_PROTO_VER_MINOR, srv_ver_maj, srv_ver_min);
      }
      return ERR_SRV_PROTO_VER;
    case TWDC_ERR_FILE_SZ: {
        int max_sz = 0;
        char max_sz_str[10] = {0};

        read_err_msg(msg_err, &max_sz);
        hr_size(max_sz, max_sz_str, sizeof(max_sz_str));
        print_error(ERR_SRV_FILE_SZ, 0, max_sz, max_sz_str);
      }
      return ERR_SRV_FILE_SZ;
    default:
      if ( err_cd == TWDC_ERR_OK || err_cd == TWDC_ERR_FILE_NM || err_cd == TWDC_ERR_SYS )
        print_error(err_cd, 0);
      else {
        print_error(ERR_SRV_UNKNWN, 0, ERR_SRV_UNKNWN);
        err_cd = ERR_SRV_UNKNWN;
      }
      return err_cd;
  }
}

/* Function    : hr_size
 * Description : Human readable representation of file size
 */
void hr_size(const int size, char * hr_str, const size_t hr_str_len) {
  char * sz_arr[] = {"B ", "KB", "MB", "GB", "TB", "PB", "EB"};
  double sz = size;
  int i = 0;

  while ( sz > 1000 ) {
    ++i;
    sz /= 1024;
  }
  snprintf(hr_str, hr_str_len, "%1.2f %s", sz, sz_arr[i]);
}

/* Function    : compress_sendfile
 * Description : Transfer compressed data between files
 * Parameters  : s_fd - source file descriptor
 *               d_fd - destination file descriptor
 */
int compress_sendfile(int s_fd, int d_fd) {
  Bytef in_buf[CHUNK] = {0};
  Bytef out_buf[CHUNK] = {0};
  size_t in_buf_len = CHUNK;
  z_stream zstrm;
  int zret = 0;
  int flush = 0;

  zstrm.zalloc = Z_NULL;
  zstrm.zfree  = Z_NULL;
  zstrm.opaque = Z_NULL;

  zret = deflateInit(&zstrm, Z_DEFAULT_COMPRESSION);

  if ( zret != Z_OK )
    return ERR_ZLIB;

  do {
    if ( read_data(s_fd, in_buf, &in_buf_len) != 0 ) {
      (void)deflateEnd(&zstrm);
      return ERR_RD_DATA;
    }

    flush = (in_buf_len < CHUNK || in_buf_len == 0 ) ? Z_FINISH : Z_NO_FLUSH;

    zstrm.avail_in = in_buf_len;
    zstrm.next_in = in_buf;

    do {
      zstrm.avail_out = CHUNK;
      zstrm.next_out = out_buf;

      zret = deflate(&zstrm, flush);

      if ( zret != Z_STREAM_ERROR ) {
        size_t have = CHUNK - zstrm.avail_out;
        size_t bw = have;
        int ret = write_data(d_fd, out_buf, &bw);

        if ( ret != 0 || bw < have ) {
          (void)deflateEnd(&zstrm);
          return ERR_WR_DATA;
        }
      }
      else
        return ERR_ZLIB;
    } while (zstrm.avail_out == 0);
  } while (flush != Z_FINISH);

  return 0;
}

/* Function    : calc_duration
 * Description : Subract two time values
 * Raturn      : The duration in floating point seconds
 */
double calc_duration(struct timeval * end, struct timeval * begin) {
  struct timeval result;

  if ( end->tv_usec - begin->tv_usec > 1000000 ) {
    int nsec = (end->tv_usec - begin->tv_usec) / 1000000;
    begin->tv_usec += 1000000 * nsec;
    begin->tv_sec -= nsec;
  }

  result.tv_sec = end->tv_sec - begin->tv_sec;
  result.tv_usec = end->tv_usec - begin->tv_usec;

  if ( end->tv_sec > begin->tv_sec )
    return (result.tv_sec + result.tv_usec * 1e-6);
  return 0.0;
}

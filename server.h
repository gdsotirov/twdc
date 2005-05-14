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
 * Description: Server configuraiton
 * File: server.h
 * ---
 * Written by George D. Sotirov <gdsotirov@dir.bg>
 * $Id: server.h,v 1.7 2005/05/14 22:24:54 gsotirov Exp $
 */

#ifndef __TWDC_SERVER_H__
#define __TWDC_SERVER_H__

#define BACK_LOG 10

/* Maximum file size in bytes the server should accept
 * Default: 10 MB
 */
#define MAX_FILE_SIZE 10485760 /* Bytes */

/* The direcotry in which the uploaded files wlll be
 * stored.
 */
#define STORE_DIR "/tmp/store"

/* The directory where the temporary files will be
 * stored
 */
#define TEMP_DIR "/tmp"

/* Where should all messages go */
#define LOG_FILE "/tmp/server-%d.log"

#endif /* __TWDC_SERVER_H__ */


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
 * Description: Server configuraiton
 * File: server.h
 * ---
 * Written by George D. Sotirov <gdsotirov@dir.bg>
 * $Id: server.h,v 1.3 2005/04/25 21:00:26 gsotirov Exp $
 */

#ifndef __TWT_SERVER_H__
#define __TWT_SERVER_H__

#define BACKLOG 10

/* Maximum file size in bytes the server should accept
 * Default: 10 MB
 */
#define MAXFILESIZE 10485760 /* Bytes */

/* The port on which the server should operate */
#define PORT 9919

/* Where should all messages go */
#define LOGFILE "/tmp/server-%d.log"

#endif


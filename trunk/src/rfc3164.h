/***************************************************************************
 *   Copyright (C) 2004 by Peter Snoblin                                   *
 *   pas@truman.edu                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef RFC3164_H
#define RFC3164_H

#define RFC3164_PORT 514
#define RFC3164_PACKET_LENGTH 1024
#define RFC3164_DATE_LEN 15

#include "cache.h"
#include "syslog.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

//! This class provides an RFC3164 compliant interface.
/*!
	RFC3164 sets forth the semantics for syslog communication via UDP. It is a very simple scheme, and this class handles all aspects of it, from setting up the networking, to passing the log strings off to the caching module.
*/
class rfc3164
{
public:
	rfc3164( cache *s );
	rfc3164(int port, cache *s );
	~rfc3164();
	
	void listen();

private:
	void create_socket(int port);
	struct syslog_message *parse_message(char *message);
	int sock;
	cache *c;
};

struct rfc3164_args {
	cache *c;
	int port;
};

void *rfc3164_launch_thread(void *arg);

struct rfc3164_args *rfc3164_build_args( cache *c, int port );


#endif

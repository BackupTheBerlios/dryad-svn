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
#include "dryerr.h"
#include "functions.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

namespace RFC3164
{
using std::cerr;
using std::endl;
using DCache::cache;
using Syslog::syslog_message;
using DString::dstring;
using DError::dryerr;
using DFunctions::itoa;

//! This class provides an RFC3164 compliant interface.
/*!
	RFC3164 sets forth the semantics for syslog communication via UDP. It is a very simple scheme, and this class handles all aspects of it, from setting up the networking, to passing the log strings off to the caching module.
*/
class rfc3164
{
public:
	//! The basic constructor
	/*!
		\param s The cache object to use. This MUST be a valid object! It is integral to things working!
		If this constructor is used, the default syslog will be used.
	*/
	rfc3164( cache *s );
	//! Another constructor
	/*!
		\param port The port to bind, if you want to use a non default one
		\param s The cache object to use. MUST NOT be NULL!
	*/
	rfc3164(int port, cache *s );
	~rfc3164();
	
	//! Starts the listening process
	/*!
		This call does not return, but it also should not be directly called. Instead use rfc3164_launch_thread.
	*/
	void listen();

private:
	//! Creates the socket, used by both constructors
	void create_socket(int port);
	//! Used to parse the message.
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

}
#endif

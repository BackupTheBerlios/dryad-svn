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
#ifndef NETWORK_H
#define NETWORK_H

#include "dstring.h"
#include "dqueue.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

//! The network object
/*!
	This is the class that controls the network portion of dryad. It sets up the listening sockets and proceeds to listen until it can accept no more, for whatever reason. It also starts the network helper threads. Once you start listening here, the function does not return, thus it should be started as a thread of it's own.
*/
class network {
public:
	//! Constructor
	/*!
		\param p The port to bind to.
		\param queue The queue to use to pass messages
		\param threads The number of threads to use
	*/
	network(int p, cache *queue, int threads);
	~network();
	
	//! Start listening
	/*!
		Starts listening for connections, passing the connection off to a thread as needed. This method does not return.
	*/
	void start_listening();

private:
	dfifo<int> *sockets;
	cache *mqueue;
	int nthreads;
	pthread_t t;
	int port;
};

//! This is the class that actually deals with connections, once they've been accept()ed and enqueued.
class network_helper {
public:
	//! Basic constructor
	/*!
		\param q The queue that it is to process on.
	*/
	network_helper( dfifo<int> *q, cache *c );
	~network_helper();
	//! Starts the process
	/*!
		This method does not return.
	*/
	void do_your_thing();
private:
	dfifo<int> *f;
	cache *mqueue;
};

struct netargs {
	dfifo<int> *q;
	cache *c;
};

void *threads_network_start(void* args);
struct netargs* threads_network_buildargs(dfifo<int> *q, cache *c);

#endif

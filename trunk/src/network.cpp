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
#include "network.h"

network::network(int p, cache *queue, int threads)
{
	threads > 0 ? nthreads = threads : nthreads = 1;
	//t = (pthread_t **)malloc(sizeof(pthread_t) * nthreads);
	mqueue = queue;
	sockets = new dfifo<int>;
	port = p;
}

network::~network()
{
	
}

void network::start_listening()
{
	int sock, aid, l;
	struct sockaddr_in info, remote_info;
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if( -1 == sock )
	{
		cerr << "Failed to create socket!\nAborting!\n";
		exit(1);
	}
	
	info.sin_family = AF_INET;
	info.sin_port = htons(port);
	info.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(info.sin_zero), '\0', 8);
	if( -1 == bind( sock, (struct sockaddr*)&info, sizeof(struct sockaddr) ) )
	{
		cerr << "Unable to bind port " << port << ".\nAborting!\n";
		exit(1);
	}
	
	if( -1 == listen(sock, nthreads *2) )
	{
		cerr << "Unable to listen on port " << port << ".\nAborting!\n";
		exit(1);
	}
	for( int c = 0; c < nthreads; c++ )
	{
		pthread_create(&t, NULL, threads_network_start, threads_network_buildargs(sockets, mqueue) );
	}
	l = sizeof( struct sockaddr );
	while( -1 != ( aid = accept( sock, (struct sockaddr*)&remote_info, (socklen_t*)&l ) ) )
	{
		sockets->enqueue(aid);
		//we should also check here to ensure that all our threads are running, and if not, restart them
	}
	cerr << "accept() failed!\nAborting!\n";
	exit(1);
}

// Here begins the network_helper class
network_helper::network_helper( dfifo<int> *q, cache *c )
{
	f = q;
	mqueue = c;
}

network_helper::~network_helper()
{

}

void network_helper::do_your_thing()
{
	int sock;
	while(true)
	{
		sock = f->dequeue();
		// I don't think sleep is thread safe, this should get changed at some point, but for the time being it works
		if( sock == NULL )
		{
			sleep(1);
			continue;
		}
		//do the actualy syslog() protocol stuff here that I am far to lazy to write.
	}
}

void *threads_network_start(void* args)
{
	struct netargs *a = (struct netargs*)args;
	network_helper *h;
	h = new network_helper( a->q, a->c );
	h->do_your_thing();
}
struct netargs* threads_network_buildargs(dfifo<int> *q, cache *c)
{
	struct netargs *ret;
	
	ret = (struct netargs*)malloc(sizeof(struct netargs));
	ret->c = c;
	ret->q = q;
	return ret;
}

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
	mqueue = queue;
	sockets = new dfifo<int>;
	port = p;
}

void network::start_listening()
{
	int sock, aid, l;
	struct sockaddr_in info, remote_info;
	
	sock = socket(PF_INET, SOCK_DGRAM, 0);
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
	
	if( -1 == listen(sock, nthreads * 5) )
	{
		cerr << "Unable to listen on port " << port << ".\nAborting!\n";
		exit(1);
	}
	l = sizeof( struct sockaddr );
	while( -1 != ( aid = accept( sock, (struct sockaddr*)&remote_info, (socklen_t*)&l ) ) )
	{
		sockets->enqueue(aid);
	}
	cerr << "accept() failed!\nAborting!\n";
	exit(1);
}

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

#include <sys/types.h>
#include <sys/socket.h>

typedef struct {
	int *fd;
} squeue;

class network {
public:
	//! Constructor
	/*!
		\param port The port to bind to.\
		\param queue The queue to use to pass messages
	*/
	network(int port, dstring **queue);
	~network();
	
	//! Start listening
	/*!
		Starts listening for connections, passing the connection off to a thread as needed. This method does not return.
	*/
	void start_listening();

private:
	squeue **sockets;
};

#endif

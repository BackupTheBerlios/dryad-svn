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
#include "rfc3164.h"
namespace RFC3164
{
rfc3164::rfc3164( cache *s )
{
	create_socket(RFC3164_PORT);
	c = s;
}
rfc3164::rfc3164(int port, cache *s )
{
	create_socket(port);
	c = s;
}
rfc3164::~rfc3164()
{
	close(sock);
}
	
void rfc3164::listen()
{
	char *buf;
	int ret, len;
	struct sockaddr_in *host;
	struct syslog_message *m;
	
	buf = (char*)malloc(RFC3164_PACKET_LENGTH);
	len = sizeof(struct sockaddr);
	host = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
	
	while(1)
	{
		/* I *think* it's safe to no longer use MSG_TRUNC here. The Linux documentation only says "sometimes the remander of the message will be truncated if you don't grab it all. The Microsoft Winsock2 documentation (I know, but whatever, they actually know how to properly document their stuff) states that for UDP sockets, if you do not get it all, the rest shall be discarded. I'm hazarding a guess that this is a consistent behavior. */
		ret = recvfrom(sock, buf, RFC3164_PACKET_LENGTH, 0, (struct sockaddr*)host, (socklen_t*)&len);
		if( -1 == ret )
		{
			//an error occured, so we are just going to start over
			free(buf);
			free(host);
			buf = (char*)malloc(RFC3164_PACKET_LENGTH);
			host = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
			continue;
		}
		#ifdef DEBUG
		cerr << "Recieved message: " << buf << endl;
		#endif
		m = parse_message(buf);
		free(buf);
		free(host);
		/* see above note about MSG_TRUNC for an explanation of this code deletage */
		
		c->add(m);
		free(m);
		
		buf = (char*)malloc(RFC3164_PACKET_LENGTH);
		host = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
	}
}

void rfc3164::create_socket(int port)
{
	struct sockaddr_in *mine;
	
	sock = socket( AF_INET, SOCK_DGRAM, 0 );
	if( -1 == sock )
	{
		cerr << "Unable to create a socket in rfc3164!\nAborting!\n";
		exit(1);
	}
	mine = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
	mine->sin_family = AF_INET;
	mine->sin_port = htons(port);
	mine->sin_addr.s_addr = htonl(INADDR_ANY);
	memset(mine->sin_zero, '\0', 8);
	if( -1 == bind(sock, (struct sockaddr*)mine, sizeof(struct sockaddr)) )
	{
		cerr << "Unable to bind port " << port << " in rfc3164!\nAborting!\n";
		exit(1);
	}
	free(mine);
}

struct syslog_message *rfc3164::parse_message(char *message)
{
	int pri, d;
	char *tmp;
	struct syslog_message *m;
	
	m = (struct syslog_message*)malloc(sizeof(struct syslog_message));
	
	m->date = NULL;
	m->facility = 0;
	m->host = NULL;
	m->message = NULL;
	m->severity = 0;
	
	if( message[0] != '<' )
	{
		free(m);
		return NULL;
	}
	if( message[2] == '>' )
	{
		tmp = (char*)malloc(sizeof(char));
		tmp[0] = message[1];
		pri = atoi(tmp);
		free(tmp);
		d = 3;
	}
	else if( message[3] == '>' )
	{
		tmp = (char*)malloc(sizeof(char)*2);
		tmp[0] = message[1]; tmp[1] = message[2];
		pri = atoi(tmp);
		free(tmp);
		d = 4;
	}
	else if( message[4] == '>' )
	{
		tmp = (char*)malloc(sizeof(char)*3);
		tmp[0] = message[1]; tmp[1] = message[2]; tmp[2] = message[3];
		pri = atoi(tmp);
		free(tmp);
		d = 5;
	}
	else
	{
		free(m);
		return NULL;
	}
	m->facility = (pri/8);
	m->severity = (pri % 8);
	
	tmp = (char*)malloc(sizeof(char)*RFC3164_DATE_LEN + sizeof(char));
	for(int c = 0; c < RFC3164_DATE_LEN; c++ )
	{
		tmp[c] = message[d+c];
	}
	tmp[RFC3164_DATE_LEN] = '\0';
	d += RFC3164_DATE_LEN + 1;
	m->date = new dstring(tmp);
	free(tmp);
	
	for( int c = d; c < (RFC3164_PACKET_LENGTH - d); c++ )
	{
		if( message[c] == ' ' )
		{
			tmp = (char*)malloc(sizeof(char) * (c - d) + sizeof(char));
			for( int q = d; q < c; q++ )
			{
				tmp[q-d] = message[q];
			}
			tmp[c-d] = '\0';
			m->host = new dstring(tmp);
			free(tmp);
			d = (c+1);
			break;
		}
	}
	if( m->host == NULL )
	{
		free(m);
		return NULL;
	}
	
	for( int c = d; c < (RFC3164_PACKET_LENGTH - d); c++ )
	{
		if( message[c] == '\0' )
		{
			tmp = (char*)malloc(sizeof(char) * (c-d) + sizeof(char));
			for( int q = d; q < c; q++ )
			{
				tmp[q-d] = message[q];
			}
			tmp[c-d] = '\0';
			m->message = new dstring(tmp);
			free(tmp);
			break;
		}
	}
	if( m->message == NULL )
	{
		free(m);
		return NULL;
	}
	#ifdef DEBUG
	cerr << "Returning a parsed message:\nfacility: " << m->facility << "\nseverity: " << m->severity << "\ndate: " << m->date->ascii() << "\nhost: " << m->host->ascii() << "\nmessage: " << m->message->ascii() << endl;
	#endif
	return m;
}

void *rfc3164_launch_thread(void *arg)
{
	struct rfc3164_args *a;
	a = (struct rfc3164_args*)arg;
	rfc3164 *obj;
	if( a->port != 0 )
		obj = new rfc3164(a->port, a->c);
	else
		obj = new rfc3164(a->c);
	free(arg);
	arg = NULL;
	a = NULL;
	obj->listen();
	return NULL;
}

struct rfc3164_args *rfc3164_build_args( cache *c, int port )
{
	struct rfc3164_args *n;
	n = (struct rfc3164_args*)malloc(sizeof(struct rfc3164_args));
	n->c = c;
	n->port = port;
	return n;
}
}

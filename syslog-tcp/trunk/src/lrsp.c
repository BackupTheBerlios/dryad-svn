/***************************************************************************
 *   Copyright (C) 2004 by Peter Snoblin                                   *
 *   pas@truman.edu                                                        *
 *                                                                         *
 *   Permission is hereby granted, free of charge, to any person obtaining *
 *   a copy of this software and associated documentation files (the       *
 *   "Software"), to deal in the Software without restriction, including   *
 *   without limitation the rights to use, copy, modify, merge, publish,   *
 *   distribute, sublicense, and/or sell copies of the Software, and to    *
 *   permit persons to whom the Software is furnished to do so, subject to *
 *   the following conditions:                                             *
 *                                                                         *
 *   The above copyright notice and this permission notice shall be        *
 *   included in all copies or substantial portions of the Software.       *
 *                                                                         *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       *
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    *
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR     *
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR *
 *   OTHER DEALINGS IN THE SOFTWARE.                                       *
 ***************************************************************************/
#include "lrsp.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifdef HAVE_PTHREADS
#include <pthread.h>

pthread_t *send_thread;
#endif

int lrsp_start_convo(char m);

char *s;
int p;
char m;
int inited;
int sock;

int lrsp_init(char *server, int port, char mode)
{
	struct sockaddr_in me, him;

	#ifdef HAVE_PTHREADS
	send_thread = malloc(sizeof(pthread_t));
	#endif
	s = malloc(sizeof(char)*strlen(server));
	strcpy(s, server);
	if( port == 0 )
		p = LRSP_PORT;
	else
		p = port;
	if( mode == LRSP_PERSISTANT || mode == LRSP_SINGLE )
		m = mode;
	else
		m = LRSP_SINGLE;
	if( -1 == (sock = socket(AF_INET, SOCK_STREAM, 0)) )
	{
		#ifdef HAVE_PTHREADS
		free(send_thread);
		#endif
		free(s);
		return -1;
	}
	me.sin_family = AF_INET;
	me.sin_port = htons(0);
	me.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(me.sin_zero), '\0', 8);
	if( -1 == bind(sock, (struct sockaddr*)&me, sizeof(struct sockaddr)) )
	{
		#ifdef HAVE_PTHREADS
		free(send_thread);
		#endif
		free(s);
		return -2;
	}
	if( m == LRSP_PERSISTANT )
	{
		him.sin_family = AF_INET;
		him.sin_port = p;
		him.sin_addr.s_addr = inet_addr(s);
		memset(&(me.sin_zero), '\0', 8);
		if( -1 == connect(sock, (struct sockaddr*)&him, sizeof(struct sockaddr)) )
		{
			#ifdef HAVE_PTHREADS
			free(send_thread);
			#endif
			free(s);
			return -3;
		}
		if( ! lrsp_start_convo(LRSP_PERSISTANT) )
		{
			#ifdef HAVE_PTHREADS
			free(send_thread);
			#endif
			free(s);
			return -4;
		}
	}
	inited = 1;
	return 1;
}

int lrsp_free()
{
	if( 0 == inited )
		return 0;
	#ifdef HAVE_PTHREADS
	free(send_thread);
	#endif
	free(s);
	inited = 0;
	return 1;
}

int lrsp_send_message(char *msg)
{

}

int lrsp_start_convo(char m)
{
	char *msg;
	int t;
	
	msg = malloc(sizeof(char)*1);
	msg[0] = m;
	if( -1 == send(sock, msg, 1*sizeof(char), 0) )
	{
		return -1;
	}
	free(msg);
	msg = malloc(sizeof(char)*3);
	t = recv(sock, msg, 2, 0);
	if( t == 0 || t == -1 )
	{
		free(msg);
		return -2;
	}
	msg[2] = '\0'; /* so we can strcmp */
	if( strcmp(msg, "OK") )
	{
		free(msg);
		return -3;
	}
}

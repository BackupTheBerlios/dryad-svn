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
#include "../config.h"
#include "lrsp.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#ifdef HAVE_PTHREADS
#include <pthread.h>

pthread_t *lrsp_send_thread;
#endif

int lrsp_start_convo(char m);
int lrsp_go(char *msg);

char *lrsp_s;
int lrsp_p;
char lrsp_m;
int lrsp_inited;
int lrsp_sock;
lrsp_callback lrsp_cb;

int lrsp_init(char *server, int port, char mode)
{
	struct sockaddr_in me;

	#ifdef HAVE_PTHREADS
	lrsp_send_thread = malloc(sizeof(pthread_t));
	#endif
	lrsp_s = malloc(sizeof(char)*strlen(server));
	strcpy(lrsp_s, server);
	if( port == 0 )
		lrsp_p = LRSP_PORT;
	else
		lrsp_p = port;
	if( mode == LRSP_PERSISTANT || mode == LRSP_SINGLE )
		lrsp_m = mode;
	else
		lrsp_m = LRSP_SINGLE;
	if( -1 == (lrsp_sock = socket(AF_INET, SOCK_STREAM, 0)) )
	{
		return -1;
	}
	me.sin_family = AF_INET;
	me.sin_port = htons(0);
	me.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(me.sin_zero), '\0', 8);
	if( -1 == bind(lrsp_sock, (struct sockaddr*)&me, sizeof(struct sockaddr)) )
	{
		return -2;
	}
	if( lrsp_m == LRSP_PERSISTANT )
	{
		if( ! lrsp_start_convo(LRSP_PERSISTANT) )
		{
			return -3;
		}
	}
	lrsp_inited = 1;
	return 1;
}

int lrsp_free()
{
	if( 0 == lrsp_inited )
		return 0;
	close(lrsp_sock);
	#ifdef HAVE_PTHREADS
	free(lrsp_send_thread);
	#endif
	free(lrsp_s);
	lrsp_inited = 0;
	return 1;
}

int lrsp_send_message(char *msg)
{
	#ifdef HAVE_PTHREADS
	return pthread_create(lrsp_send_thread, NULL, (void*)lrsp_go, msg);
	#endif
	return lrsp_go(msg);
}

int lrsp_go(char *msg)
{
	int sent, c, err;
	char *try;
	
	sent = 0;
	try = msg;

	if( lrsp_m == LRSP_SINGLE )
	{
		if(  0 > (err = lrsp_start_convo(LRSP_SINGLE)) )
		{
			#ifdef HAVE_PTHREADS
			if( lrsp_cb != NULL )
				(*lrsp_cb)(err, msg);
			return 0;
			#else
			return err;
			#endif
		}
	}
		
	while( sent < (strlen(try) + sizeof(char)) )
	{
		if( -1 == (sent = send(lrsp_sock, try, strlen(try) + sizeof(char), 0)) )
		{
			#ifdef HAVE_PTHREADS
			if( lrsp_cb != NULL )
				(*lrsp_cb)(err, msg);
			return 0;
			#else
			return -5;
			#endif
		}
		for( c = 0; c <= (sent/sizeof(char)); c++ )
			try = try + sizeof(char);
	}
	return 1;
}

int lrsp_start_convo(char m)
{
	char *msg;
	int t;
	struct sockaddr_in him;

	him.sin_family = AF_INET;
	him.sin_port = lrsp_p;
	him.sin_addr.s_addr = inet_addr(lrsp_s);
	memset(&(him.sin_zero), '\0', 8);
	if( -1 == connect(lrsp_sock, (struct sockaddr*)&him, sizeof(struct sockaddr)) )
	{
		return -6;
	}
	
	msg = malloc(sizeof(char)*1);
	msg[0] = lrsp_m;
	if( -1 == send(lrsp_sock, msg, 1*sizeof(char), 0) )
	{
		return -7;
	}
	free(msg);
	msg = malloc(sizeof(char)*3);
	t = recv(lrsp_sock, msg, 2, 0);
	if( t == 0 || t == -1 )
	{
		free(msg);
		return -8;
	}
	msg[2] = '\0'; /* so we can strcmp */
	if( strcmp(msg, "OK") )
	{
		free(msg);
		return -9;
	}
	return 1;
}

char *lrsp_error_message(int err)
{
	switch(err)
	{
		case -1:
			return "Failed to create a socket in lrsp_create.\n";
			break;
		case -2:
			return "Failed to bind a socket in lrsp_create.\n";
			break;
		case -3:
			return "Failed to start the persistant connection in lrsp_create.\n";
			break;
		case -4:
			return "Failed to start the single connection in lrsp_send_message.\n";
			break;
		case -5:
			return "Failed to send the message in lrsp_send_message.\n";
			break;
		case -6:
			return "Failed to connect the socket in lrsp_start_convo.\n";
			break;
		case -7:
			return "Failed to send mode character in lrsp_start_convo.\n";
			break;
		case -8:
			return "Failed to get OK from server in lrsp_start_convo.\n";
			break;
		case -9:
			return "Failed to get OK from server in lrsp_start_convo.\n";
			break;
	}
}

void lrsp_register_callback(lrsp_callback c)
{
	lrsp_cb = c;
}

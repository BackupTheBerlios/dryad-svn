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

pthread_t *lrsp_client_send_thread;
#endif

int lrsp_client_start_convo(char m);
int lrsp_client_go(char *msg);

char *lrsp_client_s;
int lrsp_client_p;
char lrsp_client_m;
int lrsp_client_inited;
int lrsp_client_sock;
lrsp_client_callback lrsp_client_cb;

int lrsp_client_init(char *server, int port, char mode)
{
	struct sockaddr_in me;

	#ifdef HAVE_PTHREADS
	lrsp_client_send_thread = malloc(sizeof(pthread_t));
	#endif
	lrsp_client_s = malloc(sizeof(char)*(strlen(server)+1));
	strcpy(lrsp_client_s, server);
	if( port == 0 )
		lrsp_client_p = LRSP_PORT;
	else
		lrsp_client_p = port;
	if( mode == LRSP_PERSISTANT || mode == LRSP_SINGLE )
		lrsp_client_m = mode;
	else
		lrsp_client_m = LRSP_SINGLE;
	if( -1 == (lrsp_client_sock = socket(AF_INET, SOCK_STREAM, 0)) )
	{
		return -1;
	}
	me.sin_family = AF_INET;
	me.sin_port = htons(0);
	me.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(me.sin_zero), '\0', 8);
	if( -1 == bind(lrsp_client_sock, (struct sockaddr*)&me, sizeof(struct sockaddr)) )
	{
		return -2;
	}
	if( lrsp_client_m == LRSP_PERSISTANT )
	{
		if( ! lrsp_client_start_convo(LRSP_PERSISTANT) )
		{
			return -3;
		}
	}
	lrsp_client_inited = 1;
	return 1;
}

int lrsp_client_free()
{
	if( 0 == lrsp_client_inited )
		return 0;
	#ifdef HAVE_PTHREADS
	free(lrsp_client_send_thread);
	#endif
	free(lrsp_client_s);
	lrsp_client_inited = 0;
	return 1;
}

int lrsp_client_send_message(char *msg)
{
	#ifdef HAVE_PTHREADS
	pthread_create(lrsp_client_send_thread, NULL, (void*)lrsp_client_go, msg);
	return pthread_join(*lrsp_client_send_thread, NULL);
	#endif
	return lrsp_client_go(msg);
}

int lrsp_client_go(char *msg)
{
	int sent, c, err, t;
	char *try, *code;
	
	
	sent = 0;
	try = msg;

	if( lrsp_client_m == LRSP_SINGLE )
	{
		if(  0 > (err = lrsp_client_start_convo(LRSP_SINGLE)) )
		{
			#ifdef HAVE_PTHREADS
			if( lrsp_client_cb != NULL )
				(*lrsp_client_cb)(err, msg);
			return 0;
			#else
			return err;
			#endif
		}
	}
		
	while( sent < (strlen(try) + sizeof(char)) )
	{
		if( -1 == (sent = send(lrsp_client_sock, try, strlen(try) + sizeof(char), 0)) )
		{
			#ifdef HAVE_PTHREADS
			if( lrsp_client_cb != NULL )
				(*lrsp_client_cb)(-5, msg);
			return 0;
			#else
			return -5;
			#endif
		}
		if( 0 == sent )
		{
			#ifdef HAVE_PTHREADS
			if( lrsp_client_cb != NULL )
				(*lrsp_client_cb)(-5, msg);
			return 0;
			#else
			return -5;
			#endif
		}
		for( c = 0; c <= (sent/sizeof(char)); c++ )
			try = try + sizeof(char);
	}
	/* now we check our return code */
	code = malloc(sizeof(char)*3);
	t = recv(lrsp_client_sock, code, 2, 0);
	if( t == 0 || t == -1 )
	{
		free(code);
		return -8;
	}
	code[2] = '\0'; /* so we can strcmp */
	if( strcmp(code, "OK") )
	{
		free(code);
		close(lrsp_client_sock);
		#ifdef HAVE_PTHREADS
		if( lrsp_client_cb != NULL )
			(*lrsp_client_cb)(err, msg);
		return 0;
		#else
		return -10;
		#endif
	}
	close(lrsp_client_sock);
	return 1;
}

int lrsp_client_start_convo(char m)
{
	char *msg;
	int t;
	struct sockaddr_in him;
	

	him.sin_family = AF_INET;
	him.sin_port = htons(lrsp_client_p);
	him.sin_addr.s_addr = inet_addr(lrsp_client_s);
	memset(&(him.sin_zero), '\0', 8);
	if( -1 == connect(lrsp_client_sock, (struct sockaddr*)&him, sizeof(struct sockaddr)) )
	{
		return -6;
	}
	msg = malloc(sizeof(char)*1);
	msg[0] = lrsp_client_m;
	if( -1 == send(lrsp_client_sock, msg, 1*sizeof(char), 0) )
	{
		return -7;
	}
	free(msg);
	msg = malloc(sizeof(char)*3);
	t = recv(lrsp_client_sock, msg, 2, 0);
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

char *lrsp_client_error_message(int err)
{
	switch(err)
	{
		case -1:
			return "Failed to create a socket in lrsp_client_init.\n";
			break;
		case -2:
			return "Failed to bind a socket in lrsp_client_init.\n";
			break;
		case -3:
			return "Failed to start the persistant connection in lrsp_client_init.\n";
			break;
		case -4:
			return "Failed to start the single connection in lrsp_client_send_message.\n";
			break;
		case -5:
			return "Failed to send the message in lrsp_client_send_message.\n";
			break;
		case -6:
			return "Failed to connect the socket in lrsp_client_start_convo.\n";
			break;
		case -7:
			return "Failed to send mode character in lrsp_client_start_convo.\n";
			break;
		case -8:
			return "Failed to get OK from server in lrsp_client_start_convo.\n";
			break;
		case -9:
			return "Failed to get OK from server in lrsp_client_start_convo.\n";
			break;
		case -10:
			return "Failed to get OK from server after sending the message.\n";
			break;
	}
	return NULL;
}

void lrsp_client_register_callback(lrsp_client_callback c)
{
	lrsp_client_cb = c;
}

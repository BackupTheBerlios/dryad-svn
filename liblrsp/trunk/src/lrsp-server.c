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

#define LRSP_OK 1
#define LRSP_NO 2
#define LRSP_WT 3

#ifdef HAVE_PTHREADS
#include <pthread.h>

pthread_t *lrsp_server_thread;
pthread_t *lrsp_server_callback_thread;
pthread_t *lrsp_server_persistent_thread;
pthread_mutex_t *lrsp_server_stop;
int lrsp_server_started;
#endif

int lrsp_server_sock;
int lrsp_server_port;
lrsp_server_callback lrsp_server_cb;
lrsp_server_error_callback lrsp_server_err_cb;
int lrsp_server_inited;

int lrsp_server_go();
char lrsp_server_get_mode(int asock);
void lrsp_server_send_status(int asock, int code);
char *lrsp_server_get_message(int asock, char m);
void lrsp_server_read_messages(int *a);
int lrsp_server_ensure_init();

int lrsp_server_init(int port)
{
	struct sockaddr_in me;
	
	if( lrsp_server_inited == 42 )
		return 0;
	
	#ifdef HAVE_PTHREADS
	lrsp_server_thread = malloc(sizeof(pthread_t));
	lrsp_server_callback_thread = malloc(sizeof(pthread_t));
	lrsp_server_stop = malloc(sizeof(pthread_mutex_t));
	lrsp_server_persistent_thread = malloc(sizeof(pthread_t));
	pthread_mutex_init(lrsp_server_stop, NULL);
	#endif
	if( -1 == (lrsp_server_sock = socket(AF_INET, SOCK_STREAM, 0)) )
	{
		#ifdef HAVE_PTHREADS
		free(lrsp_server_thread);
		free(lrsp_server_callback_thread);
		free(lrsp_server_persistent_thread);
		pthread_mutex_destroy(lrsp_server_stop);
		free(lrsp_server_stop);
		#endif
		return -1;
	}
	if( port == 0 )
		lrsp_server_port = LRSP_PORT;
	else
		lrsp_server_port = port;
	me.sin_family = AF_INET;
	me.sin_port = htons(lrsp_server_port);
	me.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(me.sin_zero), '\0', 8);
	if( -1 == bind(lrsp_server_sock, (struct sockaddr*)&me, sizeof(struct sockaddr)) )
	{
		#ifdef HAVE_PTHREADS
		free(lrsp_server_thread);
		free(lrsp_server_callback_thread);
		free(lrsp_server_persistent_thread);
		pthread_mutex_destroy(lrsp_server_stop);
		free(lrsp_server_stop);
		#endif
		return -2;
	}
	lrsp_server_cb = NULL;
	lrsp_server_err_cb = NULL;
	lrsp_server_inited = 42;
	return 1;
}

int lrsp_server_free()
{
	if( ! lrsp_server_ensure_init() )
		return 0;
	#ifdef HAVE_PTHREADS
	if( lrsp_server_started == 42 )
	{
		pthread_mutex_lock(lrsp_server_stop);
		pthread_join(*lrsp_server_thread, NULL);
		pthread_mutex_unlock(lrsp_server_stop);
	}
	pthread_mutex_destroy(lrsp_server_stop);
	free(lrsp_server_stop);
	free(lrsp_server_thread);
	free(lrsp_server_callback_thread);
	free(lrsp_server_persistent_thread);
	#endif
	close(lrsp_server_sock);
	return 1;
}

void lrsp_server_register_callback(lrsp_server_callback c, lrsp_server_error_callback e)
{
	lrsp_server_cb = c;
	lrsp_server_err_cb = e;
}

int lrsp_server_listen()
{
	if( ! lrsp_server_ensure_init() )
		return -4;
	#ifdef HAVE_PTHREADS
	return pthread_create(lrsp_server_thread, NULL, (void*)lrsp_server_go, NULL);
	#else
	return lrsp_server_go();
	#endif
}

int lrsp_server_go()
{
	int asock;
	struct sockaddr_in *him;
	int *s, *b;
	char t;
	
	if( -1 == listen(lrsp_server_sock, 20) )
	{
		#ifdef HAVE_PTHREADS
		if( lrsp_server_err_cb != NULL )
			(*lrsp_server_err_cb)(-3);
		return -3;
		#else
		return -3;
		#endif
	}
	him = malloc(sizeof(struct sockaddr_in));
	s = malloc(sizeof(int));
	
	while( -1 != (asock = accept(lrsp_server_sock, (struct sockaddr*)him, s)) )
	{
		free(him);
		free(s);
		
		#ifdef HAVE_PTHREADS
		if( 0 != pthread_mutex_trylock(lrsp_server_stop) )
			pthread_exit(NULL);
		else
			pthread_mutex_unlock(lrsp_server_stop);
		#endif
		
		him = malloc(sizeof(struct sockaddr_in));
		s = malloc(sizeof(int));
		
		t = lrsp_server_get_mode(asock);
		if( t != LRSP_PERSISTANT && t != LRSP_SINGLE )
		{
			lrsp_server_send_status(asock, LRSP_WT);
			close(asock);
			continue;
		}
		lrsp_server_send_status(asock, LRSP_OK);
		if( t == LRSP_SINGLE )
		{
			if( lrsp_server_cb != NULL )
			#ifdef HAVE_PTHREADS
				pthread_create(lrsp_server_callback_thread, NULL, (void*)(*lrsp_server_cb), (void*)lrsp_server_get_message(asock, t));
			#else
				(*lrsp_server_cb)(lrsp_server_get_message(asock, t));
			#endif
			else
			{
				lrsp_server_send_status(asock, LRSP_NO);
				close(asock);
				continue;
			}
		}
		else
		{
			if( lrsp_server_cb != NULL )
			{
				b = malloc(sizeof(int)); /* because we *can't* be assured that a pointer is the same size as an int. */
				*b = asock;
				#ifdef HAVE_PTHREADS
				pthread_create(lrsp_server_persistent_thread, NULL, (void*)lrsp_server_read_messages, b );
				#else
				lrsp_server_read_message(b);
				#endif
			}
			else
			{
				lrsp_server_send_status(asock, LRSP_NO);
				close(asock);
				continue;
			}
		}
	}
}

void lrsp_server_read_messages(int *a)
{
	int asock;
	char *ret;
	
	asock = *a;
	free(a);
	
	while(1)
	{
		ret = lrsp_server_get_message(asock, LRSP_PERSISTANT);
		if( ret == NULL )
		{
			close(asock);
			break;
		}
		if( ret[0] == '\0' )
		{
			free(ret);
			//lrsp_server_send_status(asock, LRSP_OK);
			close(asock);
			break;
		}
		#ifdef HAVE_PTHREADS
		pthread_create(lrsp_server_callback_thread, NULL, (void*)(*lrsp_server_cb), ret);
		#else
		lrsp_server_cb(ret);
		#endif
		lrsp_server_send_status(asock, LRSP_OK);
	}
}

char *lrsp_server_get_message(int asock, char m)
{
	char *buf, *b;
	int len, c;
	
	buf = malloc(sizeof(char)*1025); /*so we have room for the \0 if they send it*/
	memset(buf, '\0', 1025);
	if( -1 == (len = recv(asock, buf, sizeof(char)*1025, 0)) )
	{
		free(buf);
		return NULL;
	}
	if( len == 0 )
	{
		return NULL;
	}
	if( len == 1025 )
	{
		buf[(len/sizeof(char))-1] = '\0';
	}
	else if( buf[(len/sizeof(char))-1] != '\0' )
	{
		len = len + sizeof(char);
		buf[(len/sizeof(char))-1] = '\0';
	}
	b = malloc(sizeof(char) * len);
	for( c = 0; c < (len-1); c++ ) /* len - 1 is because if you use just len, we may end up copying an unkown, possibly invalid, memory cell to the last spot in b */
	{
		b[c] = buf[c];
	}
	b[len-1] = '\0';
	free(buf);
	if( m == LRSP_SINGLE )
	{
		lrsp_server_send_status(asock, LRSP_OK);
		close(asock);
	}
	return b;
}

void lrsp_server_send_status(int asock, int code)
{
	char *buf;
	
	buf = malloc(sizeof(char)*2);
	
	if( code == LRSP_OK )
	{
		buf[0] = 'O';
		buf[1] = 'K';
	}
	else if( code == LRSP_NO )
	{
		buf[0] = 'N';
		buf[1] = 'O';
	}
	else if( code == LRSP_WT )
	{
		buf[0] = 'W';
		buf[1] = 'T';
	}
	else
	{
		free(buf);
		return;
	}
	send(asock, buf, sizeof(char)*2, 0);
}

char lrsp_server_get_mode(int asock)
{
	char *buf, ret;
	int r;
	
	buf = malloc(sizeof(char));
	r = recv(asock, buf, sizeof(char), 0);
	if( r == 0 || r == -1 )
	{
		free(buf);
		return 0;
	}
	ret = buf[0];
	free(buf);
	return ret;
}

char *lrsp_server_error_message(int err)
{
	switch(err)
	{
		case -1:
			return "Unable to create a socket in lrsp_server_init.\n";
			break;
		case -2:
			return "Unable to bind the socket in lrsp_server_init.\n";
			break;
		case -3:
			return "Unable to start listening in lrsp_server_go.\n";
			break;
		case -4:
			return "You must call lrsp_server_init first.\n";
			break;
	}
	return NULL;
}

void lrsp_server_join()
{
	if( ! lrsp_server_ensure_init() )
		return;
	#ifdef HAVE_PTHREADS
	pthread_join(*lrsp_server_thread, NULL);
	#endif
}

int lrsp_server_ensure_init()
{
	return (lrsp_server_inited == 42);
}

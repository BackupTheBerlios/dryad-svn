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
#include "lrsp-server.h"
namespace DLRSP
{
lrsp *global;

lrsp::lrsp(cache *s)
{
	lrsp_server_init(0);
	c = s;
}

lrsp::lrsp(int port, cache *s)
{
	int err;
	if( 0 > (err = lrsp_server_init(port)) )
	{
		cerr << lrsp_server_error_message(err);
		exit(1);
	}
	c = s;
}

lrsp::~lrsp()
{
	lrsp_server_free();
}

void lrsp::listen()
{
	lrsp_server_register_callback( DLRSP::msg_cb, DLRSP::err_cb );
	lrsp_server_listen();
	lrsp_server_join();
}

void lrsp::error_callback(int err)
{
	char *t;
	t = itoa(err);
	dryerr(1, 4, "lrsp-server recieved error from liblrsp (", t, "):\n", lrsp_server_error_message(err));
	free(t);
}

void lrsp::message_callback(char *msg)
{
	c->add(this->parse_message(msg));
}


struct syslog_message *lrsp::parse_message(char *message)
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
	
	for( int c = d; c <= RFC3164_PACKET_LENGTH; c++ )
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
	
	for( int c = d; c <= RFC3164_PACKET_LENGTH; c++ )
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

void *lrsp_launch_thread(void *obj)
{
	struct lrsp_args *a;
	a = (struct lrsp_args*)obj;
	lrsp *l;
	l = a->obj;
	if( NULL != l )
		l->listen();
}

void msg_cb(char *msg)
{
	global->message_callback(msg);
}

void err_cb(int err)
{
	global->error_callback(err);
}

struct lrsp_args *lrsp_setup(int port, cache *c)
{
	struct lrsp_args *ret;
	ret = (struct lrsp_args*)malloc(sizeof(struct lrsp_args));
	ret->obj = new lrsp(port, c);
	global = ret->obj;
	return ret;
}

void lrsp_stop()
{
	lrsp_server_free();
}

}


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
#include "plugin.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

/*
	This is a very simple example reporter plugin. When it gets called, if sends a dump of the contents of the syslog messages to a compile time defined email address.
*/
namespace email_plugin
{
using namespace plugin;
extern "C" {

	int send_mail(dstring *msg, dstring *to, dstring *from, dstring *mailhost);
	int get_return_status( int sock, int status );
	
	// The function used to report a single incident must be called dryad_once. Before it returns, it should also free() the passed syslog_message
	int dryad_once(struct syslog_message *m)
	{
		dstring *msg;
		char *tmp;
		msg = new dstring;
		msg->cat("Single log event report from your RTLAD Dryad installation:\nTriggering Group: ");
		msg->cat(m->daemon->ascii());
		msg->cat("\nLog Message: ");
		msg->cat(m->message->ascii());
		msg->cat("\nHost: ");
		msg->cat(m->host->ascii());
		msg->cat("\nDate: ");
		msg->cat(m->date->ascii());
		msg->cat("\nSeverity: ");
		tmp = itoa(m->severity);
		msg->cat(tmp);
		free(tmp);
		msg->cat("\nFacility: ");
		tmp = itoa(m->facility);
		msg->cat(tmp);
		free(tmp);
		msg->cat("\n");
		//send_msg(msg->ascii(), "root@localhost");
		delete msg;
		free(m);
	}
	
	//! Before it returns, we need to free() all messages in m, and delete m itself.
	int dryad_many(drarray<struct syslog_message*> *m)
	{
		for( int c = 0; c < m->length(); c++ )
		{
			free(m->at(c));
		}
		delete m;
	}
	
	int send_mail(dstring *msg, dstring *to, dstring *from, dstring *mailhost)
	{
		int sock, ret;
		struct sockaddr_in *conn_addr;
		struct servent *smtp;
		struct hostent *dest;
		char *myname, *incoming;
		dstring *tmp;
		
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if( -1 == sock )
		{
			cerr << "Unable to create a socket in libemail! Not sending message.\n";
			return false;
		}
		
		smtp = getservbyname("smtp", "tcp");
		dest = gethostbyname(mailhost->ascii());
		conn_addr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
		conn_addr->sin_family = AF_INET;
		if( smtp == NULL )
			conn_addr->sin_port = htons(25);
		else
			conn_addr->sin_port = htons(smtp->s_port);
		conn_addr->sin_addr.s_addr = inet_addr(dest->h_name);
		memset(&(conn_addr->sin_zero), '\0', 8);
		if( -1 == connect( sock, (struct sockaddr*)conn_addr, sizeof(struct sockaddr)) )
		{
			cerr << "Unable to connect to remote host in libemail! Not sending message.\n";
			free(conn_addr);
			return false;
		}
		// these may segfault, need to check!
		free(smtp);
		free(dest);
		
		myname = (char*)malloc(sizeof(char)*256);
		gethostname(myname, 256);
		tmp = new dstring("HELO ");
		tmp->cat(myname);
		tmp->cat("\n");
		free(myname);
		if( -1 == send( sock, (void*)tmp->ascii(), tmp->length(), 0 ) )
		{
			cerr << "Unable to send HELO to the remote host in libemail! Not sending message.\n";
			close(sock);
			delete tmp;
			return false;
		}
		delete tmp;
		if( ! get_return_status(sock, 250) )
		{
			close(sock);
			return false;
		}
		
		tmp = new dstring("MAIL FROM: ");
		tmp->cat(from->ascii());
		tmp->cat("\n");
		if( -1 == send( sock, (void*)tmp->ascii(), tmp->length(), 0) )
		{
			cerr << "Unable to set MAIL FROM in libemail! Not sending message.\n";
			close(sock);
			delete tmp;
			return false;
		}
		delete tmp;
		if( ! get_return_status(sock, 250) )
		{
			close(sock);
			return false;
		}
		
		tmp = new dstring("RCPT TO: ");
		tmp->cat(to->ascii());
		tmp->cat("\n");
		if( -1 == send( sock, (void*)tmp->ascii(), tmp->length(), 0) )
		{
			cerr << "Unable to set RCPT TO in libemail! Not sending message.\n";
			close(sock);
			delete tmp;
			return false;
		}
		delete tmp;
		if( ! get_return_status(sock, 250) )
		{
			close(sock);
			return false;
		}
		
		tmp = new dstring("DATA\n");
		if( -1 == send(sock, (void*)tmp->ascii(), tmp->length(), 0) )
		{
			cerr << "Unable to send DATA in libemail! Not sending message.\n";
			close(sock);
			delete tmp;
			return false;
		}
		if( ! get_return_status(sock, 354) )
		{
			close(sock);
			return false;
		}
		
		if( -1 == send(sock, (void*)msg->ascii(), msg->length(), 0) )
		{
			cerr << "Unable to send message data in libemail! Not sending message.\n";
			close(sock);
			return false;
		}
		
		tmp = new dstring("\r\n.\r\n");
		if( -1 == send(sock, (void*)tmp->ascii(), tmp->length(), 0) )
		{
			cerr << "Unable to send message termination in libemail! Not sending message.\n";
			close(sock);
			return false;
		}
		if( ! get_return_status(sock, 250) )
		{
			close(sock);
			delete tmp;
			return false;
		}
		delete tmp;
		
		tmp = new dstring("QUIT\n");
		if( -1 == send(sock, (void*)tmp->ascii(), tmp->length(), 0) )
		{
			cerr << "Unable to send QUIT in libemail! Message prolly got sent though...\n";
			close(sock);
			delete tmp;
			return false;
		}
		delete tmp;
		if( ! get_return_status(sock, 221) )
		{
			close(sock);
			return false;
		}
		close(sock);
		return true;
		
	}
	
	int get_return_status( int sock, int status )
	{
		char *incoming, *s;
		int ret;
		
		incoming = (char*)malloc(sizeof(char)*3);
		ret = recv(sock, incoming, 3, MSG_PEEK);
		if( !(1 >= ret ) )
		{
			cerr << "Error in sending to remote host in libemail! Not sending message.\n";
			free(incoming);
			return false;
		}
		s = itoa(status);
		if( strcmp(incoming, s ) )
		{
			cerr << "Got an error from the remote server in libemail! Not sending message.\n";
			free(incoming);
			return false;
		}
		free(s);
		free(incoming);
		if( ret > 3 )
		{
			incoming = (char*)malloc(sizeof(char)*(ret - 3));
			recv(sock, incoming, (ret-3), 0);
			free(incoming);
		}
		return true;
	}

}
}

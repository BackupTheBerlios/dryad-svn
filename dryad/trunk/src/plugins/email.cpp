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
#include <libesmtp.h>

/*
	This is a very simple example reporter plugin. When it gets called, if sends a dump of the contents of the syslog messages to a compile time defined email address.
*/
namespace email_plugin
{
using namespace plugin;
extern "C" {
	
	// yeah, I know globals are bad. This module isn't really comendable for it's coding standards. 's called being quick 'n' dirty.
	dstring *to;
	dstring *from;
	dstring *mailhost;

	int send_message(dstring *message, dstring *to, dstring *from, dstring *mailhost);
	
	int dryad_config(conf *c)
	{
		to = c->daemon_get("libemail_module", "to");
		from = c->daemon_get("libemail_module", "from");
		mailhost = c->daemon_get("libemail_module", "mailhost");
		if( to == NULL || from == NULL || mailhost == NULL )
		{
			cerr << "Invalid configuration for libemail_module!\nAborting!\n";
			exit(1);
		}
	}
	
	// The function used to report a single incident must be called dryad_once. Before it returns, it should also free() the passed syslog_message
	int dryad_once(struct syslog_message *m)
	{
		dstring *msg;
		char *tmp;
		int ret;
		
		if( to == NULL || from == NULL || mailhost == NULL )
		{
			cerr << "You MUST call libemail::dryad_config before dryad_once!\nAborting!\n";
			exit(1);
		}
		
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
		ret = true;
		if( ! send_message(msg, to, from, mailhost ) )
		{
			cerr << "Failed to send notification email in libemail::dryad_once!\n";
			ret = false;
		}
		delete msg;
		free(m);
		delete m->daemon;
		delete m->date;
		delete m->host;
		delete m->message;
		return ret;
	}
	
	//! Before it returns, we need to free() all messages in m, and delete m itself.
	int dryad_many(drarray<struct syslog_message*> *m)
	{
		dstring *msg;
		char *tmp;
		int ret;

		if( to == NULL || from == NULL || mailhost == NULL )
		{
			cerr << "You MUST call libemail::dryad_config before dryad_many!\nAborting!\n";
			exit(1);
		}
		
		msg = new dstring;
		msg->cat("Multiple log events report from your RTLAD Dryad installation:\n\n");
		for( int c = 0; c < m->length(); c++ )
		{
			msg->cat("Triggering Group: ");
			msg->cat(m->at(c)->daemon->ascii());
			msg->cat("\nLog Message: ");
			msg->cat(m->at(c)->message->ascii());
			msg->cat("\nHost: ");
			msg->cat(m->at(c)->host->ascii());
			msg->cat("\nDate: ");
			msg->cat(m->at(c)->date->ascii());
			msg->cat("\nSeverity: ");
			tmp = itoa(m->at(c)->severity);
			msg->cat(tmp);
			free(tmp);
			msg->cat("\nFacility: ");
			tmp = itoa(m->at(c)->facility);
			msg->cat(tmp);
			free(tmp);
			msg->cat("\n\n");
			free(m->at(c));
		}
		delete m;
		ret = true;
		if( ! send_message(msg, to, from, mailhost ) )
		{
			cerr << "Failed to send notification email in libemail::dryad_many!\n";
			ret = false;
		}
		return ret;
	}
	
	int send_message(dstring *message, dstring *to, dstring *from, dstring *mailhost)
	{
		/* For the record, I HATE libESMTP. The documentation is total crap, both in that it is incomplete, as well as that what is there is poorly written to the point of being INCOMPREHENSIBLE!!!! </rant>*/
		smtp_session_t s;
		smtp_message_t m;
		
		s = smtp_create_session();
		m = smtp_add_message(s);
		if( ! smtp_set_server(s, mailhost->cstring()) )
		{
			smtp_destroy_session(s);
			return false;
		}
		if( ! smtp_set_reverse_path(m, from->cstring()) )
		{
			smtp_destroy_session(s);
			return false;
		}
		if( ! smtp_add_recipient(m, to->cstring()) )
		{
			smtp_destroy_session(s);
			return false;
		}
		//smtp_set_header(m, "To", to->cstring());
		//smtp_set_header(m, "From", from->cstring());
		//smtp_set_header(m, "Subject", "RTLAD Dryad Report");
		smtp_set_message_str(m, message->cstring());
		if( ! smtp_start_session(s) )
		{
			smtp_destroy_session(s);
			return false;
		}
		smtp_destroy_session(s);
		return true;
	}

}
}

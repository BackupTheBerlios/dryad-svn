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

namespace external_plugin
{
using namespace plugin;
extern "C"
{
	dstring *sanitize_string(dstring *str);
	dstring *command;
	int dryad_config(conf *c)
	{
		command = c->daemon_get("libexternal_module", "command");
		if( NULL == command )
		{
			cerr << "No command specified for libexternal!\nAborting!\n";
			exit(1);
		}
	}
	
	int dryad_once(struct syslog_message *m)
	{
		dstring *cmd, *san_str;
		char *t;
		
		cmd = new dstring(command);
		cmd->cat(" \"");
		san_str = sanitize_string(m->daemon);
		cmd->cat(san_str->ascii());
		delete san_str;
		
		cmd->cat("\" \"");
		san_str = sanitize_string(m->date);
		cmd->cat(san_str->ascii());
		delete san_str;
		
		cmd->cat("\" \"");
		san_str = sanitize_string(m->host);
		cmd->cat(san_str->ascii());
		delete san_str;
		
		cmd->cat("\" \"");
		san_str = sanitize_string(m->message);
		cmd->cat(san_str->ascii());
		delete san_str;
		
		cmd->cat("\" ");
		t = itoa(m->facility);
		cmd->cat(t);
		free(t);
		cmd->cat(" ");
		t = itoa(m->severity);
		cmd->cat(t);
		free(t);
		
		system(cmd->ascii());
		
		delete cmd;
		
		delete m->daemon;
		delete m->date;
		delete m->host;
		delete m->message;
		free(m);
		return true;
	}
	
	int dryad_many(drarray<struct syslog_message*> *m)
	{
		dstring *cmd, *san_str;
		char *t;
		
		cmd = new dstring(command);
		cmd->cat(" ");
		t = itoa(m->length());
		cmd->cat(t);
		free(t);
		
		for( int c = 0; c < m->length(); c++ )
		{
			cmd->cat(" \"");
			san_str = sanitize_string(m->at(c)->daemon);
			cmd->cat(san_str->ascii());
			delete san_str;
			
			cmd->cat("\" \"");
			san_str = sanitize_string(m->at(c)->date);
			cmd->cat(san_str->ascii());
			delete san_str;
			
			cmd->cat("\" \"");
			san_str = sanitize_string(m->at(c)->host);
			cmd->cat(san_str->ascii());
			delete san_str;
			
			cmd->cat("\" \"");
			san_str = sanitize_string(m->at(c)->message);
			cmd->cat(san_str->ascii());
			delete san_str;
			
			cmd->cat("\" ");
			t = itoa(m->at(c)->facility);
			cmd->cat(t);
			free(t);
			cmd->cat(" ");
			t = itoa(m->at(c)->severity);
			cmd->cat(t);
			free(t);
			
			delete m->at(c)->daemon;
			delete m->at(c)->date;
			delete m->at(c)->host;
			delete m->at(c)->message;
			free(m->at(c));
		}
		delete m;
		system(cmd->ascii());
		delete cmd;
		return true;
		
	}
	
	
	dstring *sanitize_string(dstring *str)
	{
		dstring *ret;
		ret = new dstring;
		
		for( int c = 0; c < str->length(); c++ )
		{
			if( str->ascii()[c] == '\"')
			{
				ret->cat("\\\"");
			}
			else if(str->ascii()[c] == '`')
			{
				ret->cat("\'");
			}
			else
			{
				ret->cat(str->ascii()[c]);
			}
		}
		return ret;
	}


}
}

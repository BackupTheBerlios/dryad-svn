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

namespace file_plugin
{
using namespace plugin;
extern "C"
{
	dstring *path;
	dfilestream *writer;
	int dryad_config(conf *c)
	{
		path = c->daemon_get("libfile_module", "path");
		if( path == NULL )
		{
			cerr << "path not set in libfile_module config block!\nAborting!\n";
			exit(1);
		}
		writer = new dfilestream;
		if( ! writer->open(path, "a") )
		{
			cerr << "Unable to open path (" << path->ascii() << ") for writing!\nAborting!\n";
			exit(1);
		}
	}
	
	int dryad_once(struct syslog_message *m)
	{
		dstring *tmp;
		tmp = new dstring(m->date);
		tmp->cat(" ");
		tmp->cat(m->message->ascii());
		writer->writeline(tmp);
		delete tmp;
		delete m->daemon;
		delete m->date;
		delete m->host;
		delete m->message;
		free(m);
		return true;
	}
	
	int dryad_many(drarray<struct syslog_message*> *m)
	{
		dstring *tmp;
		for( int c = 0; c < m->length(); c++ )
		{
			tmp = new dstring(m->at(c)->date);
			tmp->cat(" ");
			tmp->cat(m->at(c)->message->ascii());
			writer->writeline(tmp);
			delete tmp;
			delete m->at(c)->daemon;
			delete m->at(c)->date;
			delete m->at(c)->host;
			delete m->at(c)->message;
			free(m->at(c));
		}
		return true;
	}

}

}

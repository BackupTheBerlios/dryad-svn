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
#include "conf.h"

namespace DConf
{
conf::conf(dstring *cf)
{
	dfilestream *fs;
	if( cf == NULL )
	{
		dryerr(1, 1, "NULL config file passed to conf object!\nAborting!\n");
		exit(1);
	}
	fs = new dfilestream;

	if( ! fs->open(cf, "r") )
	{
		dryerr(1, 1, "Failed open to config file!\nAborting!\n");
		exit(1);
	}
	
	cfile = new dstring(cf);
	
	reload = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(reload, NULL);
	
	dbs = new drarray<dstring*>;
	vars = new drarray<variable*>;
	daemons = new drarray<daemon_section*>;
	
	this->readconfig(fs);
}

conf::~conf()
{
	pthread_mutex_destroy(reload);
	free(reload);
	delete dbs;
	delete vars;
	delete daemons;
	delete cfile;
}

void conf::readconfig(dfilestream *fs)
{
	dstring *tmp;
	variable *v;
	struct daemon_section *d;
	
	d = NULL;
	
	pthread_mutex_lock(reload);
	
	while( tmp = fs->readline() )
	{
		if( !strncmp( tmp->ascii(), "#", 1 ) )
		{
			delete tmp;
			continue;
		}
		else if( !strncmp( tmp->ascii(), "dbfile ", 7 ) && tmp->length() > 7 )
		{
			dbs->pushback(tmp->remove("dbfile "));
			delete tmp; //because remove() returns a whole new dstring
		}
		else if( !strncmp(tmp->ascii(), "BEGIN ", 6 ) && tmp->length() > 6 )
		{
			d = (struct daemon_section*)malloc(sizeof(struct daemon_section));
			d->contents = new drarray<variable*>;
			d->name = tmp->remove("BEGIN ");
			delete tmp;
		}
		else if( !strncmp(tmp->ascii(), "END", 3 ) )
		{
			if( d != NULL )
			{
				daemons->pushback(d);
				d = NULL;
			}
			delete tmp;
		}
		else if( d != NULL )
		{
			v = (struct variable*)malloc(sizeof(struct variable));
			v->name = tmp->prior(' ');
			v->value = tmp->following(' ');
			if( v->name == NULL || v->value == NULL )
			{
				delete v->name;
				delete v->value;
				free(v);
				continue;
			}
			d->contents->pushback(v);
			v = NULL;
			delete tmp;
		}
		else
		{
			v = (struct variable*)malloc(sizeof(struct variable));
			v->name = tmp->prior(' ');
			v->value = tmp->following(' ');
			if( v->name == NULL || v->value == NULL )
			{
				delete v->name;
				delete v->value;
				free(v);
				continue;
			}
			vars->pushback(v);
			v = NULL;
			delete tmp;
		}
	}
	if( d != NULL )
	{
		dryerr(1, 3, "Malformed config file! Never saw an END for ", d->name->ascii(), "!\nAborting!\n");
		exit(1);
	}
	pthread_mutex_unlock(reload);
}

int conf::num_dbs() const
{
	return dbs->length();
}

dstring *conf::db(int k) const
{
	return dbs->at(k);
}

dstring *conf::get(const char *name)
{
	for( int c = 0; c < vars->length(); c++ )
	{
		if( !strcmp(vars->at(c)->name->ascii(), name) )
			return vars->at(c)->value;
	}
	return NULL;
}

dstring *conf::daemon_get(const char *daemon, const char *name)
{
	for( int c = 0; c < daemons->length(); c++ )
	{
		if( !strcmp(daemons->at(c)->name->ascii(), daemon) )
		{
			for( int d = 0; d < daemons->at(c)->contents->length(); d++ )
			{
				if( !strcmp(daemons->at(c)->contents->at(d)->name->ascii(), name) )
				{
					return daemons->at(c)->contents->at(d)->value;
				}
			}
			return NULL;
		}
	}
	return NULL;
}

int conf::num_daemons() const
{
	int ret;
	if( daemons != NULL )
		ret = daemons->length();
	else
		ret = 0;
	return ret;
}

dstring *conf::daemon_name(int k)
{
	if( k < 0 || k > daemons->length() )
		return NULL;
	return daemons->at(k)->name;
}

}

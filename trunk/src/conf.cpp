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

conf::conf(dstring *cf)
{
	dfilestream *fs;
	if( cf == NULL )
	{
		cerr << "NULL config file passed to conf object!\nAborting!\n";
		exit(1);
	}
	fs = new dfilestream;
	if( ! fs->open(cf, "r") )
	{
		cerr << "Failed to config file!\nAborting!\n";
		exit(1);
	}
	
	reload = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(reload, NULL);
	
	dbs = new drarray<dstring*>;
	vars = new drarray<variable*>;
	
	this->readconfig(fs);
}

conf::~conf()
{
	pthread_mutex_destroy(reload);
}

void conf::readconfig(dfilestream *fs)
{
	dstring *tmp;
	variable *v;
	
	pthread_mutex_lock(reload);
	
	while( tmp = fs->readline() )
	{
		if( !strncmp( tmp->ascii(), "#", 1 ) )
		{
			continue;
		}
		if( !strncmp( tmp->ascii(), "dbfile ", 7 ) && tmp->length() > 7 )
		{
			dbs->pushback(tmp->remove("dbfile "));
		}
		else
		{
			v = (variable*)malloc(sizeof(variable));
			v->name = tmp->prior(' ');
			v->value = tmp->following(' ');
			if( v->name == NULL || v->value == NULL )
			{
				free(v);
				continue;
			}
			vars->pushback(v);
			v = NULL;
		}
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

dstring *conf::get(char *name)
{
	for( int c = 0; c < vars->length(); c++ )
	{
		if( !strcmp(vars->at(c)->name->ascii(), name) )
			return vars->at(c)->value;
	}
	return NULL;
}

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

conf::conf(dstring *c)
{
	num_daemons = 0;
	dfilestream *f;
	f = new dfilestream;
	if( f->open(c) )
	{
		if( loadconfig(f) )
			file = c;
		else
			file = NULL;
	}
}

int conf::reload()
{
	num_daemons = 0;
	free(daemons);
	dfilestream *f;
	f = new dfilestream;
	if(f->open(file))
	{
		return loadconfig(f);
	}
	else
	{
		return false;
	}
}

int conf::loadconfig(dfilestream *f)
{
	dstring *tmp, *t;
	dstring *cur_mod;
	struct daemon *d;
	struct daemon **n; 
	cur_mod = NULL;
	while( tmp = f->readline() )
	{
		if( !strncmp( tmp->ascii(), "BEGIN ", 6 ) )
		{
			cur_mod = tmp->remove( "BEGIN " );
			d = (struct daemon*)malloc(sizeof(struct daemon));
			d->name = cur_mod;
			num_daemons++;
			n = (struct daemon**)malloc(sizeof(struct daemon*) * num_daemons);
			for( int c = 0; c < (num_daemons-1); c++ )
			{
				n[c] = daemons[c];
			}
			n[num_daemons-1] = d;
			free(daemons);
			daemons = n;
			continue;
		}
		if( !strncmp( tmp->ascii(), "END", 3) )
		{
			cur_mod = NULL;
			continue;
		}
		if( ! strncmp( tmp->ascii(), "warn_level ", 11 ) )
		{
			if( cur_mod == NULL )
			{
				warn_level = atoi(tmp->remove("warn_level ")->ascii());
				continue;
			}
			d->warn_level = atoi(tmp->remove("warn_level ")->ascii());
			continue;
		}
		if( ! strncmp( tmp->ascii(), "clear_on_warn ", 14 ) )
		{
			if( cur_mod == NULL )
			{
				clear_on_warn = atoi(tmp->remove("clear_on_warn ")->ascii());
				continue;
			}
			d->clear_on_warn = atoi(tmp->remove("clear_on_warn ")->ascii());
			continue;
		}
		if( ! strncmp( tmp->ascii(), "error_level ", 12 ) )
		{
			if( cur_mod == NULL )
			{
				error_level = atoi(tmp->remove("error_level ")->ascii());
				continue;
			}
			d->error_level = atoi(tmp->remove("error_level ")->ascii());
			continue;
		}
		if( ! strncmp( tmp->ascii(), "clear_on_error ", 15 ) )
		{
			if( cur_mod == NULL )
			{
				clear_on_error = atoi(tmp->remove("clear_on_error ")->ascii());
				continue;
			}
			d->clear_on_error = atoi(tmp->remove("clear_on_error ")->ascii());
			continue;
		}
	} //end while
	this->checkconfig();
	return true;
}

int conf::checkconfig()
{
	if( warn_level < 1 )
	{
		cerr << "Invalid warn_level: " << warn_level << ".\nAborting.\n";
		exit(1);
	}
	if( clear_on_warn != 1 || clear_on_warn != 0 )
	{
		cerr << "Invalid clear_on_warn: " << clear_on_warn << ".\nAborting.\n";
		exit(1);
	}
	if( error_level < warn_level )
	{
		cerr << "Invalid error_level: " << error_level << ".\nAborting.\n";
		exit(1);
	}
	if( clear_on_error != 1 || clear_on_error != 0 )
	{
		cerr << "Invalid clear_on_error: " << clear_on_error << ".\nAborting.\n";
		exit(1);
	}
	for( int c = 0; c < num_daemons; c++ )
	{
		if( daemons[c]->warn_level < 1 )
		{
			cerr << "In daemon \"" << daemons[c]->name->ascii() << "\"\nInvalid warn_level: " << daemons[c]->warn_level  <<".\nAborting.\n";
			exit(1);
		}
		if( daemons[c]->clear_on_warn != 1 || daemons[c]->clear_on_warn != 0 )
		{
			cerr << "In daemon \"" << daemons[c]->name->ascii() << "\"\nInvalid clear_on_warn: " << daemons[c]->clear_on_warn << ".\nAborting.\n";
			exit(1);
		}
		if( daemons[c]->error_level < daemons[c]->warn_level )
		{
			cerr << "In daemon \"" << daemons[c]->name->ascii() << "\"\nInvalid error_level: " << daemons[c]->error_level << ".\nAborting.\n";
			exit(1);
		}
		if( daemons[c]->clear_on_error != 1 || daemons[c]->clear_on_error != 0 )
		{
			cerr << "In daemon \"" << daemons[c]->name->ascii() << "\"\nInvalid clear_on_error: " << daemons[c]->clear_on_error << ".\nAborting.\n";
			exit(1);
		}
	}
}

struct daemon *conf::get_daemon(char *name)
{
	struct daemon *d; //only used if they want default settings.
	if( name == NULL )
	{
		d = (struct daemon*)malloc(sizeof(struct daemon));
		d->name = NULL;
		d->warn_level = warn_level;
		d->clear_on_warn = clear_on_warn;
		d->error_level = error_level;
		d->clear_on_error = clear_on_error;
		return d;
	}
	for( int c = 0; c < num_daemons; c++ )
	{
		if( ! strcmp( name, daemons[c]->name->ascii() ) )
		{
			return daemons[c];
		}
	}
	return NULL;
}

struct daemon *conf::get_daemon(dstring *name)
{
	struct daemon *d; //only used if they want default settings.
	if( name == NULL )
	{
		d = (struct daemon*)malloc(sizeof(struct daemon));
		d->name = NULL;
		d->warn_level = warn_level;
		d->clear_on_warn = clear_on_warn;
		d->error_level = error_level;
		d->clear_on_error = clear_on_error;
		return d;
	}
	for( int c = 0; c < num_daemons; c++ )
	{
		if( ! strcmp( name->ascii(), daemons[c]->name->ascii() ) )
		{
			return daemons[c];
		}
	}
	return NULL;
}


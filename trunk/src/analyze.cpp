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
#include "analyze.h"

analyze::analyze(conf *c)
{
	dstring *tmp;
	if( c == NULL )
	{
		cerr << "Invalid conf passed to analyze!\nAborting!\n";
		exit(1);
	}
	
	this->c = c;
	
	emergency = this->build_severity_struct(NULL, "emergency");
	alert = this->build_severity_struct(NULL, "alert");
	critical = this->build_severity_struct(NULL, "critical");
	error = this->build_severity_struct(NULL, "error");
	warning = this->build_severity_struct(NULL, "warning");
	notice = this->build_severity_struct(NULL, "notice");
	informational = this->build_severity_struct(NULL, "informational");
	debug = this->build_severity_struct(NULL, "debug");
	
	seen = new darray<struct syslog_message*>;
	db_vec = new drarray<database*>;
}

analyze::~analyze()
{
	free(emergency);
	free(alert);
	free(critical);
	free(error);
	free(warning);
	free(notice);
	free(informational);
	free(debug);
	delete seen;
	delete db_vec;
}

int analyze::load( database *db )
{
	if( db == NULL )
		return false;
	db_vec->pushback(db);
	return true;
}

int analyze::process( struct syslog_message *m )
{
	int rr;
	int ovector[3];
	
	if( m == NULL )
		return -1;
	for(int c = 0; c < db_vec->length(); c++ )
	{
		if( db_vec->at(c)->reset() == -1 )
			continue;
		do
		{
			rr = pcre_exec( db_vec->at(c)->re(), db_vec->at(c)->rs(), m->message->ascii(), m->message->length(), 0, 0, ovector, 3 );
			if( rr != PCRE_ERROR_NOMATCH )
			{
				m->daemon = db_vec->at(c)->daemon();
				this->reg(m);
				return true;
			}
		} while( db_vec->at(c)->next() );
	}
	return false;
}

void analyze::reg( struct syslog_message *m )
{
	struct severity *useme;
	switch(m->severity)
	{
		case(0):
			useme = emergency;
			break;
		case(1):
			useme = alert;
			break;
		case(2):
			useme = critical;
			break;
		case(3):
			useme = error;
			break;
		case(4):
			useme = warning;
			break;
		case(5):
			useme = notice;
			break;
		case(6):
			useme = informational;
			break;
		case(7):
			useme = debug;
			break;
	}
	if( ! useme->track )
		return;
	seen->insert(m);
	if( useme->report )
	{
		this->report_once(m);
	}
	useme->level++;
	if( useme->level >= useme->max && useme->max > 0 )
	{
		useme->level = 0;
		this->report(m, useme->all);
	}
}

void analyze::report_once( struct syslog_message *m )
{
	//do stuff to report the single message, involving email, python, perl, whatevar.
}

void analyze::report( struct syslog_message *m, int all )
{
	if( all )
	{
		//get all messages of this daemon, at any severity
	}
	else
	{
		//get all messages of this daemon, at this severity
	}
}

struct severity *analyze::build_severity_struct(char *daemon, char *level)
{
	struct severity *ret;
	dstring *tmp, *build;
	
	ret = (struct severity*)malloc(sizeof(struct severity));
	
	if( daemon == NULL )
	{
		build = new dstring(level);
		build->cat("_track");
		tmp = c->get(build->ascii());
		delete build;
		if( ! atoi(tmp->ascii()) )
		{
			ret->track = 0;
			ret->all = 0;
			ret->level = 0;
			ret->max = 0;
			ret->report = 0;
			return ret;
		}
		delete tmp;
		ret->track = 1;
		
		build = new dstring(level);
		build->cat("_all");
		tmp = c->get(build->ascii());
		ret->all = atoi(tmp->ascii());
		delete tmp;
		delete build;
		
		build = new dstring(level);
		build->cat("_report");
		tmp = c->get(build->ascii());
		delete build;
		if( atoi(tmp->ascii()) )
		{
			ret->report = 1;
			ret->level = 0;
			ret->max = 0;
			return ret;
		}
		delete tmp;
		ret->report = 0;
		
		build = new dstring(level);
		build->cat("_max");
		tmp = c->get(build->ascii());
		ret->max = atoi(tmp->ascii());
		ret->level = 0;
		return ret;
	}
}

void *analyze_launch_thread(void *args)
{
	struct analyze_args *a;
	analyze *an;
	struct syslog_message *m;
	cache *c;
	database *loader;
	
	a = (struct analyze_args*)args;
	c = a->cash;
	an = new analyze(a->c);

	for( int b = 0; b < a->c->num_dbs(); b++ )
	{
		loader = new database(a->c->db(b));
		an->load(loader);
		loader = NULL;
	}
	
	while( true )
	{
		m = c->get();
		if( ! an->process(m) )
		{
			free(m);
		}
	}
}

struct analyze_args *analyze_build_args(conf *c, cache *cash)
{
	struct analyze_args *n;
	n = (struct analyze_args*)malloc(sizeof(struct analyze_args));
	n->c = c;
	n->cash = cash;
	return n;
}



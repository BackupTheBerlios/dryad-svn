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

namespace DAnalyze
{
analyze::analyze(conf *c)
{
	struct sev_group *tmp;
	dstring *dname, *t;
	struct reporter *rpt;
	const char *error;
	reporter_config cnf;
	if( c == NULL )
	{
		cerr << "Invalid conf passed to analyze!\nAborting!\n";
		exit(1);
	}
	
	this->c = c;
	
	def = (struct sev_group*)malloc(sizeof(struct sev_group));
	
	def->emergency = this->build_severity_struct(NULL, "emergency");
	def->alert = this->build_severity_struct(NULL, "alert");
	def->critical = this->build_severity_struct(NULL, "critical");
	def->error = this->build_severity_struct(NULL, "error");
	def->warning = this->build_severity_struct(NULL, "warning");
	def->notice = this->build_severity_struct(NULL, "notice");
	def->informational = this->build_severity_struct(NULL, "informational");
	def->debug = this->build_severity_struct(NULL, "debug");
	def->name = NULL;
	
	daemons = new drarray<struct sev_group*>;
	
	for( int q = 0; q < c->num_daemons(); q++ )
	{
		dname = c->daemon_name(q);
		tmp = (struct sev_group*)malloc(sizeof(struct sev_group));
		tmp->emergency = this->build_severity_struct(dname->ascii(), "emergency");
		tmp->alert = this->build_severity_struct(dname->ascii(), "alert");
		tmp->critical = this->build_severity_struct(dname->ascii(), "critical");
		tmp->error = this->build_severity_struct(dname->ascii(), "error");
		tmp->warning = this->build_severity_struct(dname->ascii(), "warning");
		tmp->notice = this->build_severity_struct(dname->ascii(), "notice");
		tmp->informational = this->build_severity_struct(dname->ascii(), "informational");
		tmp->debug = this->build_severity_struct(dname->ascii(), "debug");
		tmp->name = dname;
		daemons->pushback(tmp);
		tmp = NULL;
	}
	
	reports = new drarray<struct reporter*>;
	
	lt_dlinit();
	
	for( int q = 0; q < c->num_daemons(); q++ )
	{
		rpt = (struct reporter*)malloc(sizeof(struct reporter));
		t = c->daemon_get(c->daemon_name(q)->ascii(), "lib_handler");
		if( t == NULL )
		{
			free(rpt);
			continue;
		}
		rpt->dlptr = lt_dlopen( t->ascii() );
		if(!rpt->dlptr)
		{
			cerr << "Failed to open " << c->daemon_get(c->daemon_name(q)->ascii(), "lib_handler") << "!\n";
			free(rpt);
			continue;
		}
		lt_dlerror();
		rpt->once = (reporter_once)lt_dlsym(rpt->dlptr, "dryad_once");
		if( (error = lt_dlerror()) )
		{
			cerr << "Failed to export symbol dryad_once!\n" << error << endl;
			free(rpt);
			continue;
		}
		rpt->many = (reporter_many)lt_dlsym(rpt->dlptr, "dryad_many");
		if( (error = lt_dlerror()) )
		{
			cerr << "Failed to export symbol dryad_many!\n" << error << endl;
			free(rpt);
			continue;
		}
		cnf = (reporter_config)lt_dlsym(rpt->dlptr, "dryad_config");
		if( (error = lt_dlerror()) )
		{
			cerr << "Failed to export symbol dryad_config!\n" << error << endl;
			free(rpt);
			continue;
		}
		(*cnf)(c);
		reports->pushback(rpt);
		rpt = NULL;
	}
	
	def_rep = (struct reporter*)malloc(sizeof(struct reporter));
	def_rep->name = NULL;
	t = c->get("lib_handler");
	if( t == NULL )
	{
		cerr << "A default lib_handler has not been specified!\nAborting!\n";
		exit(1);
	}
	def_rep->dlptr = lt_dlopen( t->ascii() );
	if( def_rep->dlptr == NULL )
	{
		cerr << "Failed to lt_dlopen() the default lib_handler (" << t->ascii() << "):\n" << lt_dlerror() << "\nAborting!\n";
		exit(1);
	}
	lt_dlerror();
	def_rep->once = (reporter_once)lt_dlsym(def_rep->dlptr, "dryad_once");
	if( (error = lt_dlerror()) )
	{
		cerr << "Failed to resolve symbol dryad_once in default lib_handler (" << t->ascii() << "):\n" << error << "\nAborting!\n";
		exit(1);
	}
	def_rep->many = (reporter_many)lt_dlsym(def_rep->dlptr, "dryad_many");
	if( (error = lt_dlerror()) )
	{
		cerr << "Failed to resolve symbol dryad_many in default lib_handler (" << t->ascii() << "):\n" << error << "\nAborting!\n";
		exit(1);
	}
	
	seen = new darray<struct syslog_message*>;
	db_vec = new drarray<database*>;
}

analyze::~analyze()
{
	free(def->emergency);
	free(def->alert);
	free(def->critical);
	free(def->error);
	free(def->warning);
	free(def->notice);
	free(def->informational);
	free(def->debug);
	free(def);
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
	struct sev_group *use;
	use = NULL;
	for( int q = 0; q < daemons->length(); q++ )
	{
		if( !strcmp(daemons->at(q)->name->ascii(), m->daemon->ascii()) )
		{
			use = daemons->at(q);
			break;
		}
	}
	if( use == NULL )
		use = def;
	switch(m->severity)
	{
		case(0):
			useme = use->emergency;
			break;
		case(1):
			useme = use->alert;
			break;
		case(2):
			useme = use->critical;
			break;
		case(3):
			useme = use->error;
			break;
		case(4):
			useme = use->warning;
			break;
		case(5):
			useme = use->notice;
			break;
		case(6):
			useme = use->informational;
			break;
		case(7):
			useme = use->debug;
			break;
	}
	if( ! useme->track )
		return;
	if( (useme->level >= useme->max && useme->max > 0) || (useme->report && useme->all) )
	{
		useme->level = 0;
		this->report(m, useme->all);
	}
	else if( useme->report )
	{
		this->report_once(m);
	}
	else
	{
		useme->level++;
		seen->insert(m);
	}
}

void analyze::report_once( struct syslog_message *m )
{
	pthread_t launch;
	//do stuff to report the single message, involving email, python, perl, whatevar.
	for( int q = 0; q < reports->length(); q++ )
	{
		if( ! strcmp(reports->at(q)->name->ascii(), m->daemon->ascii()) )
		{
			pthread_create(&launch, NULL, reports->at(q)->once, m);
			return;
		}
	}
	pthread_create(&launch, NULL, def_rep->once, m);
}

void analyze::report( struct syslog_message *m, int all )
{
	pthread_t launch;
	drarray<struct syslog_message*> *args;
	args = new drarray<struct syslog_message*>;
	if( all )
	{
		//get all messages of this daemon, at any severity
		args->pushback(m);
		for( int q = 0; q < seen->length(); q++ )
		{
			if( !strcmp(m->daemon->ascii(), seen->get_at(q)->daemon->ascii()) )
			{
				args->pushback(seen->get_at(q));
				seen->del(q);
			}
		}
	}
	else
	{
		//get all messages of this daemon, at this severity
		for( int q = 0; q < seen->length(); q++ )
		{
			if( !strcmp(m->daemon->ascii(), seen->get_at(q)->daemon->ascii()) && (m->severity == seen->get_at(q)->severity) )
			{
				args->pushback(seen->get_at(q));
				seen->del(q);
			}
		}
	}
	for( int q = 0; q < reports->length(); q++ )
	{
		if( ! strcmp(reports->at(q)->name->ascii(), m->daemon->ascii()) )
		{
			pthread_create(&launch, NULL, reports->at(q)->many, args);
			return;
		}
	}
	pthread_create(&launch, NULL, def_rep->many, args);
}

struct severity *analyze::build_severity_struct(const char *daemon, char *level)
{
	struct severity *ret;
	dstring *tmp, *build;
	
	ret = (struct severity*)malloc(sizeof(struct severity));
	
	if( daemon == NULL )
	{
		build = new dstring(level);
		build->cat("_track");
		tmp = c->get(build->ascii());
		if( tmp == NULL )
		{
			cerr << "Failed to get " << build->ascii() << "!\nAborting!\n";
			exit(1);
		}
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
		ret->track = 1;
		
		build = new dstring(level);
		build->cat("_all");
		tmp = c->get(build->ascii());
		if( tmp == NULL )
		{
			cerr << "Failed to get " << build->ascii() << "!\nAborting!\n";
			exit(1);
		}
		ret->all = atoi(tmp->ascii());
		delete build;
		
		build = new dstring(level);
		build->cat("_report");
		tmp = c->get(build->ascii());
		if( tmp == NULL )
		{
			cerr << "Failed to get " << build->ascii() << "!\nAborting!\n";
			exit(1);
		}
		delete build;
		if( atoi(tmp->ascii()) )
		{
			ret->report = 1;
			ret->level = 0;
			ret->max = 0;
			return ret;
		}
		ret->report = 0;
		
		build = new dstring(level);
		build->cat("_max");
		tmp = c->get(build->ascii());
		if( tmp == NULL )
		{
			cerr << "Failed to get " << build->ascii() << "!\nAborting!\n";
			exit(1);
		}
		ret->max = atoi(tmp->ascii());
		ret->level = 0;
		return ret;
	}
	//daemon specified
	else
	{
		build = new dstring(level);
		build->cat("_track");
		tmp = c->daemon_get(daemon, build->ascii());
		if( tmp == NULL )
		{
			tmp = c->get(build->ascii());
		}
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
		ret->track = 1;
		
		build = new dstring(level);
		build->cat("_all");
		tmp = c->daemon_get(daemon, build->ascii());
		if( tmp == NULL )
		{
			tmp = c->get(build->ascii());
		}
		ret->all = atoi(tmp->ascii());
		delete build;
		
		build = new dstring(level);
		build->cat("_report");
		tmp = c->daemon_get(daemon, build->ascii());
		if( tmp == NULL )
		{
			tmp = c->get(build->ascii());
		}
		delete build;
		if( atoi(tmp->ascii()) )
		{
			ret->report = 1;
			ret->level = 0;
			ret->max = 0;
			return ret;
		}
		ret->report = 0;
		
		build = new dstring(level);
		build->cat("_max");
		tmp = c->daemon_get(daemon, build->ascii());
		if( tmp == NULL )
		{
			tmp = c->get(build->ascii());
		}
		ret->max = atoi(tmp->ascii());
		ret->level = 0;
		return ret;
	}
	return NULL;
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

}

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

	emergency = (struct severity*)malloc(sizeof(struct severity));
	alert = (struct severity*)malloc(sizeof(struct severity));
	critical = (struct severity*)malloc(sizeof(struct severity));
	error = (struct severity*)malloc(sizeof(struct severity));
	warning = (struct severity*)malloc(sizeof(struct severity));
	notice = (struct severity*)malloc(sizeof(struct severity));
	informational = (struct severity*)malloc(sizeof(struct severity));
	debug = (struct severity*)malloc(sizeof(struct severity));
	
	emergency->level = 0;
	alert->level = 0;
	critical->level = 0;
	error->level = 0;
	warning->level = 0;
	notice->level = 0;
	informational->level = 0;
	debug->level = 0;
	
	tmp = c->get("emergency_report");
	if( tmp == NULL )
	{
		cerr << "emergency_report not set!\nAborting!\n";
		exit(1);
	}
	emergency->report = atoi(tmp->ascii());
	tmp = c->get("alert_report");
	if( tmp == NULL )
	{
		cerr << "alert_report not set!\nAborting!\n";
		exit(1);
	}
	alert->report = atoi(tmp->ascii());
	tmp = c->get("critical_report");
	if( tmp == NULL )
	{
		cerr << "critical_report not set!\nAborting!\n";
		exit(1);
	}
	critical->report = atoi(tmp->ascii());
	tmp = c->get("error_report");
	if( tmp == NULL )
	{
		cerr << "error_report not set!\nAborting!\n";
		exit(1);
	}
	error->report = atoi(tmp->ascii());
	tmp = c->get("warning_report");
	if( tmp == NULL )
	{
		cerr << "warning_report not set!\nAborting!\n";
		exit(1);
	}
	warning->report = atoi(tmp->ascii());
	tmp = c->get("notice_report");
	if( tmp == NULL )
	{
		cerr << "notice_report not set!\nAborting!\n";
		exit(1);
	}
	notice->report = atoi(tmp->ascii());
	tmp = c->get("informational_report");
	if( tmp == NULL )
	{
		cerr << "informational_report not set!\nAborting!\n";
		exit(1);
	}
	informational->report = atoi(tmp->ascii());
	tmp = c->get("debug_report");
	if( tmp == NULL )
	{
		cerr << "debug_report not set!\nAborting!\n";
		exit(1);
	}
	debug->report = atoi(tmp->ascii());
	
	tmp = c->get("emergency_track");
	if( tmp == NULL )
	{
		cerr << "emergency_track not set!\nAborting!\n";
		exit(1);
	}
	emergency->track = atoi(tmp->ascii());
	tmp = c->get("alert_track");
	if( tmp == NULL )
	{
		cerr << "alert_track not set!\nAborting!\n";
		exit(1);
	}
	alert->track = atoi(tmp->ascii());
	tmp = c->get("critical_track");
	if( tmp == NULL )
	{
		cerr << "critical_track not set!\nAborting!\n";
		exit(1);
	}
	critical->track = atoi(tmp->ascii());
	tmp = c->get("error_track");
	if( tmp == NULL )
	{
		cerr << "error_track not set!\nAborting!\n";
		exit(1);
	}
	error->track = atoi(tmp->ascii());
	tmp = c->get("warning_track");
	if( tmp == NULL )
	{
		cerr << "warning_track not set!\nAborting!\n";
		exit(1);
	}
	warning->track = atoi(tmp->ascii());
	tmp = c->get("notice_track");
	if( tmp == NULL )
	{
		cerr << "notice_track not set!\nAborting!\n";
		exit(1);
	}
	notice->track = atoi(tmp->ascii());
	tmp = c->get("informational_track");
	if( tmp == NULL )
	{
		cerr << "informational_track not set!\nAborting!\n";
		exit(1);
	}
	informational->track = atoi(tmp->ascii());
	tmp = c->get("debug_track");
	if( tmp == NULL )
	{
		cerr << "debug_track not set!\nAborting!\n";
		exit(1);
	}
	debug->track = atoi(tmp->ascii());
	
	tmp = c->get("emergency_max");
	if( tmp == NULL )
	{
		cerr << "emergency_max not set!\nAborting!\n";
		exit(1);
	}
	emergency->max = atoi(tmp->ascii());
	tmp = c->get("alert_max");
	if( tmp == NULL )
	{
		cerr << "alert_max not set!\nAborting!\n";
		exit(1);
	}
	alert->max = atoi(tmp->ascii());
	tmp = c->get("critical_max");
	if( tmp == NULL )
	{
		cerr << "critical_max not set!\nAborting!\n";
		exit(1);
	}
	critical->max = atoi(tmp->ascii());
	tmp = c->get("error_max");
	if( tmp == NULL )
	{
		cerr << "error_max not set!\nAborting!\n";
		exit(1);
	}
	error->max = atoi(tmp->ascii());
	tmp = c->get("warning_max");
	if( tmp == NULL )
	{
		cerr << "warning_max not set!\nAborting!\n";
		exit(1);
	}
	warning->max = atoi(tmp->ascii());
	tmp = c->get("notice_max");
	if( tmp == NULL )
	{
		cerr << "notice_max not set!\nAborting!\n";
		exit(1);
	}
	notice->max = atoi(tmp->ascii());
	tmp = c->get("informational_max");
	if( tmp == NULL )
	{
		cerr << "informational_max not set!\nAborting!\n";
		exit(1);
	}
	informational->max = atoi(tmp->ascii());
	tmp = c->get("debug_max");
	if( tmp == NULL )
	{
		cerr << "debug_max not set!\nAborting!\n";
		exit(1);
	}
	debug->max = atoi(tmp->ascii());
	
	tmp = c->get("emergency_all");
	if( tmp == NULL )
	{
		cerr << "emergency_all not set!\nAborting!\n";
		exit(1);
	}
	emergency->all = atoi(tmp->ascii());
	tmp = c->get("alert_all");
	if( tmp == NULL )
	{
		cerr << "alert_all not set!\nAborting!\n";
		exit(1);
	}
	alert->all = atoi(tmp->ascii());
	tmp = c->get("critical_all");
	if( tmp == NULL )
	{
		cerr << "critical_all not set!\nAborting!\n";
		exit(1);
	}
	critical->all = atoi(tmp->ascii());
	tmp = c->get("error_all");
	if( tmp == NULL )
	{
		cerr << "error_all not set!\nAborting!\n";
		exit(1);
	}
	error->all = atoi(tmp->ascii());
	tmp = c->get("warning_all");
	if( tmp == NULL )
	{
		cerr << "warning_all not set!\nAborting!\n";
		exit(1);
	}
	warning->all = atoi(tmp->ascii());
	tmp = c->get("notice_all");
	if( tmp == NULL )
	{
		cerr << "notice_all not set!\nAborting!\n";
		exit(1);
	}
	notice->all = atoi(tmp->ascii());
	tmp = c->get("informational_all");
	if( tmp == NULL )
	{
		cerr << "informational_all not set!\nAborting!\n";
		exit(1);
	}
	informational->all = atoi(tmp->ascii());
	tmp = c->get("debug_all");
	if( tmp == NULL )
	{
		cerr << "debug_all not set!\nAborting!\n";
		exit(1);
	}
	debug->all = atoi(tmp->ascii());
	
	emergency->all = atoi(c->get("emergency_all")->ascii());
	alert->all = atoi(c->get("alert_all")->ascii());
	critical->all = atoi(c->get("critical_all")->ascii());
	error->all = atoi(c->get("error_all")->ascii());
	warning->all = atoi(c->get("warning_all")->ascii());
	notice->all = atoi(c->get("notice_all")->ascii());
	informational->all = atoi(c->get("informational_all")->ascii());
	debug->all = atoi(c->get("debug_all")->ascii());
	
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

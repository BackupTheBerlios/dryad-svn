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
#ifndef ANALYZE_H
#define ANALYZE_H


#include "conf.h"
#include "syslog.h"
#include "darray.h"
#include "drarray.h"
#include "database.h"
#include "cache.h"

#include <iostream.h>
#include <pcre.h>

struct severity {
	//! The level of warnings of this severity
	int level;
	//! If true, report occurences as soon as they happen
	int report;
	//! If false, this severity of message will be disregarded. Primarly used for debug/informational.
	int track;
	//! The level at which to report
	int max;
	//! If true, all severities for a given daemon will be reported when any one of them triggers, otherwise just that severity level will go off
	int all;
};

struct analyze_args {
	conf *c;
	cache *cash;
};

class analyze
{
public:
	analyze(conf *c);
	~analyze();
	
	int load( database *db );
	
	int process( struct syslog_message *m );

private:
	void reg( struct syslog_message *m );
	void report_once( struct syslog_message *m );
	void report( struct syslog_message *m, int all );
	struct severity *emergency;
	struct severity *alert;
	struct severity *critical;
	struct severity *error;
	struct severity *warning;
	struct severity *notice;
	struct severity *informational;
	struct severity *debug;
	darray<struct syslog_message*> *seen;
	drarray<database*> *db_vec;
};

void *analyze_launch_thread(void *args);

struct analyze_args *analyze_build_args(conf *c, cache *cash);

#endif

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

#include <dlfcn.h>
#include <iostream>
#include <pcre.h>

namespace DAnalyze
{
using std::cerr;
using std::endl;
using DCache::cache;
using DConf::conf;
using DArray::darray;
using DDatabase::database;
using DRArray::drarray;
using DString::dstring;
using Syslog::syslog_message;

//! This struct gets used by analyze to track it's default settings.
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

struct sev_group {
	struct severity *emergency;
	struct severity *alert;
	struct severity *critical;
	struct severity *error;
	struct severity *warning;
	struct severity *notice;
	struct severity *informational;
	struct severity *debug;
	dstring *name;
};

//! This struct is used to pass arguments for the purpose of thread starting.
struct analyze_args {
	conf *c;
	cache *cash;
};

//! In reality, the arg is a syslog_message*
typedef void* (*reporter_once)(void *args);
//! In reality, the arg is a drarray<syslog_message*>*
typedef void* (*reporter_many)(void *args);
//! In reality, the arg is the conf object
typedef void* (*reporter_config)(void *args);
//! This struct is used to store report options for each daemon
struct reporter {
	dstring *name;
	reporter_once once;
	reporter_many many;
	void *dlptr;
};

class analyze
{
public:
	//! The basic constructor
	/*!
		\param c The configuration object to draw information from. This object MUST be initialised before being passed here.
	*/
	analyze(conf *c);
	~analyze();
	
	//! Load a database
	/*!
		\param db The database to load. What gets stored is the pointer to the database. The actual contents REMAIN THE SAME! Do NOT free() or delete the database object after it has been loaded!
		\return If db is NULL, this method will return false, otherwise it will return true.
	*/
	int load( database *db );
	
	//! Process a syslog message
	/*!
		\param m A pointer to the syslog message to process.
		\return If m is NULL, -1 is returned. Otherwise, if m->message has a matching rule, true is returned. Finally, if there is no match, false is returned.
	*/
	int process( struct syslog_message *m );

private:
	// These are internal functions, and there for are not documented.
	void reg( struct syslog_message *m );
	void report_once( struct syslog_message *m );
	void report( struct syslog_message *m, int all );
	//! use daemon = null to get top levels
	struct severity *build_severity_struct(const char *daemon, char *level);
	struct sev_group *def;
	darray<struct syslog_message*> *seen;
	drarray<database*> *db_vec;
	drarray<struct sev_group*> *daemons;
	drarray<struct reporter*> *reports;
	struct reporter *def_rep;
	conf *c;
};

//! Launches the analyze thread
/*!
	\param args A struct analyze_args that contains the material to work with, namely a pointer to the config object and a pointer to the cache object. This call does not return.
*/
void *analyze_launch_thread(void *args);

//! Builds a struct analyze_args.
struct analyze_args *analyze_build_args(conf *c, cache *cash);

}

#endif

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
#ifndef CONF_H
#define CONF_H

#include "dstring.h"
#include "dfilestream.h"
#include "drarray.h"
#include "database.h"

#include <iostream.h>
#include <stdlib.h>
#include <pthread.h>

//! A simple struct for semi assoicatative arrays.
struct variable {
	dstring *name;
	dstring *value;
};

//! For storage of info pertaining to a specific daemon
struct daemon_section {
	dstring *name;
	drarray<variable*> *contents;
};

class conf
{
public:
	conf( dstring *cf );
	~conf();
	
	int num_dbs() const;
	dstring *db(int k) const;
	
	dstring *get(const char *name);
	dstring *daemon_get(const char *daemon, const char *name);
	int num_daemons() const;
	dstring *daemon_name(int k);

private:
	void readconfig( dfilestream *fs );
	dstring *cfile;
	pthread_mutex_t *reload;
	drarray<variable*> *vars;
	drarray<dstring*> *dbs;
	drarray<daemon_section*> *daemons;
};

#endif

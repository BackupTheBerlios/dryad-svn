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
#include "dryerr.h"

#include <iostream>
#include <stdlib.h>
#include <pthread.h>

namespace DConf
{
using std::cerr;
using DFileStream::dfilestream;
using DRArray::drarray;
using DString::dstring;
using DError::dryerr;

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

//! The conf class is used to parse config files
/*!
	This class is very simple and, I think, fairly elegent. It doesn't really do any syntax enforcement, that is left to objects pulling data from the conf object, it simply creates in memory mappings from variable names to values
*/
class conf
{
public:
	//! The basic constructor
	/*!
		\param cf The config file to read from. It must be a valid file, with valid and sufficient configuration options, or else the program will abort.
	*/
	conf( dstring *cf );
	~conf();
	
	//! Returns the number of dbs
	/*!
		\return The number of database configuration lines read from the config file.
	*/
	int num_dbs() const;
	//! Get the path to a database
	/*
		\param k The index of the database to get.
		\return The path to the database file. No checking is done to ensure this is valid.
		If you pass an out of range index, NULL is returned.
	*/
	dstring *db(int k) const;
	
	//! Get a global config variable
	/*!
		\param name The name of the variable you want to get.
		\return The contents of the variable. If the given name isn't found, NULL is returned. If it's found more than once, the first occurence will be returned.
		The way variables are stored to disk, currently, is in a pair of string:\n
		var_name value\n
		It is then split on the first space, with the first half going to the name, and the second to the contents.
	*/
	dstring *get(const char *name);
	//! Get a daemon scope config variable
	/*!
		\param daemon The name of the daemon to use.
		\param name The name of the variable you want to get.
		\param all If not null, this will be filled with *all* occurencess of name in that scope. In that case, the *first* one found will be returned.
		\return The value of the requested variable. If it's not found, or the specified daemon is not found, NULL is returned. Again, if multiple occurences exist, the first is returned.
		\sa get()
		This method works a lot like get(), with the difference that it will use a daemon scope, defined thusly:\n
		BEGIN daemon_name\n
		var_name value\n
		var2_name value2\n
		...\n
		END\n
	*/
	dstring *daemon_get(const char *daemon, const char *name, drarray<dstring*> *all);
	dstring *daemon_get(const char *daemon, const char *name);
	//! Returns the number of daemons
	/*!
		\return The number of daemon sections defined in the config file.
	*/
	int num_daemons() const;
	//! Get's the name of a daemon at a given index
	/*!
		\param k The index to check at.
		\return The name of the daemon at that index. If it is not a valid index, NULL is returned.
	*/
	dstring *daemon_name(int k);

private:
	void readconfig( dfilestream *fs );
	dstring *cfile;
	pthread_mutex_t *reload;
	drarray<variable*> *vars;
	drarray<dstring*> *dbs;
	drarray<daemon_section*> *daemons;
};

}

#endif

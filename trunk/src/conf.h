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

struct daemon {
	dstring *name;
	int warn_level;
	int clear_on_warn;
	int error_level;
	int clear_on_error;
};

//! The config class
/*!
	The basic purpose of this class is to load and store config information.
*/
class conf {
public:
	//! The construcor. You *must* pass the path to the config file.
	/*!
		\param c The path to the config file. Make sure you ensure that it is good first.
	*/
	conf( dstring *c );
	~conf();
	
	//! Reloads the config, to be used when a sighup is recieved
	int reload();
	
	//! Returns the information for the given daemon
	/*!
		\param name The name of the daemon to get info on. Use a NULL pointer to get information on the default settings.
		\return A daemon* struct if it's found, otherwise NULL.
		NOTE: This does not make a copy of the struct, it simply returns the pointer to the instance in memory.
		\sa get_daemon(char *name)
  */ 
	struct daemon *get_daemon(dstring *name) const;
	//! Returns the information for the given daemon
	/*!
		\param name The name of the daemon to get info on. Use a NULL pointer to get information on the default settings.
		\return A daemon* struct if it's found, otherwise NULL.
		NOTE: This does not make a copy of the struct, it simply returns the pointer to the instance in memory.
		\sa get_daemon(dstring *name)
	*/
	struct daemon *get_daemon(char *name) const;

private:
	//! This is the actual funtion that loads the config file
	/*!
		\param f The file stream to read from
		\return True if the load worked, otherwise false
	*/
	int loadconfig(dfilestream *f);
	//! This verifies that the config settings are sane
	int checkconfig();
	int warn_level;
	int clear_on_warn;
	int error_level;
	int clear_on_error;
	struct daemon **daemons;
	int num_daemons;
	dstring *file;
};

#endif

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

#include <pthread.h>
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
	The basic purpose of this class is to load and store config information.\n\n
	Thread Safe: I believe so. The struct returned from get_daemon can be modified, in particular, the name dstring, however an array of semaphores is used to track that. The databases themselves may also be modified, however that class should be thread safe itself.
	
	Document structure:\n
	Configuration for a specific daemon is set of in a block, denoted by BEGIN and END tags. The BEGIN tag must specifiy the name of the daemon:\n
	BEGIN daemonname\n
	END tags do not need any additional content, though they may have them, should you so desire\n
	Between the BEGIN and END tags go the parameters for the given daemon. Currently the valid params are:\n
	- warn_level -- [0-2^32] The point value to initiate the warning procedure for that daemon
	- clear_on_warn -- [0-1] Should the register of seens strings be cleared of all entries for that daemon when warn_level is reached
	- error_level -- [0-2^32] The point value to initiate the error procedure for that daemon
	- clear_on_error -- [0-1] Should the register of seen strings be cleared of all entries for the daemon when error_level is reached (1 is recommended strongly)
	In order to set global setting, simply place the params outside of any daemon block.
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
		NOTE: This does not make a copy of the struct, it simply returns the pointer to the instance in memory.\n
		This function will block until the requested daemon is available.
		\sa get_daemon(char *name)
  */ 
	struct daemon *get_daemon(dstring *name) const;
	//! Returns the information for the given daemon
	/*!
		\param name The name of the daemon to get info on. Use a NULL pointer to get information on the default settings.
		\return A daemon* struct if it's found, otherwise NULL.
		NOTE: This does not make a copy of the struct, it simply returns the pointer to the instance in memory.\n
		This function will block until the requested daemon is available.
		\sa get_daemon(dstring *name)
	*/
	struct daemon *get_daemon(char *name) const;
	
	//! Unlocks the given daemon
	/*!
		\param name The daemon to unlock.
		NOTE: This function MUST be called when you are done with the daemon you got from get_daemon()
		\sa get_daemon()
	*/
	void unlock(char *name);
	
	//! The number of databases defined
	/*!
		\return the number of databases defined
	*/
	int num_dbs() const;
	
	//! Returns the dstring at the given index
	/*!
		\param k The index
		\return the dstring at that index
	*/
	dstring *db(int k) const;
	
	//! Returns the level of the given index
	/*!
		\param k The index
		\return the int at that index
	*/
	int db_level(int k) const;
	
	#ifdef DEBUG
	void dump();
	#endif

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
	int num_db;
	dstring **dbs;
	int *db_levels;
	pthread_mutex_t **daemon_lock;
};

#endif

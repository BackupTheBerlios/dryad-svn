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

#include "database.h"
#include "dsarray.h"
#include <pcre.h>



//! Performs analysis of strings
/*!
	This class performs the meat of the actual functionality. It process strings, and compares them against known ones. It tracks the warning level, and issues reports as needed.
*/
class analyze {
public:
	//! The basic constructor
	/*!
		note that this is the prefered way of instantiating the class
	*/
	analyze();
	//! The complex constructor
	/*!
		\param db_list An array of database objects to use to analyze strings
		\param count The number of elements in db_list
		While this does work as a way to get database objects in, it is not the prefered way. Unless you really need to, please use load() instead.
		\sa load()
	*/
	analyze( database *db_list[] );
	~analyze();
	
	//! Loads a database object into the set
	/*!
		\param db The database object to load
		\return False if db is null, otherwise True.
		Once a database object has been loaded via this method, they will be used for all future log checks
	*/
	int load( database *db );
	
	//! Process a log string
	/*!
		\param str The string to process
		\return The level of severity of the string, 0 if not found. -1 if not databases are load()ed
		This function also increments the warning level, if needed, and performs other appropriate actions
		\sa register()
	*/
	int process( dstring *str );

private:
	//! Registers a log string in the database of already seen ones
	/*!
		\param str The string to register
		\param level The warning level of str
		\return True if not seen before, otherwise false
		\sa process() flush_register()
		This function should not be called directly.
	*/
	int reg( dstring *str, int level );
	
	//! Flushes the register of log messages
	/*!
		\param reset if True, the point level will be reset to 0.
		\return False if no entries in register, otherwise True.
		This function is generally called after reporting.
		\sa report()
	*/
	int flush_register( int reset );
	
	//! Reports on the status of log levels
	/*!
		\return True if the report was sent off successfuly, otherwise False.
		This function performs the user configurable actions to undertake when various point levels are reached.
	*/
	int report();
	
	//! The vector to store the database objects.
	database **db_vec;
	int num_db;
	//! The current count of points
	int points;
	//! the register of already seen log strings
	dsarray *a;
	
};

#endif

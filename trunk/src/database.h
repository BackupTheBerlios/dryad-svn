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
#ifndef DATABASE_H
#define DATABASE_H

#include "dstring.h"
#include "dfilestream.h"

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <pcre.h>

namespace DDatabase
{
using std::cerr;
using std::endl;
using DFileStream::dfilestream;
using DString::dstring;

struct list {
	pcre *re;
	pcre_extra *rs;
	dstring *str;
	dstring *daemon;
	struct list *next;
	struct list *prev;
};


//! Stores log strings
/*!
	Basically, this class is used to index, in memory, all the known log strings.\n
	It includes information on what daemon produces the log, as well as the severity of that message.\n
	File Structure:\n
	Files are to be organised by daemon type. To begin a block of daemon output, use the BEGIN /type/ tag.\n
	Eg, the apache block begins:\n
	BEGIN apache\n
	Blocks do not need to be ended, but may be if so desired, with the END /type/ tag, where /type/ is totally optional.\n
	Following the BEGIN tag, there should be one possible log output per line. All sorts of regex syntax may be used, as the string will be pattern matched by the pcre library.
*/
class database {
public:
	//! The basic constructor
	/*!
		database() just does the basic init'ing stuff. Nothing more.
		\sa load()
	*/
	database();
	
	//! The complex constructor
	/*!
		\param path Specifies the path to the definitions to load
		\sa load()
	*/
	database( dstring *path );
	
	~database();
	
	//! Loads log definitions after instantiantion
	/*!
		\param path Specifies the path to the definitions to load
		\return True.
	*/
	int load( dstring *path );
	
	//! Advances to the next log entry
	/*!
		\return False if at the last element, otherwise true.
	*/
	int next();
	//! Rewinds one, to the previous log entry.
	/*!
		\return False if at the first element, otherwise true.
	*/
	int prev();
	//! Resets to the first element.
	/*!
		\return False if already at the first element, -1 if there are 0 entries, otherwise true
	*/
	int reset();
	
	//! Returns the current log string
	dstring *string() const;
	//! Return the daemon that produces the current log string
	dstring *daemon() const;
	//! The compiled regular expression
	pcre *re() const;
	//! The results of studying the regular expression
	pcre_extra *rs() const;
	
	#ifdef DEBUG
	//! Dumps the contents of the database to stderr.
	/*!
		THIS IS A DEBUG ONLY FUNCTION
	*/
	void dump();
	#endif

private:
	//! The linked list to store this data.
	struct list *mList;
	//! The current node
	struct list *curr;
	//! The current position in mlist, this is a *0* based index!
	int curr_pos;
	//! The total number of positions in mlist. This is a *1* based index!
	int max_pos;
};

}

#endif

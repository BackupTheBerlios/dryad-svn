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
#include "plugin.h"

/*
	This is a very simple example reporter plugin. When it gets called, if sends a dump of the contents of the syslog messages to a compile tiem defined email address.
*/

extern "C" {
	
	// The function used to report a single incident must be called dryad_once. Before it returns, it should also free() the passed syslog_message
	int dryad_once(struct syslog_message *m)
	{
		cerr << "dryad_once called!\n";
		free(m);
	}
	
	//! Before it returns, we need to free() all messages in m, and delete m itself.
	int dryad_many(drarray<struct syslog_message*> *m)
	{
		for( int c = 0; c < m->length(); c++ )
		{
			free(m->at(c));
		}
		delete m;
	}

}

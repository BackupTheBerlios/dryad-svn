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

analyze::analyze()
{
	points = 0;
	num_db = 0;
	db_vec = NULL;
}

analyze::analyze( database *db_list[] )
{
	if( db_list )
	{
		db_vec = db_list;
	}
	else
	{
		db_vec = NULL;
	}
	points = 0;
	num_db = 0;
}

analyze::~analyze()
{
	if( db_vec )
		free(db_vec);
	db_vec = NULL;
}

int analyze::load( database *db )
{
	database **n;
	if( ! db )
		return false;
	
	num_db++;
	if( ! db_vec )
	{
		db_vec = (database**)malloc( sizeof(database *) );
		db_vec[0] = db;
	}
	else
	{
		n = (database**)malloc( sizeof(database *) * (num_db) );
		for( int c = 0; c < num_db - 1; c++ )
		{
			n[c] = db_vec[c];
		}
		n[num_db - 1] = db;
		free(db_vec);
		db_vec = n;
	}
	return true;
}

int analyze::process( dstring *str )
{
	int rr;
	int ovector[3];

	if( ! db_vec )
		return -1;
	if( "" == str->ascii() )
		return false;
	
	for( int c = 0; c < num_db; c++ )
	{
		if( (db_vec[c])->reset() == -1 )
		{
			continue;
		}
		do
		{
			rr = pcre_exec( (db_vec[c])->re(), (db_vec[c])->rs(), str->ascii(), str->length(), 0, 0, ovector, 3 );
			if( rr != PCRE_ERROR_NOMATCH )
			{
				reg( str, (db_vec[c])->level() );
			}
		} while( (db_vec[c])->next() );
	}
}

int reg( dstring *str, int level )
{

}

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
#include "database.h"

database::database()
{
	mList = NULL;
	curr_pos = 0;
	max_pos = 0;
	curr = NULL;
}

database::database( dstring *path )
{
	dfilestream *fs;
	dstring *tmp, *currDaemon;
	pcre *re;
	const char *error;
	int eoffset;
	struct stat *buf;
	
	currDaemon = new dstring("NULL");
	curr_pos = 0;
	max_pos = 0;
	curr = NULL;
	mList = NULL;
	
	buf = (struct stat*)malloc( sizeof(struct stat) );
	if( -1 == stat( path->ascii(), buf ) )
	{
		cerr << "In database::database( dstring *path, int level) -- Unable to stat path (" << path->ascii() << ")! Aborting.\n";
		exit(1);
	}
	free(buf);
	buf = NULL;
	
	fs = new dfilestream( path, "r" );
	
	while( tmp = fs->readline() )
	{
		if( ! strncmp( "BEGIN ", tmp->ascii(), 6 ) ) // strncmp is bassackwards, needs !
		{
			currDaemon = tmp->remove( "BEGIN " );
			if( "" == currDaemon->ascii() )
				*currDaemon = "NULL";
		}
		else if( !strncmp( "END", tmp->ascii(), 3 ) || !strncmp( "#", tmp->ascii(), 1 ) ) //again with the !s
		{
			//do nothing!
		}
		else
		{
			re = pcre_compile( tmp->ascii(), 0, &error, &eoffset, NULL );
			if( ! re )
			{
				cerr << "Unable to compile the regex \"" << tmp->ascii() << "\"!\nFailure at offset " << eoffset << ":" << error << endl;
			}
			else
			{
				if( ! mList )
				{
					mList = (struct list*)malloc( sizeof( struct list ) );
					curr = mList;
					curr->prev = NULL;
					curr->next = NULL;
				}
				else
				{
					curr->next = (struct list*) malloc( sizeof( struct list ) );
					curr->next->prev = curr;
					curr = curr->next;
					curr->next = NULL;
				}
				curr->rs = pcre_study(re, 0, &error);
				curr->str = tmp;
				curr->re = re;
				curr->daemon = currDaemon;
				curr->next = NULL;
				max_pos++;
			}
		}
	}
	curr = mList;
}

int database::load( dstring *path )
{
	dfilestream *fs;
	dstring *tmp, *currDaemon;
	pcre *re;
	const char *error;
	int eoffset;
	struct stat *buf;
	
	currDaemon = new dstring("NULL");
	curr_pos = 0;
	max_pos = 0;
	
	buf = (struct stat*)malloc( sizeof(struct stat) );
	if( -1 == stat( path->ascii(), buf ) )
	{
		cerr << "In database::database( dstring *path, int level) -- Unable to stat path (" << path->ascii() << ")! Aborting.\n";
		exit(1);
	}
	free(buf);
	buf = NULL;
	
	fs = new dfilestream( path, "r" );
	
	while( tmp = fs->readline() )
	{
		if( ! strncmp( "BEGIN ", tmp->ascii(), 6 ) ) // strncmp is bassackwards, needs !
		{
			currDaemon = tmp->remove( "BEGIN " );
			if( "" == currDaemon->ascii() )
				*currDaemon = "NULL";
		}
		else if( !strncmp( "END", tmp->ascii(), 3 ) || !strncmp( "#", tmp->ascii(), 1 ) ) //again with the !s
		{
			//do nothing!
		}
		else
		{
			re = pcre_compile( tmp->ascii(), 0, &error, &eoffset, NULL );
			if( ! re )
			{
				cerr << "Unable to compile the regex \"" << tmp->ascii() << "\"!\nFailure at offset " << eoffset << ":" << error << endl;
			}
			else
			{
				if( ! mList )
				{
					mList = (struct list*)malloc( sizeof( struct list ) );
					curr = mList;
					curr->prev = NULL;
				}
				else
				{
					curr->next = (struct list*) malloc( sizeof( struct list ) );
					curr->next->prev = curr;
					curr = curr->next;
				}
				curr->rs = pcre_study(re, 0, &error);
				curr->str = tmp;
				curr->re = re;
				curr->daemon = currDaemon;
				curr->next = NULL;
				max_pos++;
			}
		}
	}
	curr = mList;
	return true;
}

database::~database()
{
	if( max_pos > 0 && mList )
	{
		curr = mList;
		for( int c = 0; c < max_pos; c++ )
		{
			if( curr->next )
			{
				curr = curr->next;
				free( curr->prev );
				curr->prev = 0;
			}
			else
			{
				free( curr );
			}
		}
	}
	curr = 0;
}


int database::next()
{
	curr_pos++;
	if( curr_pos >= max_pos || ! curr->next )
	{
		curr_pos--;
		return false;
	}
	else
	{
		curr = curr->next;
		return true;
	}
}

int database::prev()
{
	curr_pos--;
	if( curr_pos < 0 || ! curr->prev )
	{
		curr_pos++;
		return false;
	}
	else
	{
		curr = curr->prev;
		return true;
	}
}

int database::reset()
{
	if( 0 == curr_pos )
		return false;
	else if( 0 == max_pos )
		return -1;
	else
	{
		curr = mList;
		return true;
	}
}

dstring *database::daemon() const
{
	if( ! curr )
		return &dstring("");
	else
		return curr->daemon;
}

dstring *database::string() const
{
	if( ! curr )
		return &dstring("");
	else
		return curr->str;
}

pcre *database::re() const
{
	return curr->re;
}

pcre_extra *database::rs() const
{
	return curr->rs;
}

#ifdef DEBUG
void database::dump()
{
	struct list *temp;
	temp = mList;
	for( int c = 0; c < max_pos; c++ )
	{
		cerr << temp->str->ascii() << "(" << temp->str << ")" << " - " << temp->daemon->ascii() << endl;
		cerr << temp->re << " - " << temp->rs << endl;
		temp = temp->next;
	}
}
#endif

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
#include "dfilestream.h"

dfilestream::dfilestream()
{
	fh = NULL;
}

dfilestream::dfilestream( dstring *path )
{
	struct stat *buf;
	
	buf = (struct stat*)malloc( sizeof(struct stat) );
	if( -1 == stat( path->ascii(), buf ) )
	{
		free(buf);
		fh = NULL;
	}
	else
	{
		free(buf);
		fh = fopen(path->ascii(), "r");
	}
}

dfilestream::dfilestream( char *path )
{
	struct stat *buf;
	
	buf = (struct stat*)malloc( sizeof(struct stat) );
	if( -1 == stat( path, buf ) )
	{
		free(buf);
		fh = NULL;
	}
	else
	{
		free(buf);
		buf = NULL;
		fh = fopen( path, "r" );
	}
}

dfilestream::~dfilestream()
{
	fclose(fh);
}

int dfilestream::open( dstring *path )
{
	if( fh != NULL )
		fclose(fh);
	struct stat *buf;
	
	buf = (struct stat*)malloc( sizeof(struct stat) );
	if( -1 == stat( path->ascii(), buf ) )
	{
		free(buf);
		fh = NULL;
		return false;
	}
	else
	{
		free(buf);
		fh = fopen( path->ascii(), "r" );
		return ( fh != NULL );
	}
}

int dfilestream::open( char *path )
{
	if( fh != NULL )
		fclose(fh);
	struct stat *buf;
	
	buf = (struct stat*)malloc( sizeof(struct stat) );
	if( -1 == stat( path, buf ) )
	{
		free(buf);
		fh = NULL;
		return false;
	}
	else
	{
		free(buf);
		fh = fopen( path, "r" );
		return (fh != NULL);
	}
}

//for the time being, lines can't be larger than 80 characters :(
dstring *dfilestream::readline()
{
	dstring *b;
	int c, olen;
	char *buf;
	int again;
	
	again = true;

	if( fh == NULL )
	{
		b = NULL;
		return b;
	}
	
	b = new dstring(0);
	while( again )
	{
		buf = (char*)malloc( sizeof(char) * 80 );
		if(buf = fgets(buf, 80, fh))
		{
			//this check is *probablly* unneeded, but it's possible that EOF might, somewhere, sometime, not be 0/NULL/false
			if( buf == (char*)EOF )
			{
				again = false;
				break;
			}
			for( c = 0; c < 80; c++ )
			{
				if( buf[c] == '\n' )
				{
					again = false;
					break;
				}
			}
			olen = b->length();
			b->resize( olen + c );
			for( int a = olen; a < b->length(); a++ )
			{
				(*b)[a] = buf[a];
			}
		}
		else
		{
			again = false;
			delete b;
			b = NULL;
		}
		free(buf);
	}
	return b;
}

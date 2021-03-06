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

namespace DFileStream
{
dfilestream::dfilestream()
{
	fh = NULL;
	filemode = '\0';
	filename = NULL;
}

dfilestream::dfilestream( dstring *path, char *mode )
{
	fh = NULL;
	filemode = '\0';
	filename = NULL;
	this->open( path, mode );
}

dfilestream::dfilestream( char *path, char *mode )
{
	fh = NULL;
	filemode = '\0';
	filename = NULL;
	this->open( path, mode );
}

dfilestream::~dfilestream()
{
	fclose(fh);
}

int dfilestream::open( dstring *path, char *mode )
{
	if( path == NULL )
		return false;
	return this->open(path->ascii(), mode);
}

int dfilestream::open( const char *path, char *mode )
{
	dstring *tmpname;
	int f;
	
	if(path == NULL )
		return false;
	if( fh != NULL )
	{
		fclose(fh);
		if( filename != NULL )
			delete filename;
	}
	struct stat *buf;
	
	if( path != NULL )
	{
		buf = (struct stat*)malloc( sizeof(struct stat) );
		if( -1 == stat( path, buf ) || ! S_ISREG(buf->st_mode) )
		{
			free(buf);
			fh = NULL;
			return false;
		}
		free(buf);
	}
	
	if( strcmp(mode, "r") || strcmp(mode, "a") )
	{
		mode = "r";
	}
	if( !strcmp(mode, "r") && path == NULL )
	{
		fh = NULL;
		return false;
	}
	filemode = mode;
	if( !strcmp(filemode, "a") && path == NULL )
	{
		tmpname = new dstring("/tmp/dryad-cache-XXXXXX");
		if( -1 == (f = mkstemp((char*)tmpname->ascii())) )
		{
			cerr << "Failed to create temporary file!\nAborting!\n";
			exit(1);
		}
		#ifdef DEBUG
		cerr << "Temporary file created: " << tmpname->ascii() << endl;
		#endif
		fh = fdopen(f, "a");
		filename = new dstring(tmpname);
		delete tmpname;
	}
	else
	{
		filename = new dstring((char*)path);
		fh = fopen( path, mode );
	}
	return (fh != NULL);
}

dstring *dfilestream::readline()
{
	dstring *b;
	int c, olen;
	char *buf;
	int again;
	
	again = true;

	if( fh == NULL || filemode == "a" )
	{
		b = NULL;
		return b;
	}
	
	b = new dstring;
	while( again )
	{
		buf = (char*)malloc( sizeof(char) * 80 );
		if(buf = fgets(buf, 80, fh))
		{
			//this check is *probablly* unneeded, but it's possible that EOF might, somewhere, sometime, not be 0/NULL/false
			if( buf[0] == EOF )
			{
				again = false;
				free(buf);
				buf = NULL;
				delete b;
				b = NULL;
				break;
			}
			for( c = 0; c < 80; c++ )
			{
				if( buf[c] == '\n' || (buf[c] == '\0' && c < 79) )
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
		buf = NULL;
	}
	if( NULL != buf )
		free(buf);
	return b;
}

void dfilestream::writeline(dstring *line)
{
	if( fh == NULL || strcmp(filemode, "a") || line == NULL )
		return;
	
	if( EOF == fputs(line->ascii(), fh) )
	{
		cerr << "ERROR! Unable to write to file!\nAborting!\n";
		exit(1);
	}
	if( EOF == fputc('\n', fh) )
	{
		cerr << "ERROR! Unable to write to file!\nAborting!\n";
		exit(1);
	}
}

void dfilestream::writeline(int line)
{
	char *a;
	if( fh == NULL || strcmp(filemode, "a") )
		return;
	
	a = itoa(line);
	if( EOF == fputs(a, fh) )
	{
		cerr << "ERROR! Unable to write to file!\nAborting!\n";
		exit(1);
	}
	if( EOF == fputc('\n', fh) )
	{
		cerr << "ERROR! Unable to write to file!\nAborting!\n";
		exit(1);
	}
	free(a);
}

dstring *dfilestream::get_filename() const
{
	return filename;
}
}

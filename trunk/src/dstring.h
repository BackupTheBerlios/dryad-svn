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
#ifndef DSTRING_H
#define DSTRING_H

#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <pthread.h>

//! Dryad-string
/*!
	This the basic string class used by dryad.\n
	NOTE: There will always be one cell beyond len with a \\0 in it.
*/
class dstring {

public:
	//! The basic empty string constructor
	dstring();
	//! The copy constructor
	/*!
		\param s The string to copy from
		The big difference between this and dstring( dstring &s ) is simply in how the parameter is passed.
	*/
	dstring( dstring *s );
	//! The other copy constructor
	/*!
		\sa dstring(dstring *s);
	*/
	dstring( dstring &s );
	//! Another copy constructor
	/*!
		\param s The character array to copy from
		Unlike the other two copy constructors, this one copys from a character array, not a dstring
		NOTE: s MUST be null terminated!
	*/
	dstring( char *s );
	//! The empty constructor
	/*!
		\param c The number of elements to create
		All elements will be set to ' '
	*/
	dstring( int c );
	~dstring();
	
	//! The length of the array
	/*!
		\return An int specifying the number of elements in the array
	*/
	int length() const;
	
	//! Returns the cstring
	/*!
		\return a C style character array. Null terminated.
		This is actually a *copy* of the cstring.
	*/
	char *cstring();
	
	//! Returns the cstring
	/*!
		\sa cstring()
		Unlike cstring, this returns a pointer to the internal cstring itself.
	*/
	const char *ascii();
	
	//! Removes r from the string
	/*!
		\param r The string to remove
		\return A new dstring without r.
		If r is not found, the original dstring is returned (well, a copy of the original really).
		
		If r is longer than the string, an empty string is returned.
		\sa remove( char *r )
	*/
	dstring* remove( dstring *r );
	//! Removes r from the string
	/*!
		\param r The string to remove
		\return A new dstring without r.
		This functions identically to the above, the sole difference being the parameter it takes.
		
		NOTE: As always, r MUST be null terminated.
	*/
	dstring* remove( char *r );
	
	//! Resize the string to this size
	/*!
		\param s The new size
		If expanding the string, all the new characters will be \\0.
		
		If contracting it, characters will be truncated from high to low
	*/
	void resize( int s );
	
	dstring *prior(char t);
	dstring *following(char t);
	
	void cat(char *c);
	
	const dstring & operator = ( const dstring & s );
	const dstring & operator = ( const dstring * s );
	const dstring & operator = ( const char * s );
	
	//! The indexing operator
	/*!
		If i is out of range, \\0 will be returned isntead.
	*/
	char operator[ ]( int i ) const;
	char & operator[ ]( int i );
	
private:
	char *str;
	//! The length of *str. This a *1* based count
	int len;
	pthread_mutex_t *l;
};

#endif

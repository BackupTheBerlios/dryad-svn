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
#ifndef DSARRAY_H
#define DSARRAY_H

#include "dstring.h"

struct dsastruct
{
	dstring *str;
	int count;
	//struct dsastruct *next;
	//struct dsastruct *prev;
};

/*struct pointer
{
	struct dsastruct *content;
	int index;
};

class dsarray{
public:
	dsarray();
	dsarray(int c);
	~dsarray();
	
	int length() const;
	
	void pushback( dstring *str );
	
	int iat(int i) const;
	dstring *sat(int i) const;
	
	int exists(dstring *str);
	
	int increment(int i);

private:
	struct dsastruct *head;
	struct dsastruct *tail;
	struct pointer **p;
	int pnum;
	int len;
	int space;
	struct dsastruct *getnode(int i);
};*/


//! A very simple array of dstring/int structs
/*!
	Not much to this class, really. Note that dsarray deals in pointers to dstrings and ints.
*/
class dsarray {
public:
	//! Empty constructor
	dsarray();
	//! Create an empty array of c elements
	/*!
		\param c number of elements to create
	*/
	dsarray(int c);
	//! Copy constructor
	/*!
		\param s the dsarray to copy from.
		Note: This does actually make copies, but just of the pointers, not the data pointed to.
	*/
	dsarray(dsarray *s);
	~dsarray();
	
	//! Gets the length
	/*!
		\return The length of the array
	*/
	int length() const;
	
	//! Resizes the array
	/*!
		\param c The new size
		If c is less than the old length, the old entries will be truncated, but, because they are just pointers, the actual dstring will still exist in memory. Make sure you free() it first.
		\sa pushback()
	*/
	void resize(int c);
	
	//! Tacks a new string on to the end of the array
	/*!
		\param s The new dstring
	*/
	void pushback(dstring *s);
	
	//! Returns the string at index i
	/*!
		\param i The index
		\return A pointer to the dstring at that position
	*/
	dstring *sat(int i);
	
	//! Returns the int at index i
	/*!
		\param i The index
		\return The int
	*/
	int iat(int i);
	
	//! Checks if a string exists in the array
	/*!
		\param s The string to check for
 		\return If it's found, it returns the index of the string, otherwise -1 is returned
		Note that this function is, in it's current incarnation incredibly inneficient.
	*/
	int exists(dstring *s);
	
	//! Increments the index at i
	/*!
		\param The location to increment
		\return the new value at that position, or FALSE if i is out of bounds
	*/
	int increment(int i);
	
	#ifdef DEBUG
	void dump();
	#endif

private:
	struct dsastruct **a;
	//! This is a 1 based index
	int num_items;
};

#endif
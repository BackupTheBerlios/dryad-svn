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
#ifndef DRARRAY_H
#define DRARRAY_H

//! The "real" array class
/*!
	Unlike darray, this array class uses a true array, rather than a linked list, to store it's elements.
*/
template <class T>
class drarray
{
public:
	//! Basic constructor
	/*!
		Inits the array to size zero
	*/
	drarray();
	//! Size based constructor
	/*!
		Inits the array to size `size`. The contents of the array are undefined.
	*/
	drarray(int size);
	~drarray();
	
	//! Returns the length of the array
	/*!
		\return The number of elements in the array.
	*/
	int length() const;
	
	//! Resizes the array
	/*!
		\param size The new size
		If the new size is larger than the old size, the new elements will have undefined contents. If the new size is smaller than the old, the last n|n = (old - new) elements will be truncated.
	*/
	void resize(int size);
	
	//! Addes a new element
	/*!
		\param item The item to add
		Because this class uses a true array, this method can be very memory intensize, as it has to duplicate the old array, and copy it's contents over to the new one.
	*/
	void pushback(T item);
	
	//! Checks for the existence of an element
	/*!
		\param item The item to compare all elements against
		\return True if a match is found, otherwise False
		For this method to work, T must a data type that has == defined. Also, be aware that if T is a pointer to some type, it will be comparing memory addresses, NOT actuall contents.
	*/
	int exists(T item);
	
	T at(int i);
	
	T & operator[ ]( int i );
	T operator[ ]( int i ) const;

private:
	int len;
	T *a;
};

#include "drarray.cpp"
#endif

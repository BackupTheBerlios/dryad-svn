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
#ifndef DARRAY_H
#define DARRAY_H

#include <stdlib.h>

template <class T>
struct darray_p {
	T item;
	struct darray_p *next;
	struct darray_p *prev;
};

//! A basic array class
/*!
	This "array" isn't really. It's a doubly linked list masquerading as an array. The point of this is that it makes for much quicker inserts and sorts. Access is slower though, as a result. Only use it if you need this behavior. Use drarray is you need a "real" array.
*/
template <class T>
class darray {
public:
	darray();
	~darray();
	
	//! Returns the autobalance status
	/*!
		\return int True or false
		If autobalance is set, all inserts will be done so that the array remains in a high->low order. Thus, you can not assume that a newly inserted item has the last index position. Autobalance defaults to false.
		\sa set_autobalance()
	*/
	int get_autobalance() const;
	//! Sets the autobalance status
	/*!
		\param b The new state of autobalancing
		For more info on autobalance see get_autobalance
		\sa get_autobalance()
	*/
	void set_autobalance(int b);
	
	//! Inserts into the array
	/*!
		\param item The item to insert
		\return The index of the new item
	*/
	int insert(T item);
	
	//! Get the contents at k
	/*!
		\param k The index
		\return The element at k
	*/
	T get_at(int k);
	
	//! Sets the contents at k
	/*!
		\param k the index
		\return The new index of the element.
		IF autobalance is true, upon changing the value of an item, the list will be resorted, changing the index of the item.
	*/
	int set_at(int k, T val);
	
	//! Gets the number of elements
	/*!
		\return the number of elements
	*/
	int length() const;
	
	//! Deletes the element at k
	/*!
		\param k The element to delete
		\return FALSE if k is out of range, otherwise TRUE
	*/
	int del(int k);
	
	#ifdef DEBUG
	void dump();
	#endif
	
private:
	int autobalance;
	int len;
	struct darray_p<T> *head;
	struct darray_p<T> *tail;
	// we'll prolly use more pointers later
};

#include "darray.cpp"
#endif

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
#ifndef DARRAY_CPP
#define DARRAY_CPP

#include "darray.h"

namespace DArray
{
template <class T>
darray<T>::darray()
{
	len = 0;
	autobalance = false;
	head = NULL;
	tail = NULL;
}

template <class T>
darray<T>::~darray()
{
	struct darray_p<T> *curr;
	for( int c = 0; c < len; c++ )
	{
		curr = head->next;
		free(head);
		head = curr;
	}
}

template <class T>
int darray<T>::get_autobalance() const
{
	return autobalance;
}

template <class T>
void darray<T>::set_autobalance(int b)
{
	if( b == 0 || b == 1 )
		autobalance = b;
}

template <class T>
int darray<T>::insert( T item )
{
	struct darray_p<T> *curr, *n;
	int c;
	if( head == NULL )
	{
		head = (struct darray_p<T>*)malloc(sizeof(struct darray_p<T>));
		tail = head;
		tail->next = NULL;
		tail->prev = NULL;
		tail->item = item;
		len++;
		return 0;
	}
	if( ! autobalance )
	{
		tail->next = (struct darray_p<T>*)malloc(sizeof(struct darray_p<T>));
		tail->next->prev = tail;
		tail->next->next = NULL;
		tail = tail->next;
		tail->item = item;
		len++;
		return len-1;
	}
	//this will need to get smartened up a bit
	curr = head;
	for( c = 0; c  < len; c++ )
	{
		if( curr->item < item )
			break;
		curr = curr->next;
	}
	n = (struct darray_p<T>*)malloc(sizeof(struct darray_p<T>));
	n->next = curr;
	n->prev = curr->prev;
	if( curr->prev != NULL )
		curr->prev->next = n;
	curr->prev = n;
	len++;
	return c;
}

template <class T>
T darray<T>::get_at(int k)
{
	struct darray_p<T> *curr;
	if( k < 0 || k >= len )
		return NULL;
	
	//again, this code will need to be smartened once we deal in more than just head/tail pointers
	for( int c = 0; c != k; c++ )
	{
		if( c == 0 )
			curr = head;
		else
			curr = curr->next;
	}
	return curr->item;
}

template <class T>
int darray<T>::set_at(int k, T val)
{
	struct darray_p<T> *curr;
	int t;
	if( k < 0 || k >= len )
		return -1;
	
	if( autobalance )
	{
		this->del(k);
		t = this->insert(val);
		//because insert will increment it, which is wrong.
		len--;
		free(curr);
		return t;
	}
		
	//again, this code will need to be smartened once we deal in more than just head/tail pointers
	for( int c = 0; c != k; c++ )
	{
		if( c == 0 )
			curr = head;
		else
			curr = curr->next;
	}
	curr->item = val;
	return k;
}

template <class T>
int darray<T>::length() const
{
	return len;
}

template <class T>
int darray<T>::del(int k)
{
	struct darray_p<T> *curr;
	
	if( k < 0 || k >= len )
		return false;
	
	//again, this code will need to be smartened once we deal in more than just head/tail pointers
	for( int c = 0; c != k; c++ )
	{
		if( c == 0 )
			curr = head;
		else
			curr = curr->next;
	}
	if( curr->prev != NULL )
		curr->prev->next = curr->next;
	if( curr->next != NULL )
		curr->next->prev = curr->prev;
	free(curr);
	return true;
}

#ifdef DEBUG
template <class T>
void darray<T>::dump()
{
	struct darray_p<T> *curr;
	curr = head;
	for( int c = 0; c < len; c++ )
	{
		cout << curr->item << endl;
		curr = curr->next;
	}
}
#endif
}
#endif

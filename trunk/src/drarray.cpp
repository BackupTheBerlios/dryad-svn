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
#ifndef DRARRY_CPP
#define DRARRY_CPP

#include "drarray.h"

template <class T>
drarray<T>::drarray()
{
	a = NULL;
	len = 0;
}

template <class T>
drarray<T>::drarray(int size)
{
	size > 0 ? len = size : size = 1;
	a = (T*)malloc(sizeof(T)*size);
}

template <class T>
drarray<T>::~drarray()
{
	free(a);
}

template <class T>
int drarray<T>::length() const
{
	return len;
}

template <class T>
void drarray<T>::resize(int size)
{
	T *n;
	if( !(size > 0) )
		size = 1;
	n = (T*)malloc(sizeof(T)*size);
	for( int c = 0; (c < len && c < size); c++ )
	{
		n[c] = a[c];
	}
	len = size;
	free(a);
	a = n;
}

template <class T>
void drarray<T>::pushback(T item)
{
	T *n;
	len++;
	n = (T*)malloc(sizeof(T)*len);
	for( int c = 0; c < (len-1); c++ )
	{
		n[c] = a[c];
	}
	n[len-1] = item;
	free(a);
	a = n;
}

template <class T>
int drarray<T>::exists(T item)
{
	for( int c = 0; c < len; c++ )
	{
		if( a[c] == item )
			return true;
	}
	return false;
}

template <class T>
T & drarray<T>::operator[ ]( int i )
{
	if( i > 0 && i < len )
		return a[i];
	else
		return NULL;
}

template <class T>
T drarray<T>::operator[ ]( int i ) const
{
	if( i > 0 && i < len )
		return a[i];
	else
		return NULL;
}

#endif

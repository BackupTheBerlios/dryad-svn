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
#ifndef DQUEUE_H
#define DQUEUE_H

#include <stdlib.h>
#include <pthread.h>

namespace DQueue
{
//! The basic struct type
template <class ItemType>
struct queue
{
	ItemType content;
	queue<ItemType> *next;
	queue<ItemType> *prev;
};

//! A basic fifo class
/*!
	Provides a basic first in, first out structure for any given data type.
*/
template <class T>
class dfifo
{
public:
	//! Basic constructor.
	/*!
		Creates an empty queue.
	*/
	dfifo();
	~dfifo();
	//! Add an item
	/*!
		\param item The item to add. It is copied into memory, however if you're using this to store pointers to ADTs, remember, it's the pointer getting stored.
	*/
	void enqueue( T item );
	//! Dequeue an item.
	/*!
		\return The first item in the queue, or null if there isn't one.
	*/
 	T dequeue();
	
	//! Returns the length of the queue
	/*!
		\return The number of items in the queue.
	*/
	int length() const;

private:
	queue<T> *head;
	queue<T> *tail;
	int qlen;
	pthread_mutex_t lock;
};
}
namespace DFifo = DQueue;

#include "dqueue.cpp"
#endif

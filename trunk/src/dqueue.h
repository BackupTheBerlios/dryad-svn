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

#include "cache.h"

#include <stdlib.h>
#include <pthread.h>

template <class ItemType>
struct queue
{
	ItemType content;
	queue<ItemType> *next;
	queue<ItemType> *prev;
};

template <class T>
class dfifo
{
public:
	dfifo();
	~dfifo();
	void enqueue( T item );
 	T dequeue();
	
	int length() const;

private:
	queue<T> *head;
	queue<T> *tail;
	int qlen;
	pthread_mutex_t lock;
};

#include "dqueue.cpp"
#endif

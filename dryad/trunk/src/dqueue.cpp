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
 
#ifndef DQUEUE_CPP
#define DQUEUE_CPP
#include "dqueue.h"

namespace DQueue
{
template <class T>
dfifo<T>::dfifo()
{
	head = NULL;
	tail = NULL;
	qlen = 0;
	pthread_mutex_init(&lock,NULL);
}

template <class T>
dfifo<T>::~dfifo()
{
	queue<T> tmp;
	while( head != NULL )
	{
		tmp = head->next;
		free(head);
		head = tmp;
	}
	pthread_mutex_destroy(&lock);
}

template <class T>
void dfifo<T>::enqueue(T item)
{
	pthread_mutex_lock(&lock);
	if( ! head )
	{
		head = (queue<T>*)malloc(sizeof(queue<T>));
		tail = head;
		tail->next = NULL;
		tail->prev = NULL;
		tail->content = item;
	}
	else
	{
		tail->next = (queue<T>*)malloc(sizeof(queue<T>));
		tail->next->prev = tail;
		tail->next->next = NULL;
		tail = tail->next;
		tail->content = item;
	}
	qlen++;
	pthread_mutex_unlock(&lock);
}

template <class T>
T dfifo<T>::dequeue()
{
	T ret;
	queue<T> *old;

	pthread_mutex_lock(&lock);
	if( ! head )
	{
		pthread_mutex_unlock(&lock);
		return NULL;
	}
	
	ret = head->content;
	old = head;
	head = head->next;
	head->prev = NULL;
	free(old);
	qlen--;
	pthread_mutex_unlock(&lock);
	return ret;
}

template <class T>
int dfifo<T>::length() const
{
	int ret;
	pthread_mutex_lock(&lock);
	ret = qlen;
	pthread_mutex_unlock(&lock);
	return ret;
}
}
#endif

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
#include "cache.h"

cache::cache(int s, dstring *fname)
{
	head = NULL;
	tail = NULL;
	s > 0 ? size = s : size = 32;
	pthread_mutex_init(&head_lock, NULL);
	pthread_mutex_init(&tail_lock, NULL);
	count = 0;
	
	if( fname != NULL )
		cache_file = new dstring((char*)fname->ascii());
	else
		cache_file = NULL;
}

cache::~cache()
{
	struct dstrlist *curr;
	while( head->next != NULL )
	{
		curr = head;
		head = head->next;
		free(curr);
	}
	free(head);
	pthread_mutex_destroy(&head_lock);
	pthread_mutex_destroy(&tail_lock);
}

int cache::get_size() const
{
	return size;
}

int cache::add(dstring *item)
{
	dfilestream *writer;
	int str_size;
	
	if( item == NULL )
		return -1;
	
	str_size = (sizeof(char) * item->length()) + sizeof(dstring);
	if( (count+str_size) <=  size )
	{
		pthread_mutex_lock(&tail_lock);
		if( head == NULL )
		{
			pthread_mutex_lock(&head_lock);
			head = (struct dstrlist*)malloc(sizeof(struct dstrlist));
			head->prev = NULL;
			head->next = NULL;
			head->item = new dstring((char*)(item->ascii()));
			tail = head;
			pthread_mutex_unlock(&head_lock);
		}
		else
		{
			tail->next = (struct dstrlist*)malloc(sizeof(struct dstrlist));
			tail->next->prev = tail;
			tail = tail->next;
			tail->next = NULL;
			tail->item = new dstring((char*)(item->ascii()));
		}
		count += str_size;
		pthread_mutex_unlock(&tail_lock);
	}
	else
	{
		writer = new dfilestream( cache_file, "a" );
		writer->writeline(item);
		delete writer;
	}
	// do stuff to cache it to disk. I'll prolly need to extend dfilestream...
}

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

namespace DCache
{
cache::cache(int s, dstring *fname)
{
	head = NULL;
	tail = NULL;
	file_cache = false;
	s > 0 ? size = s : size = 1024;
	
	head_lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	tail_lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	file_lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	get_wait = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
	pthread_mutex_init(head_lock, NULL);
	pthread_mutex_init(tail_lock, NULL);
	pthread_mutex_init(file_lock, NULL);
	pthread_cond_init(get_wait, NULL );
	count = 0;
	
	if( fname != NULL )
		cache_file = new dstring((char*)fname->ascii());
	else
		cache_file = NULL; // This is OK, because when we create the dfilestream, it will generate one.
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
	pthread_mutex_destroy(head_lock);
	pthread_mutex_destroy(tail_lock);
	pthread_mutex_destroy(file_lock);
	pthread_cond_destroy(get_wait);
	free(head_lock);
	free(tail_lock);
	free(file_lock);
	free(get_wait);
}

int cache::get_size() const
{
	return size;
}

int cache::add(struct syslog_message *m)
{
	dfilestream *writer;
	int msg_size;
	
	if( m == NULL )
		return -1;
	
	msg_size = (sizeof(struct syslog_message)) + (sizeof(dstring)*3) + (sizeof(int)*2) + (sizeof(char)*(m->date->length() + m->host->length() + m->message->length()));
	if( (count+msg_size) <=  size )
	{
		pthread_mutex_lock(tail_lock);
		if( head == NULL )
		{
			#ifdef DEBUG
			dryerr(1,"Creating a new head pointer in the cache.\n");
			#endif
			pthread_mutex_lock(head_lock);
			head = (struct dstrlist*)malloc(sizeof(struct dstrlist));
			head->prev = NULL;
			head->next = NULL;
			head->item = (struct syslog_message*)malloc(sizeof(struct syslog_message));
			head->item->date = m->date;
			head->item->facility = m->facility;
			head->item->host = m->host;
			head->item->message = m->message;
			head->item->severity = m->severity;
			tail = head;
			pthread_mutex_unlock(head_lock);
			pthread_cond_signal(get_wait);
		}
		else
		{
			#ifdef DEBUG
			dryerr(1,"Apending a new tail node in the cache.\n");
			#endif
			tail->next = (struct dstrlist*)malloc(sizeof(struct dstrlist));
			tail->next->prev = tail;
			tail = tail->next;
			tail->next = NULL;
			tail->item = (struct syslog_message*)malloc(sizeof(struct syslog_message));
			tail->item->date = m->date;
			tail->item->facility = m->facility;
			tail->item->host = m->host;
			tail->item->message = m->message;
			tail->item->severity = m->severity;
		}
		count += msg_size;
		#ifdef DEBUG
		dryerr(1,strcat(strcat(strcat(strcat("Current size of cache is ",count)," of "),size), endl));
		#endif
		pthread_mutex_unlock(tail_lock);
	}
	else
	{
		#ifdef DEBUG
		dryerr(1,"Outputting to a file, cache full.\n");
		#endif
		pthread_mutex_lock(file_lock);
		writer = new dfilestream( cache_file, "a" );
		// if cache_file is null, the first time we do this, a temp file name will be created. However, for get() and future file caching we need to know it's name!
		if( cache_file == NULL )
			cache_file = new dstring(writer->get_filename());
		writer->writeline(m->date);
		writer->writeline(m->facility);
		writer->writeline(m->host);
		writer->writeline(m->message);
		writer->writeline(m->severity);
		delete writer;
		file_cache = true;
		pthread_mutex_unlock(file_lock);
	}
}

struct syslog_message *cache::get()
{
	struct syslog_message *ret;
	struct dstrlist *curr;
	
	pthread_mutex_lock(head_lock);
	if( head == NULL )
	{
		pthread_cond_wait(get_wait, head_lock);
	}
	if( file_cache )
	{
		pthread_mutex_lock(tail_lock);
		pthread_mutex_lock(file_lock);
		this->load_disk_cache();
		pthread_mutex_unlock(tail_lock);
		pthread_mutex_unlock(file_lock);
	}
	ret = (struct syslog_message*)malloc(sizeof(struct syslog_message));
	ret = head->item;
	curr = head->next;
	if(curr != NULL)
		curr->prev = NULL;
	free(head);
	head = curr;
	if( head == NULL )
		tail = NULL;
	count -= (sizeof(struct syslog_message)) + (sizeof(dstring)*3) + (sizeof(int)*2) + (sizeof(char)*(ret->date->length() + ret->host->length() + ret->message->length()));
	pthread_mutex_unlock(head_lock);
	return ret;
}

void cache::load_disk_cache()
{
	dfilestream *reader, *writer;
	struct syslog_message *m;
	struct dstrlist *curr;
	dstring *t,*q;
	int msg_size;
	
	reader = new dfilestream(cache_file, "r");
	m = (struct syslog_message*)malloc(sizeof(struct syslog_message));
	
	//Yes, it is true that we overshoot the memory limit by one message. *shrug* not that big a deal.
	while( count < size )
	{
		if( NULL == (m->date = reader->readline()) )
		{
			free(m);
			break;
		}
		if( NULL == (t = reader->readline()) )
		{
			free(m);
			break;
		}
		m->facility = atoi(t->ascii());
		delete t;
		if( NULL == (m->host = reader->readline()) )
		{
			free(m);
			break;
		}
		if( NULL == (m->message = reader->readline()) )
		{
			free(m);
			break;
		}
		if( NULL == (t = reader->readline()) )
		{
			free(m);
			break;
		}
		m->severity = atoi(t->ascii());
		delete t;
		
		msg_size = (sizeof(struct syslog_message)) + (sizeof(dstring)*3) + (sizeof(int)*2) + (sizeof(char)*(m->date->length() + m->host->length() + m->message->length()));
		count += msg_size;
		
		curr = (struct dstrlist*)malloc(sizeof(struct dstrlist));
		curr->item = m;
		m = NULL;
		m = (struct syslog_message*)malloc(sizeof(struct syslog_message));
		curr->next = NULL;
		curr->prev = tail;
		curr->prev->next = curr;
		tail = curr;
		curr = NULL;
	}
	// now we copy the old cache file over to the new.
	writer = new dfilestream((char*)NULL,"a");
	file_cache = false;
	while( NULL != (t = reader->readline()) )
	{
		file_cache = true;
		writer->writeline(t);
		delete t;
	}
	t = new dstring(reader->get_filename());
	delete reader;
	q = new dstring(writer->get_filename());
	delete writer;
	rename( q->ascii(), t->ascii() );
}
}

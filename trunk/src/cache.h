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
#ifndef CACHE_H
#define CACHE_H

#include "dstring.h"
#include "dfilestream.h"
#include "syslog.h"

#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream.h>

//! A linked list struct, used in a fifo fashion, for caching syslog_messages
struct dstrlist {
	struct syslog_message *item;
	dstrlist *next;
	dstrlist *prev;
};

//! The caching class
/*!
	This class handles the storage of syslog_messages. It stores a given number of bytes in memory. Once that number is surpassed, it stores them to disk. Generally, a fairly low number of in mem storage is fine, say 8mb, as a syslog message is only going to be ~1kb.
*/
class cache {
public:
	//! Init the cache
	/*!
		\param s The number of bytes of to use as in memory storage
		\param fname The name of the cache file to use. Pass NULL to use a randomly created one.
	*/
	cache( int s, dstring *fname );
	~cache();
	
	//! Gets the max size of the cache
	/*!
		\return the size of the cache
	*/
	int get_size() const;
	
	//! Adds item to the cache
	/*!
		\param m The syslog_message to cache.
		\return True if it's cached to memory, False if it's sent to disk. It prolly won't return if you're out of disk space... :p
		A copy of m is stored in memory, however the internals of the syslog_message are NOT copied. DO NOT deallocate the dstrings inside the message itself.
	*/
	int add( struct syslog_message *m );
	
	//! Gets the next item awaiting processing.
	/*!
		\return A pointer to a syslog_message.
		This call will also move n items out of the on disk cache, if it exists. n being a number that brings the in memory cache to it's limit.
	*/
	struct syslog_message *get();

private:
	//! You MUST have locks on tail and file before calling this function!!!
	void load_disk_cache();
	int size;
	int count;
	int file_cache;
	struct dstrlist *head;
	struct dstrlist *tail;
	pthread_mutex_t head_lock, tail_lock, file_lock;
	pthread_cond_t get_wait;
	dstring *cache_file;
};
#endif

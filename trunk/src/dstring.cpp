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
#include "dstring.h"

dstring::dstring()
{
	len = 0;
	str = NULL;
	l = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(l, NULL);
}

dstring::dstring( int c )
{
	len = c;
	str = (char *)malloc( sizeof(char) * c + sizeof(char) );
	for( int a = 0; a <= len; a++ )
	{
		str[a] = '\0';
	}
	l = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(l, NULL);
}

dstring::dstring( dstring *s )
{
	len = s->length();
	str = (char *)malloc( sizeof(char) * len + sizeof(char) );
	for( int c = 0; c < len; c++ )
	{
		str[c] = s->str[c];
	}
	str[len] = '\0';
	l = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(l, NULL);
}

dstring::dstring( dstring &s )
{
	len = s.length();
	str = (char *)malloc( sizeof(char) * len + sizeof(char) );
	for( int c = 0; c < len; c++ )
	{
		str[c] = s.str[c];
	}
	str[len] = '\0';
	l = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(l, NULL);
}

dstring::dstring( char *s )
{
	len = strlen( s ); //because strlen does not could the null terminator
	str = (char *)malloc( sizeof(char) * len + sizeof(char) );
	for( int c = 0; c < len; c++ )
	{
		str[c] = s[c];
	}
	str[len] = '\0';
	l = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(l, NULL);
}

dstring::~dstring()
{
	if( str )
	{
		free(str);
		str = NULL;
	}
	pthread_mutex_destroy(l);
}

int dstring::length() const
{
	int ret;
	pthread_mutex_lock(l);
	ret = len;
	pthread_mutex_unlock(l);
	return len;
}

/*
A bit of explanation of what I'm doing with the remove() functions:
while( tmp does not point to a position in str where comparing r would take it too far )
{
	if( r is found in str, from the starting point of tmp )
	{
		for(the length of str)
		{
			if( we're below the section to be omitted )
				copy it in
			else if( we're about the section to be omitted)
				copy it in
			
		}
	}
	else
	{
		increment where tmp points to.
	}
}

If you have any questions about why this works, shoot me an email, or, better yet, take a systems programming course
*/

dstring *dstring::remove( dstring *r )
{
	char *tmp;
	dstring *n = new dstring();
	
	pthread_mutex_lock(l);
	
	if( (len - r->length()) <= 0 )
	{
		pthread_mutex_unlock(l);
		return n;
	}
	
	n->resize(len - r->length());
	
	tmp = str;
	while( tmp < (str + len - r->length() + 1) )
	{
		if( ! strncmp( tmp, r->str, r->length() ) ) // I know that ! seems a bit odd, but it's how strncmp works...
		{
			for( int c = 0; c < len; c++ )
			{
				if( & str[c] < tmp )
				{
					n->str[c] = str[c];
				}
				else if( &str[c] > (tmp + r->length() - 1) )
				{
					n->str[c - r->length()] = str[c];
				}
			}
		}
		// yes, it's nitpicky to use sizeof(char) rather than ++, but Tod made me :(
		tmp += sizeof(char);
	}
	pthread_mutex_unlock(l);
	return n;
}

dstring *dstring::remove( char *r )
{
	char *tmp;
	dstring *n = new dstring();
	int rlen = strlen(r);
	
	pthread_mutex_lock(l);
	
	if( (len - rlen) <= 0 )
	{
		pthread_mutex_unlock(l);
		return n;
	}
	
	n->resize(len - rlen);
	
	tmp = str;
	while( tmp < (str + len - rlen + 1) )
	{
		if( ! strncmp( tmp, r, rlen ) ) // I know that ! seems a bit odd, but it's how strncmp works...
		{
			for( int c = 0; c < len; c++ )
			{
				if( & str[c] < tmp )
				{
					n->str[c] = str[c];
				}
				else if( &str[c] > (tmp + rlen - 1) )
				{
					n->str[c - rlen] = str[c];
				}
			}
		}
		tmp++;
	}
	pthread_mutex_unlock(l);
	return n;
}

char *dstring::cstring()
{
	char *r;
	pthread_mutex_lock(l);
	r = (char *)malloc( sizeof(char) * len + sizeof(char) );
	for( int c = 0; c < len; c++ )
	{
		r[c] = str[c];
	}
	r[len] = '\0';
	pthread_mutex_unlock(l);
	return r;
}

const char *dstring::ascii()
{
	return (const char*)str;
}

void dstring::resize(int s)
{
	char *n;
	pthread_mutex_lock(l);
	n = (char *)malloc( sizeof(char) * s + sizeof(char) );
	for( int c = 0; c < s; c++ )
	{
		if( c < len )
			n[c] = str[c];
		else
			n[c] = '\0';
	}
	n[s] = '\0';
	if( str )
		free(str);
	str = n;
	len = s;
	pthread_mutex_unlock(l);
}

const dstring & dstring::operator = ( const dstring & s )
{
	pthread_mutex_lock(l);
	if( this != &s )
	{
		if( str )
			free(str);
		str = (char *)malloc( sizeof(char) * s.length() + sizeof(char) );
		len = s.length();
		for( int c = 0; c < len; c++ )
		{
			str[c] = s.str[c];
		}
	}
	pthread_mutex_unlock(l);
	return *this;
}

const dstring & dstring::operator = ( const dstring * s )
{
	pthread_mutex_lock(l);
	if( this != s )
	{
		if( str )
			free(str);
		str = (char *)malloc( sizeof(char) * s->length() + sizeof(char) );
		len = s->length();
		for( int c = 0; c < len; c++ )
		{
			str[c] = s->str[c];
		}
	}
	pthread_mutex_unlock(l);
	return *this;
}

const dstring & dstring::operator = ( const char * s )
{
	pthread_mutex_lock(l);
	if( s != str )
	{
		if( str )
			free(str);
		str = (char *)malloc( sizeof(char) * strlen(s) + sizeof(char) );
		len = strlen(s);
		for( int c = 0; c < len; c++ )
		{
			str[c] = s[c];
		}
	}
	pthread_mutex_unlock(l);
	return *this;
}

char dstring::operator[ ]( int i ) const
{
	char ret;
	pthread_mutex_lock(l);
	if( i < len || i < 0 )
	{
		ret = str[i];
		pthread_mutex_unlock(l);
		return ret;
	}
	else
	{
		pthread_mutex_unlock(l);
		return '\0';
	}
}

char & dstring::operator[ ]( int i )
{
	char ret, r = '\0';
	pthread_mutex_lock(l);
	if( i < len || i < 0 )
	{
		ret = str[i];
		pthread_mutex_unlock(l);
		return ret;
	}
	else
	{
		pthread_mutex_unlock(l);
		return r;
	}
}

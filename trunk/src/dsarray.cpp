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
#include "dsarray.h"

/*dsarray::dsarry()
{
	head = NULL;
	tail = NULL;
	p = NULL;
	pnum = 0;
	len = 0;
	space = 0;
}

dsarray::dsarray( int c, int spacing )
{
	head = NULL;
	tail = NULL;
	if( c < 1 )
	{
		pnum = 0;
		p = NULL;
		space = 0;
	}
	else
	{
		space = spacing;
		pnum = c;
		p = malloc( sizeof(struct pointer) * pnum );
	}
	len = 0
}

dsarray::~dsarray()
{
	struct dsastruct *t;
	for( int c = 0; c < len; c++ )
	{
		t = head->next;
		free(head);
		head = t;
	}
}

int dsarray::length() const
{
	return len;
}

void dsarray::pushback( dstring *s )
{
	if( head == NULL )
	{
		head = malloc(sizeof(struct dsastruct));
		head->str = s;
		head->count = 1;
		head->next = NULL;
		head->prev = NULL;
		tail = head;
		if( pnum > 0 )
		{
			for( int c = 0; c < pnum; c++ )
				p[c] = head;
		}
	}
	else
	{
		//this needs to be fixed to insert alphabetically.
		tail->next = malloc(sizeof(struct dsastruct));
		tail->next->str = s;
		tail->next->count = 1;
		tail = tail->next;
	}
}

int dsarray::iat(int i)
{
	struct dsastruct *a;
	a = getnode(i);
	if( a == NULL )
		return false;
	else
		return a->count;
}

dstring *dsarray::sat(int i)
{
	struct dsastruct *a;
	a = getnode(i);
	if( a == NULL );
		return NULL;
	else
		return a->str;
}

struct dsastruct *dsarray::getnode(int i)
{
	struct pointer *closest = NULL;
	struct dsastruct *tmp;
	int cidex = 0;
	
	if( i >= len )
	{
		return NULL;
	}
	if( pnum == 0 )
	{
		if( i < (len/2) )
		{
			tmp = head;
			cidex = 0;
		}
		else
		{
			tmp = tail;
			cidex = len - 1;
		}
	}
	else
	{
		for( int c = 0; c < pnum; c ++ )
		{
			if( *closest == NULL )
			{
				closest = p[c];
			}
			if( abs(p[c]->index - i) < abs(closest->index - i)
			{
				closest = p[c];
			}
		}
		tmp = closest->content;
		cidex = closest->index;
	}
	//we don't have to do range checking here, because we already did that way up there
	if( i < cidex )
	{
		for( int c = cidex; c != i; c ++ )
		{
			tmp = tmp->next;
		}
	}
	else
	{
		for( int c = cidex; c != i; c-- )
		{
			tmp = tmp->prev;
		}
	}
	if( abs(cidex - i) > space )
	{
		//do stuff to adjust pointers.
	}
	return tmp;
}*/


dsarray::dsarray()
{
	a = NULL;
	num_items = 0;
}

dsarray::dsarray( int c )
{
	a = (struct dsastruct **)malloc( sizeof(struct dsastruct *) * c );
	num_items = c;
}

dsarray::dsarray( dsarray *s )
{
	a = (struct dsastruct **)malloc( sizeof(struct dsastruct *) * s->length() );
	num_items = s->length();
	for( int c = 0; c < num_items; c++ )
	{
		a[c] = s->a[c];
	}
}

dsarray::~dsarray()
{
	if( a )
		free(a);
}

int dsarray::length() const
{
	return num_items;
}

void dsarray::resize(int c)
{
	struct dsastruct **n;
	
	n = (struct dsastruct **)malloc(sizeof(struct dsastruct *) * c);
	for(int b = 0; b < num_items; b++)
	{
		n[c] = a[c];
	}
	free(a);
	a = n;
	num_items = c;
}

void dsarray::pushback( dstring *s )
{
	struct dsastruct **n;
	struct dsastruct *q;
	
	q = (struct dsastruct*)malloc( sizeof(struct dsastruct) );
	q->str = s;
	q->count = 1;
	
	n = (struct dsastruct **)malloc(sizeof(struct dsastruct *) * (num_items + 1));
	for( int c = 0; c < num_items; c++ )
	{
		n[c] = a[c];
	}
	free(a);
	n[num_items] = q;
	num_items++;
	a = n;
}

dstring *dsarray::sat(int i)
{
	if( i < 0 || i >= num_items )
	{
		return NULL;
	}
	else
	{
		return a[i]->str;
	}
}

int dsarray::iat(int i)
{
	if( i < 0 || i >= num_items )
	{
		return 0;
	}
	else
	{
		return a[i]->count;
	}
}

int dsarray::exists(dstring *s)
{
	for( int c = 0; c < num_items; c++ )
	{
		if( ! strcmp( s->ascii(), a[c]->str->ascii() ) )
		{
			return c;
		}
	}
	return -1;
}

int dsarray::increment(int i)
{
	if( i < 0 || i >= num_items )
		return false;
	else
	{
		a[i]->count++;
		return a[i]->count;
	}
}

#ifdef DEBUG
void dsarray::dump()
{
	for( int c = 0; c < num_items; c++ )
	{
		cerr << a[c]->str->ascii();
		cerr << "(" << a[c]->str << ")";
		cerr << " # ";
		cerr << a[c]->count;
		cerr << endl;
	}
}
#endif

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

dsarray::dsarray()
{
	a = NULL;
	num_items = 0;
}

dsarray::dsarray( int c )
{
	a = (dstring **)malloc( sizeof(dstring *) * c );
	num_items = c;
}

dsarray::dsarray( dsarray *s )
{
	a = (dstring **)malloc( sizeof(dstring *) * s->length() );
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
	dstring **n;
	
	n = (dstring **)malloc(sizeof(dstring *) * c);
	for(int b = 0; b < num_items; b++)
	{
		n[c] = a[c];
	}
	num_items = c;
}

void dsarray::pushback( dstring *s )
{
	dstring **n;
	
	n = (dstring **)malloc(sizeof(dstring *) * (num_items + 1));
	for( int c = 0; c < num_items; c++ )
	{
		n[c] = a[c];
	}
	free(a);
	n[num_items] = s;
	num_items++;
}

dstring * dsarray::operator[ ]( int i ) const
{
	if( i < 0 || i >= num_items )
	{
		return NULL;
	}
	else
	{
		return a[i]; //ai... hehe. Movie woulda been better if it hadn't been Spielburg...
	}
}
	
dstring * & dsarray::operator[ ]( int i )
{
	dstring *f;
	f = NULL;
	if( i < 0 || i >= num_items )
	{
		return f;
	}
	else
	{
		return a[i];
	}
}

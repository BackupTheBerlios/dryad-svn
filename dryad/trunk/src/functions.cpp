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
#include "functions.h"
namespace DFunctions
{
char *itoa(int i)
{
	/* here's what we do:
		1. find out how much space we need by counting how many times we can divide by 10
		2. allocate that space
		3. fill it right to left with number gained via modulo, reduce the source int by an order of magnitude, start 3 again, stop when the source int is 0
  */
	int temp,x;
	char *a;
	int neg;

	neg = 0;
	if( i < 0 )
	{
		neg = 1;
		i = abs(i);
	}

	temp = i;
	x = 1;
	while( temp > 9 )
	{
		temp = (int)(temp / 10);
		x++;
	}
	x += neg;
	x++; /*to allow for \0*/
	a = (char*)malloc( x * sizeof(char) );
	x--; /* so the spot for \0 doesn't get overwritten */
	a[x] = '\0';

	temp = i;
	x--;
	while( x >= (0+neg) )
	{
		a[x--] = (temp % 10) + '0';
		temp = (int)(temp/10);
	}
	if(neg)
	{
		a[0] = '-';
	}

	return a;

}
}

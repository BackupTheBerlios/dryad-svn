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
#ifndef DSARRAY_H
#define DSARRAY_H

//! A very simple array class for dstrings
/*!
	Not much to this class, really. Note that dsarray deals in pointers to dstrings.
*/
class dsarray {
public:
	//! Empty constructor
	dsarray();
	//! Create an empty array of c elements
	/*!
		\param c number of elements to create
	*/
	dsarray(int c);
	//! Copy constructor
	/*!
		\param s the dsarray to copy from.
		Note: This does actually make copies, but just of the pointers, not the data pointed to.
	*/
	dsarray(dsarray *s);
	~dsarray();
	
	

private:
	dstring **a;
	int num_item;
}

#endif

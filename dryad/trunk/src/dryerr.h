/***************************************************************************
 *   Copyright (C) 2004 by Peter Snoblin, Tod Hansmann                     *
 *   pas@truman.edu   tod@todandlorna.com                                  *
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

#ifndef _DRYERR_H
#define _DRYERR_H

#include <iostream>

/*  This file is the error handler for Dryad.
//  The idea is that you call this function and pass it an
//  error number.  The Error number gets passed to a switch,
//  and if an error gets processed, the function returns 1.
//  Otherwise, it's just a general error and the function
//  returns 0.  This may be of no use to anyone at this time
//  but I like to give flexibility where available.  The case
//  statement will call a script from the scripting file
//  when we've written the modules to handle them.  Then the
//  error handling can be customized.  Until then, we'll just
//  handle the error in a default way, and then we'll keep that
//  default unless a seperate script has been written.
//  Feedback to: tod@rtlad.org  */

/* Recent changes:
  Changed the include to iostream
  added the DError namespace
  split into dryerr.h and dryerr.cpp
  -- peter 5/15/2004
*/

namespace DError
{
using std::cout;
using std::cerr;
using std::endl;

//! Document me
/*!
	\param errnum Document me
	\return Document me
	Include any other notes here
*/
int dryerr(int errnum, char *errstring);

}

#endif


/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Tod, include your copyright notice here! I don't want to make any assumptions...
but you'll need to pick GPL, LGPL, MIT/X, or BSD
Also, make sure you put it in dryerr.cpp as well.

*/
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
int dryerr(int errnum);

}

#endif


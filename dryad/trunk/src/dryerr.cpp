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

/*   Currently, we need to classify errors into types so that actions can
	be taken on errors in a general sense.  I.e. you might want to take the
	same action on all errors resolving symbols, but you wouldn't want to
	necessarily output the same error message.  Thus we've changed to adding
	a string literal.  We just haven't organized error processes yet, and I
	don't think it's high on the list of priorities until we determine what
	to DO with the errors, let alone how they should be divided up.  This
	plan is not set in stone, and may change soon.
*/

#include "dryerr.h"

namespace DError
{

int dryerr(int errnum, int num_params, ...)
{
	va_list ap;
	char *t;

	int diderr = 0;  // Used to determine if an error is processed
	
	va_start(ap, num_params);
	
	switch (errnum){
	case 1:
		cerr << "ERROR: ";
		for( int c = 0; c < num_params; c++ )
		{
			cerr << va_arg(ap, char*);
		}
		diderr = 1;
		break;
	case 2:
		cerr << "ERROR: ";
		for( int c = 0; c < num_params; c++ )
		{
			cerr << va_arg(ap, char*);
		}
		diderr = 1;
		break;
	case 3:
		cout << "Error: 3\n";
		diderr = 1;
		break;
	case 4:
		cout << "Error: 4\n";
		diderr = 1;
		break;
	case 5:
		cout << "Error: 5\n";
		diderr = 1;
		break;
	case 6:
		cout << "Error: 6\n";
		diderr = 1;
		break;
	case 7:
		cout << "Error: 7\n";
		diderr = 1;
		break;
	case 8:
		cout << "Error: 8\n";
		diderr = 1;
		break;
	case 9:
		cout << "Error: 9\n";
		diderr = 1;
		break;
	case 10:
		cout << "Error: 10\n";
		diderr = 1;
		break;
	case 11:
		cout << "Error: 11\n";
		diderr = 1;
		break;
	case 12:
		cout << "Error: 12\n";
		diderr = 1;
		break;
	case 13:
		cout << "Error: 13\n";
		diderr = 1;
		break;
	case 14:
		cout << "Error: 14\n";
		diderr = 1;
		break;
	case 15:
		cout << "Error: 15\n";
		diderr = 1;
		break;
	default:
		cout << "UNKNOWN ERROR\n";
	}
	va_end(ap);
	return diderr;
}

}

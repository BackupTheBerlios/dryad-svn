#ifndef _DRYERR_H
#define _DRYERR_H

#include <iostream.h>

/*	This file is the error handler for Dryad.
//	The idea is that you call this function and pass it an
//	error number.  The Error number gets passed to a switch,
//	and if an error gets processed, the function returns 1.
//  Otherwise, it's just a general error and the function
//	returns 0.  This may be of no use to anyone at this time
//	but I like to give flexibility where available.  The case
//	statement will call a script from the scripting file
//	when we've written the modules to handle them.  Then the
//	error handling can be customized.  Until then, we'll just
//	handle the error in a default way, and then we'll keep that
//	default unless a seperate script has been written.
//	Feedback to: tod@rtlad.org  */


int dryerr(int errnum)
{

	int diderr = 0;  // Used to determine if an error is processed
	
	switch (errnum){
	case 1:
		cout << "Error: 1\n";
		diderr = 1;
		break;
	case 2:
		cout << "Error: 2\n";
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

	return diderr;
}

#endif
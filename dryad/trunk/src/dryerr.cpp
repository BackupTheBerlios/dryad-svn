/*
!!!!!!!!!!!!!
Again, Tod, put in a copyright notice
*/

#include "dryerr.h"

namespace DError
{

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

}
#include "dstring.h"
#include <string.h>
//----------------------

using DString::dstring;
int main()
{
	dstring *test, *test2;
	
	test = new dstring;
	delete test;
	test = new dstring("Hello World");
	if( 0 != strcmp(test->ascii(), "Hello World") )
	{
		exit(1);
	}
	test2 = new dstring(test);
	if( 0 != strcmp(test2->ascii(), "Hello World") )
	{
		exit(1);
	}
	delete test2;
	delete test;
	test = new dstring(4);
	if( 0 != strcmp(test2, "    ") )
	{
		exit(1);
	}
	if( test->length() != 4 )
	{
		exit(1);
	}
	delete test;
	
}


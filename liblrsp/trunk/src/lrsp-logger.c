/***************************************************************************
 *   Copyright (C) 2004 by Peter Snoblin                                   *
 *   pas@truman.edu                                                        *
 *                                                                         *
 *   Permission is hereby granted, free of charge, to any person obtaining *
 *   a copy of this software and associated documentation files (the       *
 *   "Software"), to deal in the Software without restriction, including   *
 *   without limitation the rights to use, copy, modify, merge, publish,   *
 *   distribute, sublicense, and/or sell copies of the Software, and to    *
 *   permit persons to whom the Software is furnished to do so, subject to *
 *   the following conditions:                                             *
 *                                                                         *
 *   The above copyright notice and this permission notice shall be        *
 *   included in all copies or substantial portions of the Software.       *
 *                                                                         *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       *
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    *
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR     *
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR *
 *   OTHER DEALINGS IN THE SOFTWARE.                                       *
 ***************************************************************************/
#include "lrsp.h"

#include <stdlib.h>
#include <stdio.h>
/* This is a small sample program to show off how to use liblrsp */

void usage(char *);
void call( int err, char *msg );

int main(int argc, char **argv)
{
	int err;
	if( argc != 4 )
	{
		usage(argv[0]);
		exit(1);
	}
	if( 0 > (err = lrsp_client_init(argv[1], atoi(argv[2]), LRSP_SINGLE)) )
	{
		fprintf(stderr, "Got an error when trying to init the connection:\n%s", lrsp_client_error_message(err) );
		exit(1);
	}
	lrsp_client_register_callback(call);
	if( 0 > (err = lrsp_client_send_message(argv[3])) )
	{
		fprintf(stderr, "Got an error when trying to send the message:\n%s", lrsp_client_error_message(err) );
		exit(1);
	}
	lrsp_client_free();
	return EXIT_SUCCESS;
}

void usage(char *p)
{
	printf("%s server port message\n\tserver -- The ip address of the server to send the log message.\n\tport -- The port on server where the log daemon listens\n\tmessage -- The message to send.\n", p);
}

void call( int err, char *msg )
{
	fprintf( stderr, "Got an error when trying to send message: %s\n%s", msg, lrsp_client_error_message(err) );
}

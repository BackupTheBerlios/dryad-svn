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
#ifndef _LRSP_H
#define _LRSP_H

#define LRSP_PORT 1234
#define LRSP_PERSISTANT 'P'
#define LRSP_SINGLE 'S'

#ifdef __cplusplus
extern "C" {
#endif

/*
	This function sets up needed items for the library to work.
	Parameters:
	server -- The IP address of the server to contact
	port -- the port to use. If port is 0 LRSP_PORT is used
	mode -- The mode of the connection. For forward compatibility, please use LRSP_PERSISTANT and LRSP_SINGLE here
*/
int lrsp_init(char *server, int port, char mode);

/*
	Frees the resources used by the library.
*/
int lrsp_free();

/*
	This will send the message to syslog server.
*/
int lrsp_send_message(char *msg);

char *lrsp_error_message(int err);

#ifdef __cplusplus
}
#endif

#endif

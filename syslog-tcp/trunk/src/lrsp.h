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

/*! \defgroup liblrsp
@{*/

/*! \brief The default port that liblrsp will attempt to connect to
 You should not change this if you are using this header as a client library.
*/
#define LRSP_PORT 1234
/*! \brief Used to specify that the connection should be created in persistant mode.
 It is strongly recommended that you use the define rather than the value for compatibility with future revisions.
*/
#define LRSP_PERSISTANT 'P'
/*! \brief Used to specify that the connection should be created in single mode.
 It is strongly recommended that you use the define rather than the value for compatibility with future revisions.
*/
#define LRSP_SINGLE 'S'


#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Use this specification for callback functions

*/
typedef void (*lrsp_client_callback)(int err, char *msg);


/*! \brief Inits the library resources
	\param server the ip address of the log server
	\param port the port to connect to on the log server, if this is zero LRSP_PORT is used.
	\param mode the mode of the connection, either LRSP_PERSISTANT or LRSP_SINGLE
	\sa LRSP_PORT LRSP_PERSISTANT LRSP_SINGLE lrsp_client_error_message
	\return On success the return value will be greater than 0, on failure it will be less. Use lrsp_error_message to get a human readable form of the message.
*/
int lrsp_client_init(char *server, int port, char mode);

/*! \brief Frees the resources used by the library.
	\return True.
*/
int lrsp_client_free();

/*! \brief Sends the message to the server
	\param msg The message to send
	\return A value greater than 0 if succesful, 0 if pthreads are enabled, and a value less than zero if there is an error.
	\sa lrsp_client_error_message lrsp_client_register_callback
	If pthreads are enabled, you will need to register a callback function if you want to have any sort of error handling.
*/
int lrsp_client_send_message(char *msg);

/*! \brief Register a function to be called if there is an error in sending the message
	\param c The callback function
	The callback function will only be used if you have pthreads enabled.
*/
void lrsp_client_register_callback(lrsp_client_callback c);

/*! \brief Returns a human readable version of the error code.
	\param err The error code
	\return A human readable description of the error.
*/
char *lrsp_client_error_message(int err);

// server functions begin here.

/*! \brief Use this spec for the callback that will handle messages from the devices
	Note that once your function has msg, it's in your court. It was malloc'd and it's your responsibility to free() it.
*/
typedef void (*lrsp_server_callback)(char *msg);
/*! \brief Use this spec for the error handling callback
	\sa lrsp_server_error_message
*/
typedef void (*lrsp_server_error_callback)(int err);

/*! \brief Allocates library resources
	\param port The port to bind to. If it is null, LRSP_PORT is used.
	\return Greater than 0 on success, less on failure.
	\sa LRSP_PORT lrsp_server_error_message.
*/
int lrsp_server_init(int port);

/*! \brief Deallocated library resources
	
*/
int lrsp_server_free();

/*! \brief Registers callback functions.
	\param c This function is called to process incoming messages. If it is null, the connecting clients are told that server does not support the mode they attempted to use.
	\param e The callback to use for errors if pthreads are enabled. It if is null, the error is just discarded.
*/
void lrsp_server_register_callback(lrsp_server_callback c, lrsp_server_error_callback e);

/*! \brief Starts the server doing it's thing.
	IF the server was built with pthreads, you can stop the server by calling lrsp_server_free.
	\sa lrsp_server_free
*/
int lrsp_server_listen();

/*! \brief Returns a human readable version of the error code.
	\param err The error code
	\return A human readable description of the error.
*/
char *lrsp_server_error_message(int err);

#ifdef __cplusplus
}
#endif
/*!@}*/

#endif

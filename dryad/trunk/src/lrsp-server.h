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
#ifndef LRSP_SERVER_H
#define LRSP_SERVER_H

#include "cache.h"
#include "dstring.h"
#include "syslog.h"
#include "dryerr.h"
#include "functions.h"

#include <lrsp.h>
#include <iostream>

#define RFC3164_PACKET_LENGTH 1024
#define RFC3164_DATE_LEN 15

namespace DLRSP
{
using std::cerr;
using std::endl;
using DCache::cache;
using Syslog::syslog_message;
using DString::dstring;
using DError::dryerr;
using DFunctions::itoa;

class lrsp {
public:
	lrsp(cache *s);
	lrsp(int port, cache *s);
	~lrsp();
	
	void listen();
	
	void error_callback(int err);
	void message_callback(char *msg);
private:
	//! Used to parse the message.
	struct syslog_message *parse_message(char *message);
	cache *c;

};

struct lrsp_args {
	lrsp *obj;
};

struct lrsp_args *lrsp_setup(int port, cache *c);

void err_cb(int err);

void msg_cb(char *msg);

void *lrsp_launch_thread(void *arg);

void lrsp_stop();


}

#endif

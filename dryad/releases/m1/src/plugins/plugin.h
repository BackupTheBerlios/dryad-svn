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
#ifndef PLUGIN_H
#define PLUGIN_H

#include "../drarray.h"
#include "../dstring.h"
#include "../functions.h"
#include "../conf.h"

namespace plugin
{
using DRArray::drarray;
using DString::dstring;
using std::cerr;
using DFunctions::itoa;
using DConf::conf;

//! This is how it comes at ya.
struct syslog_message {
	int facility;
	int severity;
	dstring *date;
	dstring *host;
	dstring *message;
	dstring *daemon;
};
}
#endif

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
#include <Python.h>
#include <plugin.h>
namespace python_plugin
{
using namespace plugin;
extern "C"
{
	dstring *script;
	int dryad_config(conf *c)
	{
		drarray<dstring*> *funcs;
		
		funcs = new drarray<dstring*>;
		
		script = c->daemon_get("libpython_module", "script");
		if( script == NULL )
		{
			cerr << "No script specified for libpython!\nAborting!\n";
			exit(1);
		}
		
		if( NULL == c->daemon_get("libpython_module", "map", funcs) )
		{
			cerr << "No daemon->python function mappings defined!\nAborting!\n";
			exit(1);
		}
		
	}
	
	int dryad_once(struct syslog_message *m)
	{
	
	}
	
	int dryad_many(drarray<struct syslog_message*> *m)
	{
	
	}

}
}

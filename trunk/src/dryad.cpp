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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "database.h"
#include "analyze.h"
#include "dstring.h"
#include "conf.h"
#include "dqueue.h"
#include "network.h"
#include <pthread.h>


#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char *argv[])
{
	conf *cnf;
	dstring *cfile;
	analyze *core;
	database *loader;
	network *net;
	
	if( argc > 1 )
	{
		//for now all we get from the command line is the config file, this may change
		cfile = new dstring(argv[1]);
	}
	else
	{
		cfile = new dstring("/home/peter/tmp/config.test");
	}
	cnf = new conf(cfile);
	
	core = new analyze(cnf);
	for( int c = 0; c < cnf->num_dbs(); c++ )
	{
		loader = new database(cnf->db(c), cnf->db_level(c));
		core->load(loader);
		loader = NULL;
	}
	
	net = new network(5674, NULL, 5);
	net->start_listening();
	
  return EXIT_SUCCESS;
}

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
#include "cache.h"
#include <pthread.h>


#include <iostream>
#include <cstdlib>

using namespace std;

struct cmdlineargs
{
	dstring *conffile;
	int port;
	int tcp;
	int udp;
};

struct cmdlineargs *parse_commandline(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	conf *cnf;
	dstring *cfile;
	analyze *core;
	database *loader;
	network *net;
	cache *cash;
	struct cmdlineargs *args;
	
	args = parse_commandline(argc, argv);
	
	if( args->conffile == NULL )
		args->conffile = new dstring("/home/peter/tmp/config.test");
	
	cnf = new conf(args->conffile);
	
	core = new analyze(cnf);
	for( int c = 0; c < cnf->num_dbs(); c++ )
	{
		loader = new database(cnf->db(c), cnf->db_level(c));
		core->load(loader);
		loader = NULL;
	}
	
	cash = new cache( cnf->get_cache_size(), cnf->get_cache_file() );
	
	net = new network( cnf->get_port(), cash, 5);
	net->start_listening();
	
  return EXIT_SUCCESS;
}

struct cmdlineargs *parse_commandline(int argc, char *argv[])
{
	struct cmdlineargs *ret;
	
	ret = (struct cmdlineargs*)malloc(sizeof(struct cmdlineargs));
	ret->conffile = NULL;
	ret->port = 0;
	ret->tcp = 0;
	ret->udp = 0;
	if( argc == 1 )
	{
		return ret;
	}
	for( int c = 1; c < argc; c++ )
	{
		if( ! strcmp(argv[c], "-c") )
		{
			if( (c+1) < argc && argv[c+1][0] != '-' )
				ret->conffile = new dstring(argv[++c]);
		}
		if( ! strcmp(argv[c], "-p") )
		{
			if( (c+1) < argc && argv[c+1][0] != '-' )
			{
				ret->port = atoi(argv[++c]);
				if( ret->port < 0 )
					ret->port = 0;
			}
		}
		if( ! strcmp(argv[c], "-t") )
		{
			if( (c+1) < argc && argv[c+1][0] != '-' )
			{
				ret->tcp = atoi(argv[++c]);
				if( ret->tcp != 0 && ret->tcp != 1 )
					ret->tcp = 0;
			}
		}
		if( ! strcmp(argv[c], "-u") )
		{
			if( (c+1) < argc && argv[c+1][0] != '-' )
			{
				ret->udp = atoi(argv[++c]);
				if( ret->udp != 0 && ret->udp != 1 )
					ret->udp = 0;
			}
		}
	}
	return ret;
}

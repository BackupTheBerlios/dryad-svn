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
#include "rfc3164.h"
#include "cache.h"
#include "lrsp-server.h"

#include <pthread.h>
#include <iostream>

using namespace std;
using DConf::conf;
using DCache::cache;
using DString::dstring;

struct cmdlineargs
{
	dstring *conffile;
	int tcp;
	int udp;
	int sekret;
};

struct cmdlineargs *parse_commandline(int argc, char *argv[]);
void usage(char **argv);


//! Welcome to the dryad documentation!
/*!
	I suppose the most important thing to document here are the possible command line parameters:\n
	-u [port] -- specifies that dryad should run an RFC3164 compliant udp syslog server, if port is not specified, it will bind the default, 514
	-t [port] -- specifies that dryad should run an RFC3195 compliant tcp syslog server.
	-c /path/to/config/file -- specifies that dryad should get it's config information from that file, rather than /etc/dryad/dryad.conf
*/
int main(int argc, char *argv[])
{
	conf *cnf;
	dstring *csize;
	int cs;
	cache *cash;
	struct cmdlineargs *args;
	pthread_t *udp_t, *tcp_t, *core;
	
	udp_t = NULL;
	tcp_t = NULL;
	core = NULL;
	
	args = parse_commandline(argc, argv);
	
	if( args->conffile == NULL )
		args->conffile = new dstring("/etc/dryad/dryad.conf");
	
	cnf = new conf(args->conffile);
	
	csize = cnf->get("cache_size");
	if( csize == NULL )
		cs = 8388608; // 8MB default
	else
		cs = atoi(csize->ascii());
	if( cs < 512 )
		cs = 8388608; // 8MB default
	
	cash = new cache( cs, cnf->get("cache_file") );
	
	if( args->udp > -1 )
	{
		udp_t = (pthread_t*)malloc(sizeof(pthread_t));
		pthread_create(udp_t, NULL, RFC3164::rfc3164_launch_thread, RFC3164::rfc3164_build_args(cash, args->udp) );
	}
	if( args->tcp > -1 )
	{
		tcp_t = (pthread_t*)malloc(sizeof(pthread_t));
		pthread_create(tcp_t, NULL, DLRSP::lrsp_launch_thread, DLRSP::lrsp_setup(args->tcp, cash) );
	}
	if( args->tcp == -1 && args->udp == -1 )
	{
		cerr << "You really should specify tcp and/or udp mode, if you actually want me to do anything.\n";
		exit(1);
	}
	core = (pthread_t*)malloc(sizeof(pthread_t));
	pthread_create(core, NULL, DAnalyze::analyze_launch_thread, DAnalyze::analyze_build_args(cnf, cash));
	if( args->sekret == 1 )
	{
		// I am well aware that there is a race condition here, if there are invalid config options or whatnot. But whatevar. Enough of the error will more than likely get outputted to bork the tests, and if it doesn't, it'll get caught in the next stage when we don't use the sekret flag
		cerr << "tcp: " << args->tcp << "--udp: " << args->udp << endl;
		exit(0);
	}
	if( udp_t != NULL )
		pthread_join( *udp_t, NULL );
	if( tcp_t != NULL )
		pthread_join( *tcp_t, NULL );
	
	pthread_join( *core, NULL );
	
  return EXIT_SUCCESS;
}

struct cmdlineargs *parse_commandline(int argc, char *argv[])
{
	struct cmdlineargs *ret;
	
	ret = (struct cmdlineargs*)malloc(sizeof(struct cmdlineargs));
	ret->conffile = NULL;
	ret->tcp = -1;
	ret->udp = -1;
	ret->sekret = 0;
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
		if( ! strcmp(argv[c], "-t") )
		{
			ret->tcp = 0;
			if( (c+1) < argc && argv[c+1][0] != '-' )
			{
				ret->tcp = atoi(argv[++c]);
				if( ret->tcp < 0 )
					ret->tcp = 0;
			}
		}
		if( ! strcmp(argv[c], "-u") )
		{
			ret->udp = 0;
			if( (c+1) < argc && argv[c+1][0] != '-' )
			{
				ret->udp = atoi(argv[++c]);
				if( ret->udp < 0 )
					ret->udp = 0;
			}
		}
		if( ! strcmp(argv[c], "-h") )
		{
			usage(argv);
			exit(0);
		}
		if( ! strcmp(argv[c], "-s") ) // s for sekret! :p
		{
			ret->sekret = 1;
		}
	}
	return ret;
}

void usage(char **argv)
{
	cout << "Usage: " << argv[0] << " [options]\n";
	cout << "Options:\n";
	cout << "-c conffile    Specifies the config file to use, defaults to\n\t/etc/dryad/dryad.conf\n";
	cout << "-t [port]      Specifies that the TCP network module should be started.\n\tIf a port is specified, it will bind to that port, otherwise the\n\tRFC specified port will be used. NYI.\n";
	cout << "-u [port]      Specifies that the UDP network module should be started.\n\tIf a port is specified, it will bind to that port, otherwise the\n\tRFC specified port will be used.\n";
	cout << "NB: It is valid to specify both -t and -u.\n";
}

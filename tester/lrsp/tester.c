#include <lrsp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
//--------------------------

void err_msg(int err, char *msg);

int main( int argc, char *argv[] )
{
	int dport, c, times, err;
	char *msg, *ipaddr;
	
	if( argc != 4 && argc != 5 )
	{
		fprintf(stderr, "%s ipaddress port message [times]\n", argv[0]);
		exit(1);
	}
	dport = atoi(argv[2]);
	if( dport < 1 )
	{
		fprintf(stderr, "%s ipaddress port message [times]\n", argv[0]);
		exit(1);
	}
	times = 1;
	if( argc == 5 )
		times = atoi(argv[4]);
	if( times < 1 )
	{
		fprintf(stderr, "%s ipaddress port message [times]\n", argv[0]);
		exit(1);
	}
	ipaddr = argv[1];
	if( times == 1 )
		err = lrsp_client_init(ipaddr, dport, LRSP_SINGLE);
	else
		err = lrsp_client_init(ipaddr, dport, LRSP_PERSISTANT);
		
	lrsp_client_register_callback(err_msg);

	msg = argv[3];
	
	for( c = 0; c < times; c++ )
	{
		fprintf( stderr, "sending #%d (%s): ", c, msg );
		lrsp_client_send_message(msg);
		fprintf( stderr, "done.\n" );
	}

	lrsp_client_free();

	return EXIT_SUCCESS;
}

void err_msg(int err, char *msg)
{
	fprintf(stderr, "%s", lrsp_client_error_message(err));
	exit(1);
}


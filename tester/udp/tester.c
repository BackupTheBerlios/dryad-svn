#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
//--------------------------
int main( int argc, char *argv[] )
{
	int dport, sock, ret, msglen;
	char *msg, *ipaddr;
	struct sockaddr_in mine, his;
	
	if( argc != 4 )
	{
		fprintf(stderr, "%s ipaddress port message\n", argv[0]);
		exit(1);
	}
	dport = atoi(argv[2]);
	if( dport < 1 )
	{
		fprintf(stderr, "%s ipaddress port message\n", argv[0]);
		exit(1);
	}
	ipaddr = argv[1];

	if( -1 == (sock = socket(AF_INET, SOCK_DGRAM, 0)) )
	{
		fprintf( stderr, "Failed to create socket!\n" );
		perror(NULL);
		exit(1);
	}

	mine.sin_family = AF_INET;
	mine.sin_port = htons(0);
	mine.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(mine.sin_zero), '\0', 8);
	if( -1 == bind(sock, (struct sockaddr*)&mine, sizeof(struct sockaddr)) )
	{
		fprintf( stderr, "Failed to bind socket!\n" );
		perror(NULL);
		exit(1);
	}

	his.sin_family = AF_INET;
	his.sin_port = htons(dport);
	his.sin_addr.s_addr = inet_addr(ipaddr);
	memset(&(his.sin_zero), '\0', 8);

	msg = argv[3];
	msglen = strlen(msg);
	
	ret = sendto(sock, msg, msglen, 0, (struct sockaddr*)&his, sizeof(struct sockaddr));
	printf( "Sent %d bytes to %s:%d.\n", ret, ipaddr, dport );
	perror(NULL);
	return EXIT_SUCCESS;
}

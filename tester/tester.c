#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
//--------------------------
int main( int argc, char *argv[] )
{
	int dport, sock, ret, msglen;
	char *msg;
	struct sockaddr_in mine, his;
	
	if( argc != 2 )
	{
		fprintf(stderr, "%s port\n", argv[0]);
		exit(1);
	}
	dport = atoi(argv[1]);
	if( dport < 1 )
	{
		fprintf(stderr, "%s port\n", argv[0]);
		exit(1);
	}

	sock = socket(AF_INET, SOCK_DGRAM, 0);

	mine.sin_family = AF_INET;
	mine.sin_port = htons(0);
	mine.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(mine.sin_zero), '\0', 8);
	bind(sock, (struct sockaddr*)&mine, sizeof(struct sockaddr));

	his.sin_family = AF_INET;
	his.sin_port = htons(dport);
	his.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(&(his.sin_zero), '\0', 8);

	msg = (char*)malloc(sizeof(char) * 78);
	msg = "<34>Oct 11 22:14:15 mymachine su: 'su root' failed for lonvick on /dev/pts/8\0";
	msglen = 77;
	
	ret = sendto(sock, msg, msglen, 0, (struct sockaddr*)&his, sizeof(struct sockaddr));
	printf( "Sent %d bytes.\n", ret );
	perror(NULL);
	return EXIT_SUCCESS;
}

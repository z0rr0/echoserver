#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/*
#include <fcntl.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
*/

#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>

#include "mtransport.h"
#include "merror.h"

#define BUFSIZE 255

/* echoserver.c 
transport - mtransport.c
error - merror.c
*/

int listenfd, client[FD_SETSIZE]; // my socket 

/* signal detect */
static void sig_handler(int sig, siginfo_t *siginfo, void *context);

int main(int argc, char* argv[])
{
	int port; // port
	struct sockaddr_in serveraddr; // server addr
	struct sockaddr_in clientaddr; // client addr
	char buf[BUFSIZE]; // message buffer 
	struct sigaction act;
	
	// conditions
	if (argc < 2 || !(port=atoi(argv[1]))) {
		printf("Error use: <%s> <port: number 50000-55000>\n", argv[0]);
		errorhand("exit \n");
	}
	listenfd = create_socket(&serveraddr, &port);
	
	act.sa_sigaction = &sig_handler;
	sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
	if (sigaction(SIGINT, &act, NULL) < 0)
		errorhand("sigaction error \n");

	// working
	printf("use port #%d\n", port);
	
	// block code
	if (noblock_sock(listenfd, (struct sockaddr *) &clientaddr, buf, client) !=0)
		errorhand("error block socket\n");

	return 0;
}

static void sig_handler(int sig, siginfo_t *siginfo, void *context)
{
	int i;
	for (i=0; i<FD_SETSIZE; i++)
		if (client[i] != -1)
			close (client[i]);
	close(listenfd);
	fprintf(stderr, "\nSignal...Ctrl+C (close cockets, exit) \n");
	exit(0);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "mtransport.h"
#include "merror.h"

#define BUFSIZE 255

int bind_port(int *port, struct sockaddr_in *serveraddr, int listenfd)
{
	int range_port[2]={50000, 55000};
	memset((char *) serveraddr, 0, sizeof(*serveraddr));
	// change port for internet
	serveraddr->sin_port = htons(*port);
	// bind=sock & address
	if (bind(listenfd, (struct sockaddr *) serveraddr,  sizeof(*serveraddr)) >= 0) {
		return 0;
	}
	else {
		for (*port = range_port[0]; *port <= range_port[1]; ++(*port)) {
			serveraddr->sin_port = htons(*port);
			if (bind(listenfd, (struct sockaddr *) serveraddr,  sizeof(*serveraddr)) >= 0)
				return 0; 
		}
	}
	close(listenfd);
	return 1;
}

int create_socket(struct sockaddr_in *serveraddr, int *port)
{
	int sock;
	// SOCK_STREAM or SOCK_DGRAM (datagram )
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) 
		errorhand("Error create socket\n");
	if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0)
		errorhand("Error create nonblock socket (fcntl)\n");
		
	memset((char *) serveraddr, 0, sizeof(serveraddr));
	serveraddr->sin_family = AF_INET;
	serveraddr->sin_addr.s_addr = INADDR_ANY;
	// select free port
	if (bind_port(port, (struct sockaddr *) serveraddr, sock) != 0) 
		errorhand("Error bind socket and address\n");
	return sock;
}

int block_sock(int sock, struct sockaddr_in *clientaddr, char *buf)
{
	int connfd, clientlen, n;
	clientlen = sizeof(clientaddr);
	connfd = accept(sock, (struct sockaddr *) clientaddr, &clientlen);
	if (connfd < 0)
		errorhand("read error\n");
	memset(buf, 0, BUFSIZE);
	n = read(connfd, buf, BUFSIZE-1);
	if (n < 0) 
		errorhand("error read from socket\n");
	printf("server received %d bytes: %s\n", n, buf);
	n = write(connfd, buf, strlen(buf));
	if (n < 0) 
		errorhand("Error writing to socket\n");
	return 0;
}

int noblock_sock(int sock, struct sockaddr_in *clientaddr, char *buf, int *client)
{
	fd_set allset, rfd; // set of open sockets
	struct timeval tv;
	int i, select_result, clientlen, connfd, n, maxfd, maxid, sockfd;
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	// allow 1 request
	if (listen(sock, 1) < 0) 
		errorhand("Error socket listen\n");
	
	FD_ZERO(&allset);
	FD_SET(sock, &allset);
	clientlen=sizeof(*clientaddr);
	maxfd = sock;
	maxid = -1;
	
	for (i=0; i<FD_SETSIZE; i++)
		client[i] = -1;
	
	while (1) {
		rfd = allset;
		select_result = select(maxfd+1, &rfd, NULL, NULL, NULL);
		if (FD_ISSET(sock, &rfd)) {
			printf("socket %d was ready\n", sock);
			if ((connfd = accept(sock, (struct sockaddr *) clientaddr, &clientlen)) < 0) {
				close(sock);
				errorhand("accept error\n");
			}
			for (i = 0; i<FD_SETSIZE; i++) {
				if (client[i]<0) {
					client[i] = connfd;
					printf("New client[%d], socket %d\n", i, client[i]);
					break;
				}
			}
			if (i == FD_SETSIZE) {
				printf("Too many clients\n");
				close(connfd);
			}
			else {
				FD_SET(connfd, &allset);
				if (maxfd < connfd)
					maxfd = connfd;
				if (i > maxid)
					maxid = i;
			}
		}
		for (i=0; i <= maxid; i++) {
			if((sockfd = client [i])<0)
				continue;
			if (FD_ISSET(sockfd, &rfd)) {
				memset(buf, 0, BUFSIZE);
				if ((n = read(sockfd, buf, BUFSIZE)) < 0) {
					printf("error read from socket\n");
					close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i]=-1;
				}
				else {
					printf("server received %d bytes: <%s> from client #%d\n", n, buf, i);
					memset(buf, 0, BUFSIZE);
					strcpy(buf, "pong");
					if ((n=write(sockfd, buf, strlen(buf))) < 0) {
						printf("Error send mes to client");
						close(sockfd);
						FD_CLR(sockfd, &allset);
						client[i]=-1;
					}
				}
			}
		}
	}
	return 0;
}

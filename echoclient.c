#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/fcntl.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFSIZE 255

int main(int argc, char const *argv[])
{
	int port; // port
	int listenfd; // my socket, conn-soccket
	struct sockaddr_in serveraddr; // server addr 
	char buf[BUFSIZE+1];
	struct hostent *server;
	int n;	

	// conditions
	if (argc != 3 || !(port=atoi(argv[2]))) {
		printf("Error use: <%s> <host> <port: see server>\n", argv[0]);
		return 1;
	}
	server = gethostbyname(argv[1]);
	if (server == NULL)
	{
	  printf("Host not found\n");
	  return 1;
	}
	memset((char *) &serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	strncpy((char *) &serveraddr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
	serveraddr.sin_port = htons(port);
	
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error create socket\n");
		return 1;
	}
	//fcntl(listenfd, F_SETFL, O_NONBLOCK);
	// conect to server
	if (connect(listenfd, &serveraddr, sizeof(serveraddr)) < 0) 
	{
		printf("Connect error\n");
		return 1;
	}
	printf("Start\n");
	memset(buf, 0, BUFSIZE);
	strcpy(buf, "ping");
	if (write(listenfd, buf, strlen(buf)) < 0) {
		perror("Error send mes");
		return 1;
	}
	printf("write ok\n");
	while ((n=read(listenfd, buf, BUFSIZE)) > 0) {
		printf("get %d bytes from %s <%s>\n", n, inet_ntoa(serveraddr.sin_addr), buf);
		memset(buf, 0, BUFSIZE);
		strcpy(buf, "ping");
		write(listenfd, buf, strlen(buf));
		sleep(1);
	}
	close(listenfd);
	return 0;
}

#ifdef MTRANSPORT_H
#define MTRANSPORT_H

int bind_port(int *port, struct sockaddr_in *serveraddr, int listenfd);
int create_socket(struct sockaddr_in *serveraddr, int *port);
int block_sock(int sock, struct sockaddr_in *clientaddr, char *buf);
int noblock_sock(int sock, struct sockaddr_in *clientaddr, char *buf);

#endif

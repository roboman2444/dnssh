#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <arpa/inet.h>


#include "protocol.h"
#include "server.h"
#include "server_getaddrinfo.h"
#include <sys/socket.h>


int server_listen(int port){
	int servsock;
	struct sockaddr_in servaddr = {0};
	if((servsock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr, "DNSSH listen failed for port %i\n", port);
		return -1;
	}
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);
	if(bind(servsock, (struct sockaddr *) &servaddr, sizeof(struct sockaddr_in)) < 0){
		if(servsock) close(servsock);
		fprintf(stderr, "DNSSH bind failed for port %i\n", port);
		return -1;
	}
	if(listen(servsock, 5) < 0){
		if(servsock) close(servsock);
		fprintf(stderr, "DNSSH listen failed for port %i\n", port);
		return -1;
	}
	return servsock;
}

int server_accept(int sock){
	struct sockaddr_in clientaddr;
	unsigned int clientlen = sizeof(struct sockaddr_in);
	int serv;
	if((serv = accept(sock, (struct sockaddr *) &clientaddr, &clientlen)) < 0){
		fprintf(stderr, "DNSSH accept failed\n");
		return -1;
	}
	printf("DNSSH client %s connected\n", inet_ntoa(clientaddr.sin_addr));
	return serv;
}



int main(void){
	int lsock = server_listen(5335);
	if(lsock <0) return 1;
	while(1){
		int csock = server_accept(lsock);
		int len;
		unsigned char *data = 0;
		int srec = protocol_receive(csock, &len, &data);
		server_getaddrinfo(csock, data);
		if(data)free(data);
		close(csock);
	}
	//lol;
	return 0;
}

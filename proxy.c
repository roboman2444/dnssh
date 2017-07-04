#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>


#include "dnssh.h"
#include "proxy.h"
#include <sys/socket.h>


char * phost = 0;
int pport = -1;


int proxy_init(char * hostname, int port){
	phost = strdup(hostname);
	pport = port;
	return 1;
}

int proxy_connect(char * host, int port){
	if(!host){ host = phost; port = pport;}
	struct sockaddr_in servaddr = {0};
	struct hostent * thost;
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	if((thost = gethostbyname_orig(host)) == NULL){
		fprintf(stderr, "DNSSH: Hostname resolution failed for %s, unable to connect to proxy server\n", host);
		return -1;
	}
	memcpy(&servaddr.sin_addr, thost->h_addr_list[0], thost->h_length);
	int sock;
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) <0){
		fprintf(stderr, "DNSSH Socket failed for %s port %i, unable to connect to proxy server\n", host, port);
		return -1;
	}
	if(connect(sock, (struct sockaddr *) &servaddr, sizeof(struct sockaddr_in)) <0){
		fprintf(stderr, "DNSSH Connect failed for %s port %i, unable to connect to proxy server\n", host, port);
		return -1;
	}
	return sock;
}

int proxy_listen(int port){
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

int proxy_accept(int sock){
	struct sockaddr_in clientaddr;
	unsigned int clientlen = sizeof(struct sockaddr_in);
	int serv;
	if((serv = accept(sock, (struct sockaddr *) &clientaddr, &clientlen)) < 0){
		fprintf(stderr, "DNSSH accept failed\n");
		return -1;
	}
	printf("DNSSH cleint %s connected\n", inet_ntoa(clientaddr.sin_addr));
	return serv;
}


int proxy_send(int sock, int length, unsigned char * data){
	//come up with data
	pheader_t h ={0};
	h.length = length;
	int ret = send(sock, &h, sizeof(pheader_t), 0);
	if(ret != sizeof(pheader_t)) return 0;
	ret = send(sock, data, length, 0);
	return ret;
}
int proxy_recieve(int sock, int *length, unsigned char **data){
	pheader_t h ={0};
	int ret;
	int bytesrec = 0;
	if(*length)*length = 0;
	ret = recv(sock, &h, sizeof(pheader_t), 0);
	if(ret != sizeof(pheader_t)) return 0;
	//allocate size
	if(h.length < 1) return 0;
	if(*data)free(*data);
	*data = malloc(h.length);
	while(bytesrec < h.length){
		ret = recv(sock, *data +bytesrec, h.length-bytesrec, 0);
		if(ret <0){
			//todo this error all nice
			fprintf(stderr, "DNSSH size failure\n");
			return 0;
		}
		bytesrec+=ret;
	}
	if(*length) *length = bytesrec;
	return bytesrec;
}

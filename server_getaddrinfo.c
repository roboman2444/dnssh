#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <unistd.h>

#include "protocol.h"
#include "dnssh.h"
#include "server.h"
#include "server_getaddrinfo.h"

int server_getaddrinfo(int sock, unsigned char * data){
	addrinfosend_t *sd = (addrinfosend_t *)data;
	struct addrinfo hints = {0};
	if(sd->hints){
		hints.ai_family = sd->ai_family;
		hints.ai_socktype = sd->ai_socktype;
		hints.ai_protocol = sd->ai_protocol;
		hints.ai_flags = sd->ai_flags;
	}
	char *node = 0;
	char * service = 0;
	if(sd->nodelen) node = strdup((char*)data + sizeof(addrinfosend_t));
	if(sd->servlen) service = strdup((char*)data + sizeof(addrinfosend_t) + sd->nodelen);
	printf("node %s serv %s\n", node, service);
	struct addrinfo * res = 0;
	int retval = getaddrinfo(node, service, sd->hints ? &hints : 0, &res);

	//walk ll, count number of addrs, total size of addrlen, total size of canonname len
	struct addrinfo * w;
	int i;
	int addrlentot=0;
	int canonnametot=0;
	for(i = 0, w= res; w; w= w->ai_next, i++){
		addrlentot+= w->ai_addrlen;
		if(w->ai_canonname) canonnametot+= strlen(w->ai_canonname)+1;
	}

	addrinforec_t r = {0};
	r.ret = retval;
	r.numres = i;
	printf("ret %i numres %i addrlentot %i canonnametot %i\n", r.ret, r.numres, addrlentot, canonnametot);
	int datalen = sizeof(addrinforec_t) + r.numres * sizeof(addrinfofickle_t) + addrlentot + canonnametot;
	unsigned char *senddata = malloc(datalen);
	memset(senddata, 0, datalen);

	memcpy(senddata, &r, sizeof(addrinforec_t));


	addrinfofickle_t * addrs = (addrinfofickle_t *)(senddata + sizeof(addrinforec_t));
	int addrst = sizeof(addrinforec_t) + r.numres * sizeof(addrinfofickle_t) ;
	int canonnamest = addrst + addrlentot;
	//walk, set up stuff
	for(i = 0, w= res; w; w= w->ai_next, i++){
		addrs[i].ai_flags = w->ai_flags;
		addrs[i].ai_family = w->ai_family;
		addrs[i].ai_socktype = w->ai_socktype;
		addrs[i].ai_protocol = w->ai_protocol;
		addrs[i].ai_addrlen = w->ai_addrlen;


		addrs[i].addr = addrst;
		memcpy(senddata + addrst, w->ai_addr, w->ai_addrlen);
		addrst+= w->ai_addrlen;
		if(w->ai_canonname){
			addrs[i].canonname = canonnamest;
			strcpy((char *)senddata + canonnamest, w->ai_canonname);
			canonnamest+= strlen(w->ai_canonname)+1;
		}
	}
	//everything set up

	protocol_send(sock, datalen, senddata);

	if(senddata) free(senddata);

	return 0;
}

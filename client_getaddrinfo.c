#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <unistd.h>

#include "protocol.h"
#include "dnssh.h"
#include "client.h"
#include "client_getaddrinfo.h"

int client_getaddrinfo(const char * node, const char * service, const struct addrinfo *hints, struct addrinfo **res){
	*res = 0;
	//construct data to send (node, service, hints)
	addrinfosend_t sd = {0};
	if(hints){
		sd.hints = 1;
		sd.ai_family = hints->ai_family;
		sd.ai_socktype = hints->ai_socktype;
		sd.ai_protocol = hints->ai_protocol;
		sd.ai_flags = hints->ai_flags;
	}
	if(node)sd.nodelen = strlen(node)+1;
	if(service)sd.servlen = strlen(service)+1;

	int len = sizeof(addrinfosend_t) + sd.nodelen + sd.servlen;
	unsigned char *d = malloc(len);
	memcpy(d, &sd, sizeof(addrinfosend_t));
	memcpy(d+sizeof(addrinfosend_t), node, sd.nodelen);
	memcpy(d+sizeof(addrinfosend_t) + sd.nodelen, service, sd.servlen);

	//connect
	int sock = client_connect(0, 0);
	if(sock < 1){
		//todo
		fprintf(stderr, "DNSSH unable to connect\n");
		return 0;
	}
	//send data
	//todo do something with sent
	int sent = protocol_send(sock, len, d);
	free(d);
	d=0;
		//server does shit
	//get data back

	//todo do something with srec
	len =0;
	int srec = protocol_receive(sock, &len, &d);
	//close
	close(sock);
//	if(srec != 1){
//		fprintf(stderr, "DNSSH received an unknown response %i from server\n", srec);
//		return 0;
//	}
	if(len < sizeof(addrinforec_t)){
		fprintf(stderr, "DNSSH received %i bytes from server, not enough\n", len);
		return 0;
	}
	//construct results (return code, res)
	addrinforec_t *r = (addrinforec_t *) d;
	int retval = r->ret;

	if(r->numres){
		//create new ll
		addrinfofickle_t *addrs = (addrinfofickle_t *)(d+sizeof(addrinforec_t));

		struct addrinfo ** oldaddrnext = res; //pointer to previous node's ll, or to start pointer
		int i;
		for(i = 0; i < r->numres; i++){
			struct addrinfo * newaddr = malloc(sizeof(struct addrinfo));


			newaddr->ai_flags = addrs[i].ai_flags;
			newaddr->ai_family = addrs[i].ai_family;
			newaddr->ai_socktype = addrs[i].ai_socktype;
			newaddr->ai_protocol = addrs[i].ai_protocol;
			newaddr->ai_addrlen = addrs[i].ai_addrlen;
			newaddr->ai_addr = 0;		//will do further down
			newaddr->ai_canonname = 0;	//will do further down
			newaddr->ai_next = 0;		//will do in next loop itteration (if needed)

			*oldaddrnext = newaddr;
			oldaddrnext = &newaddr->ai_next;

			if(addrs[i].canonname)		newaddr->ai_canonname = strdup((char*)(d + addrs[i].canonname));
			if(addrs[i].addr){
				newaddr->ai_addr = malloc(newaddr->ai_addrlen);
				memcpy(newaddr->ai_addr, addrs[i].addr + d, newaddr->ai_addrlen);
			}
		}
	}
	//all data allocated in the linked list should be freed by a call to freeaddrinfo

	free(d);
	return retval;
}

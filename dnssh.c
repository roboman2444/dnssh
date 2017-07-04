#define __USE_GNU
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <dlfcn.h>

#include "dnssh.h"
#include "proxy.h"
#include "proxy_getaddrinfo.h"

//#define HOOKDLSYM
#ifdef HOOKDLSYM
static void * (* real_dlsym)(void *, const char *) = NULL;
#else
#define real_dlsym dlsym
#endif

//todo getservbyname?
struct hostent  * (*gethostbyname_orig)(const char *name) = 0;
struct hostent  * (*gethostbyname2_orig)(const char *name, int af) = 0;
int (*getaddrinfo_orig)(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res) = 0;

int init(void){
	char *hostname = getenv("DNSSHHOST");
	if(!hostname){
		hostname = "localhost";
		printf("DNSSH: environment variable DNSSHHOST not set, defaulting to %s\n", hostname);
	}
	char *sport = getenv("DNSSHPORT");
	int port = -1;
	if(sport) port = atoi(sport);
	if(port < 1 || port > 65535){
		port = 5335;
		printf("DNSSH: environment variable DNSSHPORT not set or out of range, defaulting to %i\n", port);
	}


	proxy_init(hostname, port);

	if(!getaddrinfo_orig && !(getaddrinfo_orig = real_dlsym(RTLD_NEXT, "getaddrinfo"))){
		printf("DNSSH: unable to get origional getaddrinfo, exiting\n");
		exit(-1);
	}
	if(!gethostbyname_orig && !(gethostbyname_orig = real_dlsym(RTLD_NEXT, "gethostbyname"))){
		printf("DNSSH: unable to get origional gethostbyname, exiting\n");
		exit(-1);
	}
	if(!gethostbyname2_orig && !(gethostbyname2_orig = real_dlsym(RTLD_NEXT, "gethostbyname2"))){
		printf("DNSSH: unable to get origional gethostbyname2, exiting\n");
		exit(-1);
	}
	return 1;
}

int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res){
	printf("\nDNSSH: dns request (getaddrinfo) node: %s, service: %s, hints? %s\n", node, service, hints ? "yes": "no");
	if(!getaddrinfo_orig) init();
//	return getaddrinfo_orig(node, service, hints, res);
	return proxy_getaddrinfo(node, service, hints, res);
}

struct hostent *gethostbyname(const char *name){
	printf("\nDNSSH: dns request (gethostbyname) for %s\n", name);
	if(!gethostbyname_orig) init();
	return gethostbyname_orig(name);
}

struct hostent *gethostbyname2(const char *name, int af){
	printf("\nDNSSH: dns request (gethostbyname2) for %s, af %i\n", name, af);
	if(!gethostbyname2_orig) init();
	return gethostbyname2_orig(name, af);
}


#ifdef HOOKDLSYM
extern void *_dl_sym(void *, const char *, void *);
extern void * dlsym(void * handle, const char * symbol){
	if(!real_dlsym && !(real_dlsym = _dl_sym(RTLD_NEXT, "dlsym", dlsym) )){
		printf("DNSSH: unable to get origional dlsym, exiting\n");
		exit(0);
	}
	void *ret = NULL;
	if(!strcmp(symbol, "dlsym")) return dlsym;
	if(!(ret = real_dlsym(handle, symbol))) return ret;
	printf("hooked dlsym, %s\n", symbol);
	if(!strcmp(symbol, "gethostbyname")){
		gethostbyname_orig = real_dlsym(handle, "gethostbyname");
		return gethostbyname;
	} else if(!strcmp(symbol, "gethostbyname2")){
		gethostbyname2_orig = real_dlsym(handle, "gethostbyname2");
		return gethostbyname2;
	} else if(!strcmp(symbol, "getaddrinfo")){
		getaddrinfo_orig = real_dlsym(handle, "getaddrinfo");
		return getaddrinfo;
	}

	init();
	return ret;
}
#endif

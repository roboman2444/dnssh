#ifndef PROXYHEADER
#define PROXYHEADER

int client_init(char * hostname, int port);
//as of now it just sets some flags to connect on every dns request
int client_connect(char * hostname, int port);


#endif

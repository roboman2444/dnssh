#ifndef PROXYHEADER
#define PROXYHEADER

typedef struct pheader_s {
	int length;	//in bytes
	int type;	//what was sent
	int uid;	//maybe
} pheader_t;


int proxy_init(char * hostname, int port);
//as of now it just sets some flags to connect on every dns request
int proxy_connect(char * hostname, int port);

//copies and sends
int proxy_send(int sock, int length, unsigned char * data);
//recieves
int proxy_recieve(int sock, int *length, unsigned char ** data);


#endif

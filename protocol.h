#ifndef PROTOCOLHEADER
#define PROTOCOLHEADER


typedef struct pheader_s {
        int length;     //in bytes
        int type;       //what was sent
        int uid;        //maybe
} pheader_t;


typedef struct addrinfosend_s {
        int hints;
        int ai_family;
        int ai_socktype;
        int ai_protocol;
        int ai_flags;
        int nodelen;	//null terminated string
        int servlen;	//null terminated string
}addrinfosend_t;

typedef struct addrinforec_s {
        int ret;
        int numres;
} addrinforec_t;

typedef struct addrinfofickle_s {
        int ai_flags;
        int ai_family;
        int ai_socktype;
        int ai_protocol;
        int ai_addrlen;
        int addr; //offset from the recieved data
        int canonname;  //offset from the recieved data, null terminated string
} addrinfofickle_t;


int protocol_send(int sock, int length, unsigned char * data);
int protocol_receive(int sock, int *length, unsigned char **data);



#endif

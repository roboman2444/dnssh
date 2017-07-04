#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>

#include "protocol.h"
int protocol_send(int sock, int length, unsigned char * data){
        //come up with data
        pheader_t h ={0};
        h.length = length;
        int ret = send(sock, &h, sizeof(pheader_t), 0);
        if(ret != sizeof(pheader_t)) return 0;
        ret = send(sock, data, length, 0);
        return ret;
}


int protocol_receive(int sock, int *length, unsigned char **data){
        pheader_t h ={0};
        int ret;
        int bytesrec = 0;
        if(length)*length = 0;
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
        if(length) *length = bytesrec;
        return h.type;
}

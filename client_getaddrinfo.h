#ifndef CLIENT_GETADDRINFOHEADER
#define CLIENT_GETADDRINFOHEADER

int client_getaddrinfo(const char *node, const char * service, const struct addrinfo *hints, struct addrinfo **res);

#endif

#ifndef DNSSHHEADER
#define DNSSHHEADER
struct hostent  * (*gethostbyname_orig)(const char *name);
struct hostent  * (*gethostbyname2_orig)(const char *name, int af);
int (*getaddrinfo_orig)(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
#endif

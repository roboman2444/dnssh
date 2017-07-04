CC = gcc
LDFLAGS =-ldl -shared -Wl,-soname,dnssh.so
CFLAGS = -Wall -Ofast -fstrict-aliasing -march=native -fPIC
OBJECTS = dnssh.o proxy.o proxy_getaddrinfo.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

dnssh.so: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

debug:	CFLAGS= -Wall -O0 -g -fstrict-aliasing -march=native -fPIC
debug: 	$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@.so $(LDFLAGS)
clean:
	rm -f $(OBJECTS)

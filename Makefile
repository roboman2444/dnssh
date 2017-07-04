CC = gcc
LDFLAGS =-ldl -shared -Wl,-soname,dnssh.so
CFLAGS = -Wall -Ofast -fstrict-aliasing -march=native -fPIC
OBJECTS = dnssh.o client.o client_getaddrinfo.o protocol.o

SERVERLDFLAGS =
SERVERCFLAGS = -Wall -Ofast -fstrict-aliasing -march=native
SERVEROBJECTS = protocol.o server.o server_getaddrinfo.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

dnssh.so: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

server: CFLAGS=$(SERVERCFLAGS)
server: $(SERVEROBJECTS)
	$(CC) $(SERVERCFLAGS) $(SERVEROBJECTS) -o $@ $(SERVERLDFLAGS)

serverdebug: SERVERCFLAGS= -Wall -O0 -g -fstrict-aliasing -march=native
serverdebug: CFLAGS=$(SERVERCFLAGS)
serverdebug: $(SERVEROBJECTS)
	$(CC) $(SERVERCFLAGS) $(SERVEROBJECTS) -o $@ $(SERVERLDFLAGS)


debug:	CFLAGS= -Wall -O0 -g -fstrict-aliasing -march=native -fPIC
debug: 	$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@.so $(LDFLAGS)
clean:
	rm -f $(OBJECTS) $(SERVEROBJECTS)

CC = gcc
CFLAGS = -Wall
LIBS = -lmytest
LDFLAGS = -L./

all: mytest

mytest: mytest.c libmytest.a
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

libmytest.a: create.o client.o server.o
	ar rc $@ $^
	ar tv $@

create.o: create.c
	$(CC) $(CFLAGS) -c $<

client.o: client.c
	$(CC) $(CFLAGS) -DTIMES -c $<

server.o: server.c
	$(CC) $(CFLAGS) -DTIMES -c $<

clean:
	rm -f *.o *.a *.dat mytest

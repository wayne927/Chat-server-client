CC=gcc
LIBS=-lpthread

%.o: %.c
	$(CC) -c -o $@ $<

all: server.o client.o
	$(CC) -o server server.o $(LIBS)
	$(CC) -o client client.o $(LIBS)
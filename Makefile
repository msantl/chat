CC=gcc
CFLAGS=-Wall -pedantic
OBJECTS_S=misc.o server.o
OBJECTS_C=misc.o client.o
LIBS=-lncurses -lpthread

client: $(OBJECTS_C)
	$(CC) $(OBJECTS_C) $(LIBS) $(CFLAGS) -o $@ 

server: $(OBJECTS_S)
	$(CC) $(OBJECTS_S) $(CFLAGS) -o $@ 

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
  
clean:
	rm -f *.o client server

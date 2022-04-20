CC = gcc

CFLAGS = -c -W -Wall -Wextra

.PHONY:all
all: start

start:	main.o child.o
	$(CC) main.o -o main
	$(CC) child.o -o child
	
main.o: main.c
	$(CC) $(CFLAGS) main.c
	
child.o: child.c
	$(CC) $(CFLAGS) child.c

clean:
	rm -rf *.o main child	

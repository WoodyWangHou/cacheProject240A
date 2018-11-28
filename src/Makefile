CC=gcc
OPTS=-g -std=c99 -Werror

all: main.o cache.o utils.o
	$(CC) $(OPTS) -lm -o cache main.o cache.o utils.o

main.o: main.c cache.h
	$(CC) $(OPTS) -c main.c

cache.o: cache.h cache.c
	$(CC) $(OPTS) -c cache.c

utils.o: utils.h utils.c
	$(CC) $(OPTS) -c utils.c

clean:
	rm -f *.o cache;

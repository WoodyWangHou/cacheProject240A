CC=g++
OPTS=-g -std=c++11 

all: main.o cache.o utils.o
	$(CC) $(OPTS) -lm -o cache main.o cache.o utils.o

main.o: main.c cache.h
	$(CC) $(OPTS) -c main.c

cache.o: cache.h cache.cpp
	$(CC) $(OPTS) -c cache.cpp

utils.o: utils.h utils.c
	$(CC) $(OPTS) -c utils.c

clean:
	rm -f *.o cache;

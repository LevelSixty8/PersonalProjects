CC = g++
CFLAGS = -std=c++14 -Wall -Wextra -O2

all: encoder

encoder: main.o hash.o
	$(CC) $(CFLAGS) main.o hash.o -o encoder
main.o: main.cpp hash.h
	$(CC) $(CFLAGS) -c main.cpp
hash.o: hash.cpp hash.h
	$(CC) $(CFLAGS) -c hash.cpp
clean: 
	rm -f *.o encoder

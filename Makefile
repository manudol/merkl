CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lssl -lcrypto

all: outmerkle

$(shell mkdir -p build)

hash.o: hash.c hash.h
	$(CC) $(CFLAGS) -c hash.c -o ./build/hash.o

merkl.o: merkl.c merkl.h
	$(CC) $(CFLAGS) -c merkl.c -o ./build/merkl.o

main.o: main.c merkl.h hash.h
	$(CC) $(CFLAGS) -c main.c -o ./build/main.o 

outmerkle: main.o merkl.o hash.o
	$(CC) $(CFLAGS) ./build/main.o ./build/merkl.o -o outmerkle $(LIBS)

clean:
	rm -rf ./build outmerkle

CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lssl -lcrypto

all: outmerkle

$(shell mkdir -p build)

merkl.o: merkl.c merkl.h
	$(CC) $(CFLAGS) -c merkl.c -o ./build/merkl.o

main.o: main.c merkl.h
	$(CC) $(CFLAGS) -c main.c -o ./build/main.o 

outmerkle: main.o merkl.o
	$(CC) $(CFLAGS) ./build/main.o ./build/merkl.o -o outmerkle $(LIBS)

clean:
	rm -rf ./build outmerkle

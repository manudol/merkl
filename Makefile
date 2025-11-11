CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lssl -lcrypto

all: outmerkle

$(shell mkdir -p build)

hash.o: hash.c hash.h
	$(CC) $(CFLAGS) -c hash.c -o ./build/hash.o

gettime.o: gettime.c gettime.h
	$(CC) $(CFLAGS) -c gettime.c -o ./build/gettime.o

data.o: data.c data.h
	$(CC) $(CFLAGS) -c data.c -o ./build/data.o

static_merkl.o: static_merkl.c static_merkl.h
	$(CC) $(FLAGS) -c static_merkl.c -o ./build/static_merkl.o

merkl.o: merkl.c merkl.h
	$(CC) $(CFLAGS) -c merkl.c -o ./build/merkl.o

main.o: main.c merkl.h static_merkl.h hash.h data.h gettime.h
	$(CC) $(CFLAGS) -c main.c -o ./build/main.o 

outmerkle: main.o merkl.o static_merkl.o hash.o data.o gettime.o
	$(CC) $(CFLAGS) \
    ./build/main.o  \
    ./build/merkl.o \
    ./build/static_merkl.o \
    ./build/hash.o \
    ./build/data.o \
    ./build/gettime.o \
    -o outmerkle $(LIBS)

clean:
	rm -rf ./build outmerkle

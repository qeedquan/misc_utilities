CC=wr-cc

all:
	$(CC) -Wall -fPIC -c hello.c
	$(CC) -shared -o libhello.so hello.o
	$(CC) -o dyn-hello.vxe main.c -lhello -L$(shell pwd)

clean:
	rm -f *.o *.so *.vxe

all:
	wr-cc -Wall -fPIC -c hello.c
	wr-cc -shared -o libhello.so hello.o
	wr-cc -o dyn-hello.vxe main.c -lhello -L$(shell pwd)

clean:
	rm -f *.o *.so *.vxe

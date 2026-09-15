all:
	cc -c -fpic *.c `pkg-config --cflags --libs tcl`
	cc -shared -o libtest.so lib.o

clean:
	rm -f *.o *.so

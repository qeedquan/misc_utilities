# Test linking against multiple libraries (.so) files
CC=cc
BIN=test

all:
	$(CC) -c bar.c -fPIC
	$(CC) -c foo.c -fPIC
	$(CC) bar.o -shared -o libbar.so
	# foo depends on bar, if -L. -lbar is not specified
	# it will not store the bar.so dependency in the foo.so file
	# and when it tries to link against the main program it will fail with undefine reference to bar
	$(CC) foo.o -shared -o libfoo.so -L. -lbar
	$(CC) -o $(BIN) main.c -lbar -lfoo -L.

clean:
	rm -f $(BIN) *.so *.o

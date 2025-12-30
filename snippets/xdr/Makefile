CFLAGS=-I/usr/include/tirpc
LIBS=-ltirpc

all: tests

tests:
	$(CC) $(CFLAGS) -o test_basic test_basic.c $(LIBS)

.PHONY: clean

clean:
	-rm -f test_basic

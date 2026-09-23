CFLAGS=-fsanitize=address -ggdb -g3 -Wall -Wextra -pedantic
LDFLAGS=`pkg-config --cflags --libs openssl tss2-esys tss2-sys tss2-tctildr`

all:
	cc -o test-tss test-tss.c $(CFLAGS) $(LDFLAGS)
	cc -o test-esys test-esys.c $(CFLAGS) $(LDFLAGS)
	cc -o test-openssl test-openssl.c $(CFLAGS) $(LDFLAGS)

clean:
	rm -f test-tss
	rm -f test-esys
	rm -f test-openssl

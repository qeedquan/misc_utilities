CFLAGS=-fsanitize=address -ggdb -g3 -Wall -Wextra -pedantic
LDFLAGS=-ltss2-esys `pkg-config --cflags --libs openssl`

all:
	cc -o test-tpm test-tpm.c $(CFLAGS) $(LDFLAGS)
	cc -o test-openssl test-openssl.c $(CFLAGS) $(LDFLAGS)

clean:
	rm -f test-tpm
	rm -f test-openssl

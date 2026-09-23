CFLAGS+=-Wall -Wextra -pedantic
CFLAGS+=$(shell pkg-config --cflags gio-2.0)
CFLAGS+=$(shell pkg-config --cflags gtk+-3.0)
LDFLAGS+=$(shell pkg-config --libs gio-2.0)
LDFLAGS+=$(shell pkg-config --libs gtk+-3.0)

all:
	cc -o notify-test notify-test.c $(CFLAGS) $(LDFLAGS)
	cc -o about-dialog-test about-dialog-test.c $(CFLAGS) $(LDFLAGS)

format:
	clang-format -i *.c

clean:
	rm -f about-dialog-test
	rm -f notify-test

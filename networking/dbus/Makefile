CFLAGS+=-Wall -Wextra -pedantic
CFLAGS+=$(shell pkg-config --cflags glib-2.0 gobject-2.0 gio-2.0)
CFLAGS+=$(shell pkg-config --cflags libsystemd)
#CFLAGS+= -fsanitize=address -ggdb -g3

LDFLAGS+=$(shell pkg-config --libs glib-2.0 gobject-2.0 gio-2.0)
LDFLAGS+=$(shell pkg-config --libs libsystemd)

all:
	cc -o mprisctl mprisctl.c $(CFLAGS) $(LDFLAGS)
	cc -o gen-guid gen-guid.c $(CFLAGS) $(LDFLAGS)
	cc -o dump-services dump-services.c $(CFLAGS) $(LDFLAGS)
	cc -o sdbus-test sdbus-test.c $(CFLAGS) $(LDFLAGS)
	cc -o sdbus-list-services sdbus-list-services.c $(CFLAGS) $(LDFLAGS)
	cc -o sdbus-dump-objects sdbus-dump-objects.c $(CFLAGS) $(LDFLAGS)
	cc -o sdbus-dump-types sdbus-dump-types.c $(CFLAGS) $(LDFLAGS)

clean:
	rm -f mprisctl gen-guid dump-services sdbus-test sdbus-list-services sdbus-dump-objects sdbus-dump-types

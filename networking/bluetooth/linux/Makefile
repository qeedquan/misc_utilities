CFLAGS+=-Wall -Wextra -pedantic
CFLAGS+=$(shell pkg-config --cflags glib-2.0 gobject-2.0 gio-2.0 libsystemd)
CFLAGS += -fsanitize=address -ggdb -g3

LDFLAGS+=$(shell pkg-config --libs glib-2.0 gobject-2.0 gio-2.0 libsystemd)
LDFLAGS+=-lbluetooth -ludev

all:
	cc -o gatt gatt.c $(CFLAGS) $(LDFLAGS)
	cc -o dump-bdaddr dump-bdaddr.c $(CFLAGS) $(LDFLAGS)
	cc -o dump-sockopt dump-sockopt.c $(CFLAGS) $(LDFLAGS)
	cc -o test-transfer test-transfer.c $(CFLAGS) $(LDFLAGS)
	cc -o discover-devices discover-devices.c $(CFLAGS) $(LDFLAGS)
	cc -o hci-read hci-read.c $(CFLAGS) $(LDFLAGS)
	cc -o dump-gatt-services dump-gatt-services.c $(CFLAGS) $(LDFLAGS)
	cc -o serve-gatt-test serve-gatt-test.c $(CFLAGS) $(LDFLAGS)
	go build l2ping.go
	go build test-hci.go

clean:
	rm -f gatt dump-bdaddr dump-sockopt test-transfer discover-devices hci-read dump-gatt-services serve-gatt-test l2ping test-hci

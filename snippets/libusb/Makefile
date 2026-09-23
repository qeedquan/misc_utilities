all:
	cc -o test-libusb test-libusb.c `pkg-config --cflags --libs libusb-1.0` -Wall -Wextra -pedantic

clean:
	rm -f test-libusb

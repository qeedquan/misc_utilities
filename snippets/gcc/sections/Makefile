CFLAGS=-maccumulate-outgoing-args -mcmodel=large
SECTION=-Wl,--section-start=.foo=0x31337
SECTION+=-Wl,--section-start=.bar=0x1234567

all:
	cc -o section section.c $(CFLAGS) $(SECTION)

clean:
	rm -f section

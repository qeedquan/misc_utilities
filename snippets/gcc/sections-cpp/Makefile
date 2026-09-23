CFLAGS=-maccumulate-outgoing-args -ggdb -g3
SECTION=-Wl,--section-start=.f_load=0x10000
SECTION+=-Wl,--section-start=.f_quux=0x30000
SECTION+=-Wl,--section-start=.f_fixed=0x95000
SECTION+=-Wl,--section-start=.cl_far_constructor=0x50000
SECTION+=-Wl,--section-start=.cl_far_faze=0x60000

all:
	g++ -c section.cpp $(CFLAGS)
	g++ -c main.cpp $(CFLAGS)
	g++ -o test-section *.o $(SECTION)

clean:
	rm -f test-section *.o

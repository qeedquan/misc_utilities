# WIND_CC_SYSROOT = $VSB_DIR

CSRC=$(wildcard *.c)
CPPSRC=$(wildcard *.cpp)
CUTIL=$(wildcard util/*.c)
INC=-Iutil

ASAN=0
STATIC_CFLAGS=-static
ASAN_CFLAGS=
ifeq ($(ASAN), 1)
	ASAN_CFLAGS=-fsanitize=address
	STATIC_CFLAGS=
endif

CC=cc
CPP=c++
CFLAGS=-ggdb -g3 -Wall -Wextra -pedantic -Wno-unused-parameter $(ASAN_CFLAGS) $(STATIC_CFLAGS)
CXXFLAGS=$(CFLAGS)
LDFLAGS=-lcrc

POSIXCFLAGS=-fsanitize=address -std=c2x
POSIXCXXFLAGS=-fsanitize=address -std=c++23
POSIXINC=-I../shim/posix
POSIXLDFLAGS=-L../shim/posix -lvxposixshim

WINDCC=wr-cc
WINDCPP=wr-c++
WINDCFLAGS=-rtp $(CFLAGS)

all: vxworks

vxworks:
	$(foreach file, $(CSRC), $(WINDCC) $(WINDCFLAGS) $(INC) -o $(subst .c,.vxe,$(file)) $(file) $(CUTIL) $(LDFLAGS);)
	$(foreach file, $(CPPSRC), $(WINDCPP) $(WINDCXXFLAGS) $(INC) -o $(subst .cpp,.vxe,$(file)) $(file) $(LDFLAGS);)

posix:
	$(foreach file, $(CSRC), $(CC) $(POSIXCFLAGS) $(CFLAGS) $(INC) $(POSIXINC) -o $(subst .c,.out,$(file)) $(file) $(CUTIL) $(POSIXLDFLAGS);)
	$(foreach file, $(CPPSRC), $(CPP) $(POSIXCXXFLAGS) $(CXXFLAGS) $(INC) $(POSIXINC) -o $(subst .cpp,.out,$(file)) $(file) $(POSIXLDFLAGS);)

clean:
	rm -f *.vxe *.out

# WIND_CC_SYSROOT = $VSB_DIR

SRC=$(wildcard *.c)
UTIL=$(wildcard util/*.c)
INC=-Iutil

CC=wr-cc
CFLAGS=-rtp -static -ggdb -g3 -Wall -Wextra -pedantic
LDFLAGS=-ltpm20tss -lcrypto

all:
	$(foreach file, $(SRC), $(CC) $(CFLAGS) $(INC) -o $(subst .c,.vxe,$(file)) $(file) $(UTIL) $(LDFLAGS);)

clean:
	rm -f *.vxe

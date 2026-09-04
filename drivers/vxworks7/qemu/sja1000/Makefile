# WIND_CC_SYSROOT = $VSB_DIR

SRC=$(wildcard *.c)

CC=wr-cc
CFLAGS=-dkm -ggdb -g3 -Wall -Wextra -pedantic

all:
	$(CC) $(CFLAGS) -o sja1000.out kvaser.c can3680.c sja1000.c

clean:
	rm -f *.out *.o

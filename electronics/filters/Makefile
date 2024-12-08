# WIND_CC_SYSROOT = $VSB_DIR

SRC=$(wildcard *.c)

CC=gcc
CFLAGS=-ggdb -g3 -Wall -Wextra -pedantic -fsanitize=address -std=c2x
LDFLAGS=-lfftw3 -lm

all:
	$(foreach file, $(SRC), $(CC) $(CFLAGS) $(LDFLAGS) -o $(subst .c,,$(file)) $(file);)

clean:
	$(foreach file, $(SRC), rm $(subst .c,,$(file));)

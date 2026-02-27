# WIND_CC_SYSROOT = $VSB_DIR

SRC=$(wildcard *.c)

CC=wr-cc
CFLAGS=-dkm -ggdb -g3 -Wall #-Wextra -pedantic 

all:
	$(foreach file, $(SRC), $(CC) $(CFLAGS) $(INC) -o $(subst .c,.out,$(file)) $(file);)

clean:
	rm -f *.out

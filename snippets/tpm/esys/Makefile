CC=cc
CFLAGS=-fsanitize=address -ggdb -g3 -Wall -Wextra -pedantic
LDFLAGS=`pkg-config --cflags --libs openssl tss2-esys tss2-sys tss2-tctildr tss2-rc`

SRC=$(wildcard *.c)

all:
	$(foreach file, $(SRC), $(CC) $(CFLAGS) -o $(subst .c,.elf,$(file)) $(file) $(UTIL) $(LDFLAGS);)

clean:
	rm -f *.elf

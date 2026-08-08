SRC=$(wildcard *.c)

CC=cc
CFLAGS=-ggdb -g3 -Wall -Wextra -pedantic -fsanitize=address $(shell pkg-config --cflags --libs openssl glib-2.0)

all:
	$(foreach file, $(SRC), $(CC) $(CFLAGS) $(INC) -o $(subst .c,.elf,$(file)) $(file);)

clean:
	rm -f *.elf

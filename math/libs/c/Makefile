CFLAGS=-fsanitize=address -Wall -Wextra -pedantic -std=c11 $(shell sdl2-config --cflags)
LDFLAGS=-lm $(shell sdl2-config --libs)

SRC=linalg.c sdf.c

all:
	cc $(CFLAGS) -o test-linalg test-linalg.c $(SRC) $(LDFLAGS)
	cc $(CFLAGS) -o test-sdf test-sdf.c $(SRC) $(LDFLAGS)

clean:
	rm -f test-linalg
	rm -f test-sdf
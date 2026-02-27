CFLAGS=-Wall -Wextra -pedantic
LDFLAGS=-lm

all:
	$(CC) -o pwmau vio.c util.c pwmau.c $(CFLAGS) $(LDFLAGS)
	$(CC) -o pwmaudma vio.c util.c pwmaudma.c $(CFLAGS) $(LDFLAGS)

clean:
	rm -f pwmau pwmaudma

format:
	clang-format -i *.c *.h

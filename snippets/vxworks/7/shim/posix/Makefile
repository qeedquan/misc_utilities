CC=cc
CFLAGS=-std=c2x -Wall -pedantic -ggdb -g3 -fsanitize=address -lm -lpthread -lrt
INC=-I.

all:
	$(CC) $(INC) $(CFLAGS) -c *.c
	$(AR) rcs libvxposixshim.a *.o

clean:
	rm -f *.o *.a

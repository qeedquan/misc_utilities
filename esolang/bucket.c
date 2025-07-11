// https://codegolf.stackexchange.com/questions/189661/bucket-interpreter

/*

Create a program that interprets the programming language Bucket.

Bucket works on two buckets: the first can hold A and the second can hold B units of liquid. The things you can do with these buckets are:

f: fill bucket A

F: fill bucket B

e: empty bucket A

E: empty bucket B

p: pour units of liquid from A to B until one is empty and the other is full, whichever happens first

P: the command p, but from B to A

o: output the value of bucket A

O: output the value of bucket B

These examples assume A is less than or equal to B in the tuple (A,B), which is the main tuple determining which variant of the language is to be interpreted. A is the lesser-or-equal value of a bucket here and B is the larger-or-equal value: substitute as you need.

Your program should ask for 3 inputs:

    the element x in the tuple (x,y):
    the element y in (x,y):
    the program to interpret.

The tuple (x,y) determines which variant of the language Bucket is interpreted.

As a general rule, make sure the first two inputs can range from 0 to 2,147,483,647.

Any characters in the third input other than fFeEpPoO do not need to be handled. Assume no characters other than these will be in the source.

Any commands equal to pouring more liquid in any bucket than it can hold do not need to be handled.

Assume the program will never try to pour liquid from an empty bucket.

*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define min(a, b) (((a) < (b)) ? (a) : (b))

typedef struct {
	const char *code;
	const char *pc;
	unsigned long ra, rb;
	unsigned long a, b;
} Bucket;

void
bucketreset(Bucket *bt)
{
	bt->pc = bt->code;
	bt->a = bt->ra;
	bt->b = bt->rb;
}

void
bucketload(Bucket *bt, const char *code, unsigned long a, unsigned long b)
{
	bt->code = code;
	bt->ra = a;
	bt->rb = b;
	bucketreset(bt);
}

void
bucketrun(Bucket *bt)
{
	unsigned long t;

	for (;;) {
		switch (*bt->pc++) {
		case '\0':
			return;
		case 'f':
			bt->a = bt->ra;
			break;
		case 'F':
			bt->b = bt->rb;
			break;
		case 'e':
			bt->a = 0;
			break;
		case 'E':
			bt->b = 0;
			break;
		case 'p':
			t = min(bt->a, bt->rb - bt->b);
			bt->a -= t;
			bt->b += t;
			break;
		case 'P':
			t = min(bt->b, bt->ra - bt->a);
			bt->b -= t;
			bt->a += t;
			break;
		case 'o':
			printf("A: %lu\n", bt->a);
			break;
		case 'O':
			printf("B: %lu\n", bt->b);
			break;
		}
	}
}

void
usage(void)
{
	fprintf(stderr, "usage: bucket code <a> <b>\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	unsigned long a, b;
	const char *code;
	Bucket bt;

	if (argc < 2)
		usage();

	code = argv[1];

	a = b = ULONG_MAX;
	if (argc >= 3)
		a = strtoul(argv[2], NULL, 0);
	if (argc >= 4)
		b = strtoul(argv[3], NULL, 0);

	bucketload(&bt, code, a, b);
	bucketrun(&bt);
	return 0;
}

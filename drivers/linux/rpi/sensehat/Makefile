CFLAGS=-Wall -Wextra -pedantic
LDFLAGS = -li2c -lm

all:
	cc $(CFLAGS) -o test_fb_rand test_fb_rand.c fb.c joy.c util.c $(LDFLAGS)
	cc $(CFLAGS) -o test_sense test_sense.c lps25h.c hts221.c lsm9ds1.c util.c $(LDFLAGS)
	cc $(CFLAGS) -o test_stick test_stick.c joy.c util.c $(LDFLAGS)
	cc $(CFLAGS) -o test_whoami test_whoami.c ui2c.c util.c $(LDFLAGS)

format:
	clang-format -i *.c *.h

clean:
	rm -f test_fb_rand test_sense test_stick test_whoami

// https://www.cs.dartmouth.edu/~doug/sieve/sieve.pdf
#include <stdio.h>
#include <unistd.h>

void
source(void)
{
	int n;
	for (n = 2;; n++)
		write(1, &n, sizeof(n));
}

void
cull(int p)
{
	int n;
	for (;;) {
		read(0, &n, sizeof(n));
		if (n % p != 0)
			write(1, &n, sizeof(n));
	}
}

void
redirect(int k, int pd[2])
{
	close(k);
	dup(pd[k]);
	close(pd[0]);
	close(pd[1]);
}

void
sink(void)
{
	int pd[2], p;
	for (;;) {
		read(0, &p, sizeof(p));
		printf("%d\n", p);
		fflush(stdout);
		pipe(pd);
		if (fork()) {
			redirect(0, pd);
			continue;
		} else {
			redirect(1, pd);
			cull(p);
		}
	}
}

int
main(void)
{
	int pd[2];
	pipe(pd);
	if (fork()) {
		redirect(0, pd);
		sink();
	} else {
		redirect(1, pd);
		source();
	}
	return 0;
}

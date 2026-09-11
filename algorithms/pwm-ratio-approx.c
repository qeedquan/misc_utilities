// algorithm described in https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf

// problem:
// we want to generate a value with duty cycle that is expressed
// as a ratio of N/M

// solution: to send value with N/M within period of M cycles,
// output should be 1 for N cycles and 0 for (M-N) cycles.
// The desired sequence should be spread out as evenly as possible
// The solution is to take the simplify fraction N/M, ie, divide by the
// greatest common divisor. So a duty cycles of 4/8 is best approximated
// by 1/2 over time, the algorithm below does this only using adds/subs

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int c;
	int n, m;
} PWM;

void
pwminit(PWM *p, int n, int m)
{
	p->c = 0;
	p->n = n;
	p->m = m;
}

int
pwmgen(PWM *p)
{
	p->c += p->n;
	if (p->c >= p->m) {
		p->c -= p->m;
		return 1;
	}
	return 0;
}

int
main(int argc, char *argv[])
{
	PWM p;
	int i, n, m;

	n = 4;
	m = 8;
	if (argc == 3) {
		n = atoi(argv[1]);
		m = atoi(argv[2]);
	}

	pwminit(&p, n, m);
	for (i = 0; i < 1024 * m; i++) {
		printf("%d ", pwmgen(&p));
		if ((i & 7) == 7)
			printf("\n");
	}

	return 0;
}

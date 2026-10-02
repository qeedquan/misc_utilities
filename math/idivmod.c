#include <stdio.h>

// http://www.chrishecker.com/images/9/97/Gdmtex2.pdf
void
floordivmod(int n, int d, int *f, int *m)
{
	if (d == 0) {
		*f = 0;
		*m = 0;
		return;
	}

	if (d < 0) {
		n = -n;
		d = -d;
	}

	if (n >= 0) {
		*f = n / d;
		*m = n % d;
	} else {
		*f = -((-n) / d);
		*m = (-n) % d;
		if (*m) {
			(*f)--;
			*m = d - *m;
		}
	}
}

void
floordivmodc(int n, int d, int *f, int *m)
{
	if (d == 0) {
		*f = 0;
		*m = 0;
		return;
	}

	if (d < 0) {
		n = -n;
		d = -d;
	}
	*f = n / d;
	*m = n % d;
	if (*m < 0) {
		(*f)--;
		*m += d;
	}
}

int
main(void)
{
	int n, d, f1, m1, f2, m2;
	int start, end;

	start = -10000;
	end = 10000;
	for (n = start; n <= end; n++) {
		for (d = start; d < end; d++) {
			floordivmod(n, d, &f1, &m1);
			floordivmodc(n, d, &f2, &m2);
			if (f1 != f2 || m1 != m2)
				printf("%d %d %d %d %d %d\n", n, d, f1, m1, f2, m2);
		}
	}
	return 0;
}

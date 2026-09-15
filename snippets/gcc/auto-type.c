#include <stdio.h>

#define P(x) printf("%s: %zu\n", #x, sizeof(x))

int
main(void)
{
	__auto_type s = "hello";
	__auto_type f = 1.0f;
	__auto_type d = 1.0;
	__auto_type c = (char)'1';
	__auto_type h = (short)1;
	__auto_type i = 1;
	__auto_type ll = 10ll;
	__auto_type n = NULL;

	P(s);
	P(f);
	P(d);
	P(c);
	P(h);
	P(i);
	P(ll);
	P(n);

	return 0;
}

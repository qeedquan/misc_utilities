#define max(a, b) ({ typeof (a) _a = (a); typeof (b) _b = (b); _a > _b ? _a : _b; })
#define min(a, b) ({ typeof (a) _a = (a); typeof (b) _b = (b); _a < _b ? _a : _b; })

int
f(void)
{
	return 1;
}

int
g(void)
{
	return 0;
}

int
main(void)
{
	max(f(), g());
	return 0;
}

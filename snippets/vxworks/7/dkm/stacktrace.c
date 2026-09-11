#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <taskLib.h>
#include <trcLib.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

typedef struct Context Context;
typedef void (*Func)(Context *, int, int);

struct Context {
	Func funcs[26];
};

static void
dump_stack(void)
{
	TASK_ID tid;
	REG_SET regs;

	tid = taskIdSelf();
	if (taskRegsGet(tid, &regs) != OK) {
		printf("Failed to get registers\n");
		return;
	}
	trcStack(&regs, NULL, tid);
	printf("\n");
}

static void
recurse(Context *c, int n, int d)
{
	if (n >= d) {
		dump_stack();
		printf("Reached terminal, sleeping\n");
		sleep(5);
		printf("Returning\n");
		return;
	}

	c->funcs[rand() % nelem(c->funcs)](c, n + 1, d);
}

#define N(fn) f_##fn
#define F(fn)                                        \
	static void N(fn)(Context * c, int n, int d) \
	{                                            \
		printf("%s()\n", __func__);          \
		recurse(c, n, d);                    \
	}

F(a)
F(b)
F(c)
F(d)
F(e)
F(f)
F(g)
F(h)
F(i)
F(j)
F(k)
F(l)
F(m)
F(n)
F(o)
F(p)
F(q)
F(r)
F(s)
F(t)
F(u)
F(v)
F(w)
F(x)
F(y)
F(z)

static void
init_context(Context *c)
{
	static const Func ftab[] = {
	    N(a),
	    N(b),
	    N(c),
	    N(d),
	    N(e),
	    N(f),
	    N(g),
	    N(h),
	    N(i),
	    N(j),
	    N(k),
	    N(l),
	    N(m),
	    N(n),
	    N(o),
	    N(p),
	    N(q),
	    N(r),
	    N(s),
	    N(t),
	    N(u),
	    N(v),
	    N(w),
	    N(x),
	    N(y),
	    N(z),
	};
	size_t i;

	for (i = 0; i < nelem(c->funcs); i++)
		c->funcs[i] = ftab[i];
}

void
test_stack_trace(void)
{
	Context c[1];

	srand(time(NULL));
	init_context(c);
	recurse(c, 0, 50);
}

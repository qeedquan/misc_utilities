#include <stdio.h>
#include <stdatomic.h>
#include <stdint.h>
#include <threads.h>
#include <limits.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

#define TP(T)                                                                                                             \
	do {                                                                                                              \
		T v;                                                                                                      \
		atomic_init(&v, 0x1);                                                                                     \
		atomic_store(&v, 0xff);                                                                                   \
		atomic_fetch_or(&v, 0xff);                                                                                \
		atomic_fetch_and(&v, 0x3f);                                                                               \
		atomic_fetch_add(&v, 0x3f);                                                                               \
		atomic_fetch_sub(&v, 0xf);                                                                                \
		atomic_fetch_xor(&v, 0xff);                                                                               \
		printf("%s: %zu value: %d lockfree: %d\n", #T, sizeof(T), (int)atomic_load(&v), atomic_is_lock_free(&v)); \
	} while (0);

void
types(void)
{
	atomic_bool abool;
	printf("atomic_bool %zu lockfree: %d\n", sizeof(atomic_bool), atomic_is_lock_free(&abool));

	TP(atomic_char);
	TP(atomic_uchar);
	TP(atomic_schar);
	TP(atomic_short);
	TP(atomic_ushort);
	TP(atomic_int);
	TP(atomic_uint);
	TP(atomic_long);
	TP(atomic_ulong);
	TP(atomic_llong);
	TP(atomic_ullong);
	TP(atomic_char16_t);
	TP(atomic_char32_t);
	TP(atomic_wchar_t);
	TP(atomic_int_least8_t);
	TP(atomic_uint_least8_t);
	TP(atomic_int_least16_t);
	TP(atomic_uint_least16_t);
	TP(atomic_int_least32_t);
	TP(atomic_uint_least32_t);
	TP(atomic_int_least64_t);
	TP(atomic_uint_least64_t);
	TP(atomic_int_fast8_t);
	TP(atomic_uint_fast8_t);
	TP(atomic_int_fast16_t);
	TP(atomic_uint_fast16_t);
	TP(atomic_int_fast32_t);
	TP(atomic_uint_fast32_t);
	TP(atomic_int_fast64_t);
	TP(atomic_uint_fast64_t);
	TP(atomic_intptr_t);
	TP(atomic_uintptr_t);
	TP(atomic_size_t);
	TP(atomic_ptrdiff_t);
	TP(atomic_intmax_t);
	TP(atomic_uintmax_t);
}

int
cas(void *arg)
{
	atomic_int *a;
	mtx_t *m;
	int i, x, y;
	void **l;

	l = (void **)arg;
	a = l[0];
	m = l[1];

	x = y = 0;
	for (i = 0; i < 10000000; i++) {
		if (atomic_compare_exchange_strong(a, a, i)) {
			x++;
		} else {
			y++;
		}
	}

	mtx_lock(m);
	printf("%d %d\n", x, y);
	mtx_unlock(m);
	return 0;
}

void
xchg(void)
{
	thrd_t thr[128];
	mtx_t mtx;
	atomic_int v;
	size_t i, j;
	void *a[2];

	v = 0;
	mtx_init(&mtx, mtx_plain);

	a[0] = &v;
	a[1] = &mtx;

	for (i = 0; i < nelem(thr); i++) {
		printf("%zu\n", i + 1);
		for (j = 0; j <= i; j++) {
			thrd_create(&thr[j], cas, a);
		}
		for (j = 0; j <= i; j++) {
			thrd_join(thr[j], NULL);
		}
		printf("\n");
	}
}

int
main(void)
{
	types();
	xchg();
	return 0;
}

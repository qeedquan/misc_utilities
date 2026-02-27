#include <cstdio>
#include <cstdint>
#include <unistd.h>

enum Errno
{
	E0,
	E1,
	E2,
};

struct Data
{
	void *buf;
	uint64_t len;
};

struct Far
{
	Data data;

	Far(void *buf, uint64_t buflen) __attribute__((section(".cl_far_constructor"))) __attribute__((ms_abi));
	void faze() __attribute__((section(".cl_far_faze"))) __attribute__((sysv_abi));
};

__attribute__((section(".f_load"))) __attribute__((ms_hook_prologue)) Errno load(Data *, int);
__attribute__((section(".f_quux"))) __attribute__((ms_abi)) int quux(const char *, const char *, int, int, float);

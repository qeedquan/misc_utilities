#include <string.h>
#include <libgen.h>
#include <err.h>
#include <openssl/engine.h>
#include <openssl/rand.h>

void
hexdump(const char *pfx, void *b, size_t n)
{
	unsigned char *p;
	size_t i;

	p = b;
	printf("%s\n", pfx);
	for (i = 0; i < n; i++) {
		printf("%x ", p[i]);
		if ((i & 15) == 15)
			printf("\n");
	}
	printf("\n");
}

void
register_engines(void)
{
	ENGINE *eg;
	const char *val;

	ENGINE_load_builtin_engines();
	eg = ENGINE_by_id("dynamic");
	if (!eg)
		goto out;

	val = getenv("OPENSSL_TPM_SO");
	if (val) {
		printf("OPENSSL_TPM_SO = %s\n", val);
		ENGINE_ctrl_cmd_string(eg, "SO_PATH", val, 0);
		ENGINE_ctrl_cmd_string(eg, "ID", "tpm2tss", 0);
		ENGINE_ctrl_cmd_string(eg, "LIST_ADD", "2", 2);
		if (!ENGINE_ctrl_cmd_string(eg, "LOAD", NULL, 0))
			printf("Failed to load TPM engine\n");
	}

	ENGINE_free(eg);
out:
	ENGINE_register_all_complete();
}

void
list_engines(void)
{
	ENGINE *eg;

	printf("Engine Listing\n");
	eg = ENGINE_get_first();
	while (eg) {
		printf("'%s' %s\n", ENGINE_get_id(eg), ENGINE_get_name(eg));
		eg = ENGINE_get_next(eg);
	}
	printf("\n");
}

int
test_rand(ENGINE *eg)
{
	unsigned char buf[128];
	const RAND_METHOD *m;
	int r;

	printf("Testing Random Generator\n");

	m = ENGINE_get_RAND(eg);
	if (!m) {
		printf("Failed to get random method\n");
		return -1;
	}

	r = RAND_set_rand_method(m);
	if (r != 1) {
		printf("Failed to set random method\n");
		return -1;
	}

	memset(buf, 0, sizeof(buf));
	RAND_seed("test", 4);
	RAND_bytes(buf, sizeof(buf));
	hexdump("Random Buffer", buf, sizeof(buf));

	return 0;
}

int
main(void)
{
	ENGINE *eg;
	unsigned flags;

	register_engines();
	list_engines();

	eg = ENGINE_by_id("tpm2tss");
	if (!eg)
		errx(1, "Failed to get TPM2 Engine");

	if (!ENGINE_init(eg))
		errx(1, "Failed to init engine");

	ENGINE_set_default(eg, ENGINE_METHOD_ALL);
	OpenSSL_add_all_algorithms();

	flags = ENGINE_get_table_flags();
	printf("Engine Flags: %X\n", flags);

	test_rand(eg);

	ENGINE_finish(eg);
	ENGINE_free(eg);

	return 0;
}

#include <crypto/internal/hash.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>

struct xorsum_ctx {
	u8 sum;
};

static int
xorsum_init(struct shash_desc *desc)
{
	struct xorsum_ctx *ctx;

	ctx = shash_desc_ctx(desc);
	ctx->sum = 0;
	return 0;
}

static int
xorsum_update(struct shash_desc *desc, const u8 *data, unsigned int len)
{
	struct xorsum_ctx *ctx;
	unsigned int i;

	ctx = shash_desc_ctx(desc);
	for (i = 0; i < len; i++)
		ctx->sum ^= data[i];
	return 0;
}

static int
xorsum_final(struct shash_desc *desc, u8 *out)
{
	struct xorsum_ctx *ctx;

	ctx = shash_desc_ctx(desc);
	out[0] = ctx->sum;
	return 0;
}

static struct shash_alg alg = {
    .digestsize = 1,
    .init = xorsum_init,
    .update = xorsum_update,
    .final = xorsum_final,
    .descsize = sizeof(struct xorsum_ctx),
    .base = {
        .cra_name = "xorsum",
        .cra_flags = CRYPTO_ALG_TYPE_SHASH,
        .cra_blocksize = 1,
        .cra_module = THIS_MODULE,
    },
};

static int __init
xorsum_mod_init(void)
{
	pr_info("xorsum: starting module\n");
	crypto_register_shash(&alg);
	return 0;
}

static void
xorsum_mod_exit(void)
{
	pr_info("xorsum: stopping module\n");
	crypto_unregister_shash(&alg);
}

module_init(xorsum_mod_init);
module_exit(xorsum_mod_exit);

MODULE_AUTHOR("Quan Tran");
MODULE_DESCRIPTION("XOR Sum");
MODULE_LICENSE("GPL");

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/random.h>
#include <linux/types.h>

static char *name = "user";
static int value = 1;
static bool accept = false;

module_param(name, charp, S_IRUGO);
module_param(value, int, S_IRUGO);
module_param(accept, bool, S_IRUGO);

MODULE_PARM_DESC(name, "The name to display");
MODULE_PARM_DESC(value, "The value to display");
MODULE_PARM_DESC(accept, "Acceptance");

static int __init
hello_init(void)
{
	int rnd;

	get_random_bytes(&rnd, sizeof(rnd));
	pr_info("Hello %s with value (%d, %d, %d)\n",
	        name, value, accept, rnd);
	return 0;
}

static void
hello_exit(void)
{
	int rnd;

	get_random_bytes(&rnd, sizeof(rnd));
	pr_info("Goodbye %s with value (%d, %d, %d)\n",
	        name, value, accept, rnd);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Quan Tran");
MODULE_DESCRIPTION("Hello World");
MODULE_LICENSE("GPL");

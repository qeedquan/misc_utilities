#include <linux/kallsyms.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#if !(defined(__x86_64__) || defined(__i386__))
#error Module only supported on x86 platform
#endif

static asmlinkage long (*original_sys_open)(const char __user *, int, umode_t);
static asmlinkage unsigned long **sys_call_table;

static void
disable_write_protection(void)
{
	write_cr0(read_cr0() & ~0x10000);
}

static void
enable_write_protection(void)
{
	write_cr0(read_cr0() | 0x10000);
}

static asmlinkage long
ext_open(const char __user *name, int flags, umode_t mode)
{
	if (strstr(name, "."))
		pr_info("ext: open(name = %s, flags = %x, mode = %x)\n", name, flags, mode);
	return original_sys_open(name, flags, mode);
}

static int __init
ext_init(void)
{
	pr_info("ext: starting module\n");

	sys_call_table = (void *)kallsyms_lookup_name("sys_call_table");
	if (sys_call_table == NULL) {
		pr_info("ext: failed to find sys_call_table address\n");
		return -EINVAL;
	}

	disable_write_protection();
	original_sys_open = (void *)sys_call_table[__NR_open];
	sys_call_table[__NR_open] = (void *)ext_open;
	enable_write_protection();

	return 0;
}

static void
ext_exit(void)
{
	pr_info("ext: stopping module\n");
	if (sys_call_table != NULL) {
		disable_write_protection();
		sys_call_table[__NR_open] = (void *)original_sys_open;
		enable_write_protection();
	}
}

module_init(ext_init);
module_exit(ext_exit);

MODULE_AUTHOR("Quan Tran");
MODULE_DESCRIPTION("Watches open calls for filenames with extensions");
MODULE_LICENSE("GPL");

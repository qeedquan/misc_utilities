#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#ifdef pr_fmt
#undef pr_fmt
#endif
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

static struct proc_dir_entry *proc;
static atomic_t user_value = ATOMIC_INIT(0);

static ssize_t
pfmt_proc_read(struct file *filp, char __user *buf, size_t count, loff_t *offp)
{
	char str[128];
	int val, n;

	val = atomic_read(&user_value);
	n = 0;
	n += snprintf(str + n, sizeof(str) - n, "%d\n", val);
	n += snprintf(str + n, sizeof(str) - n, "%04d\n", val);
	n += snprintf(str + n, sizeof(str) - n, "%-4d\n", val);
	n += snprintf(str + n, sizeof(str) - n, "%x\n", val);
	n += snprintf(str + n, sizeof(str) - n, "%4x\n", val);
	n += snprintf(str + n, sizeof(str) - n, "%X\n", val);
	n += snprintf(str + n, sizeof(str) - n, "%010X\n", val);
	n += snprintf(str + n, sizeof(str) - n, "%#x\n", val);
	n += snprintf(str + n, sizeof(str) - n, "%p\n", &val);
	n += snprintf(str + n, sizeof(str) - n, "%pa\n", &val);
	n += snprintf(str + n, sizeof(str) - n, "%-c\n", val);
	if (n > count)
		n = count;
	copy_to_user(buf, str, n);

	if (*offp != 0)
		return 0;

	*offp += n;
	return n;
}

static ssize_t
pfmt_proc_write(struct file *filp, const char __user *buf, size_t count, loff_t *offp)
{
	long val;

	kstrtol_from_user(buf, count, 0, &val);
	atomic_set(&user_value, val);
	return count;
}

static struct file_operations pfmt_proc_ops = {
    .owner = THIS_MODULE,
    .read = pfmt_proc_read,
    .write = pfmt_proc_write,
};

static int __init
pfmt_init(void)
{
	pr_info("starting module\n");

	proc = proc_create("pfmt_info", 0, NULL, &pfmt_proc_ops);
	if (proc == NULL)
		pr_warn("failed to create proc entry\n");

	return 0;
}

static void
pfmt_exit(void)
{
	pr_info("stopping module\n");
	if (proc != NULL)
		remove_proc_entry("pfmt_info", NULL);
}

module_init(pfmt_init);
module_exit(pfmt_exit);

MODULE_AUTHOR("Quan Tran");
MODULE_DESCRIPTION("print format test");
MODULE_LICENSE("GPL");

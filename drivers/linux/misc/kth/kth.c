#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

static struct task_struct *task;
static struct proc_dir_entry *proc;
static atomic_t counter = ATOMIC_INIT(0);

static ssize_t
kth_proc_read(struct file *filp, char __user *buf, size_t count, loff_t *offp)
{
	char str[16];
	int val, n, nw;

	val = atomic_read(&counter);
	n = snprintf(str, sizeof(str), "%d\n", val);
	if (n > count)
		n = count;

	nw = copy_to_user(buf, str, n);
	n -= nw;

	if (*offp != 0)
		return 0;

	*offp += n;
	return n;
}

static int
kth_runner(void *data)
{
	pr_info("kth: runner thread starting\n");
	while (!kthread_should_stop()) {
		atomic_inc(&counter);
		msleep_interruptible(1000);
	}
	pr_info("kth: runner thread exiting\n");
	return 0;
}

static struct proc_ops kth_proc_ops = {
    .proc_read = kth_proc_read,
};

static int __init
kth_init(void)
{
	pr_info("kth: starting module\n");

	task = kthread_run(kth_runner, NULL, "kth_test");
	if (task == NULL)
		pr_warn("kth: failed to create runner task\n");

	proc = proc_create("kth_info", 0, NULL, &kth_proc_ops);
	if (proc == NULL)
		pr_warn("kth: failed to create proc entry\n");

	return 0;
}

static void
kth_exit(void)
{
	pr_info("kth: stopping module\n");

	if (task != NULL)
		kthread_stop(task);

	if (proc != NULL)
		remove_proc_entry("kth_info", NULL);
}

module_init(kth_init);
module_exit(kth_exit);

MODULE_AUTHOR("Quan Tran");
MODULE_DESCRIPTION("kthread test");
MODULE_LICENSE("GPL");

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

static struct task_struct *task[16];
static struct proc_dir_entry *proc;
static DECLARE_COMPLETION(completed);

static ssize_t
cpl_proc_write(struct file *filp, const char __user *buf, size_t count, loff_t *offp)
{
	pr_info("stopping all runners\n");
	complete_all(&completed);
	return count;
}

static int
cpl_runner(void *data)
{
	int id;

	id = (int)((uintptr_t)data);
	pr_info("runner %d waiting\n", id);
	wait_for_completion(&completed);
	pr_info("runner %d exiting\n", id);
	do_exit(0);
}

static struct proc_ops cpl_proc_ops = {
    .proc_write = cpl_proc_write,
};

static int __init
cpl_init(void)
{
	char name[32];
	size_t i;

	pr_info("starting module\n");

	for (i = 0; i < ARRAY_SIZE(task); i++) {
		snprintf(name, sizeof(name), "cpl_runner%zu", i);
		task[i] = kthread_run(cpl_runner, (void *)((uintptr_t)i), name);
		if (task[i] == NULL)
			pr_warn("failed to create runner task %zu\n", i);
	}

	proc = proc_create("cpl_info", 0, NULL, &cpl_proc_ops);
	if (proc == NULL)
		pr_warn("failed to create proc entry\n");

	return 0;
}

static void
cpl_exit(void)
{
	pr_info("stopping module\n");
	
	complete_all(&completed);

	if (proc != NULL)
		remove_proc_entry("cpl_info", NULL);
}

module_init(cpl_init);
module_exit(cpl_exit);

MODULE_AUTHOR("Quan Tran");
MODULE_DESCRIPTION("Completion test");
MODULE_LICENSE("GPL");

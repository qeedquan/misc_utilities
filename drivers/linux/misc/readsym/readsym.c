#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/cdev.h>
#include <linux/kallsyms.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#ifdef pr_fmt
#undef pr_fmt
#endif
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#define RK_MAJOR 100

struct rk_sym {
	char name[KSYM_NAME_LEN];
	int type;
	void *addr;
	unsigned long size;
};

struct rk_iter {
	loff_t pos;
};

static struct cdev cdev;
static struct rk_sym *syms;
static atomic_t symlen;
static int maxsyms = 256;

module_param(maxsyms, int, S_IRUGO);

MODULE_PARM_DESC(maxsyms, "Maximum number of symbols");

static void
reset_iter(struct rk_iter *iter, loff_t new_pos)
{
	iter->pos = new_pos;
}

static int
update_iter(struct rk_iter *iter, loff_t pos)
{
	if (pos >= atomic_read(&symlen))
		return 0;
	if (pos != iter->pos)
		reset_iter(iter, pos);
	iter->pos++;
	return 1;
}

static void *
s_next(struct seq_file *m, void *p, loff_t *pos)
{
	(*pos)++;
	if (!update_iter(m->private, *pos))
		return NULL;
	return p;
}

static void *
s_start(struct seq_file *m, loff_t *pos)
{
	if (!update_iter(m->private, *pos))
		return NULL;
	return m->private;
}

static void
s_stop(struct seq_file *m, void *p)
{
}

static void
s_show_type(struct seq_file *m, int type, void *addr, unsigned long size)
{
	unsigned long i;

	seq_printf(m, "type: %c size: %lu addr %p value: ", type, size, addr);
	if (!virt_addr_valid(addr) || !virt_addr_valid(addr + size)) {
		seq_printf(m, "(invalid address)");
		return;
	}
	switch (type) {
	case 'b':
		for (i = 0; i < size; i++)
			seq_printf(m, "%x ", *((unsigned char *)addr));
		break;
	case 'c':
		seq_printf(m, "%d", *((char *)addr));
		break;
	case 'C':
		seq_printf(m, "%d", *((unsigned char *)addr));
		break;
	case 'i':
		seq_printf(m, "%d", *((int *)addr));
		break;
	case 'I':
		seq_printf(m, "%u", *((unsigned int *)addr));
		break;
	case 'l':
		seq_printf(m, "%ld", *((long *)addr));
		break;
	case 'L':
		seq_printf(m, "%lu", *((unsigned long *)addr));
		break;
	case 'q':
		seq_printf(m, "%lld", *((long long *)addr));
		break;
	case 'Q':
		seq_printf(m, "%llu", *((unsigned long long *)addr));
		break;
	}
}

static int
s_show(struct seq_file *m, void *p)
{
	struct rk_iter *iter;
	struct rk_sym *y;

	iter = m->private;

	y = &syms[iter->pos - 1];
	seq_printf(m, "%lld name: \"%s\" ", iter->pos - 1, y->name);
	s_show_type(m, y->type, y->addr, y->size);
	seq_printf(m, "\n");
	return 0;
}

static const struct seq_operations rk_sops = {
    .start = s_start,
    .next = s_next,
    .stop = s_stop,
    .show = s_show,
};

static int
rk_open(struct inode *inode, struct file *file)
{
	struct rk_iter *iter;

	iter = __seq_open_private(file, &rk_sops, sizeof(*iter));
	if (!iter)
		return -ENOMEM;
	reset_iter(iter, 0);
	return 0;
}

static int
check_type(const char *str, int *type, unsigned long *size)
{
	*type = str[0];
	switch (str[0]) {
	case 'b':
		if (!kstrtoul(str + 1, 0, size))
			return 0;
		break;
	case 'c':
		*size = sizeof(char);
		break;
	case 'C':
		*size = sizeof(unsigned char);
		break;
	case 'i':
		*size = sizeof(int);
		break;
	case 'I':
		*size = sizeof(unsigned int);
		break;
	case 'l':
		*size = sizeof(long);
		break;
	case 'L':
		*size = sizeof(unsigned long);
		break;
	case 'q':
		*size = sizeof(long long);
		break;
	case 'Q':
		*size = sizeof(unsigned long long);
		break;
	default:
		return 0;
	}

	return 1;
}

static ssize_t
rk_write(struct file *file, const char __user *ubuf, size_t len, loff_t *offp)
{
	char buf[KSYM_NAME_LEN + 32], name[KSYM_NAME_LEN], *ptr, *str;
	unsigned long long addr;
	unsigned long size;
	int type, pos;

	pos = atomic_read(&symlen);
	if (pos >= maxsyms)
		return -ENOMEM;
	if (len >= sizeof(buf))
		return -EINVAL;

	if (copy_from_user(buf, ubuf, len))
		return -EFAULT;

	if ((ptr = strchr(buf, '\n')))
		*ptr = '\0';
	ptr = buf;

	if (!(str = strsep(&ptr, " ")))
		return -EINVAL;

	addr = kallsyms_lookup_name(str);
	if (!addr) {
		if (kstrtoull(ptr, 0, &addr))
			return -EINVAL;
	}
	if (!addr)
		return -EINVAL;
	strscpy(name, str, sizeof(name));

	if (!(str = strsep(&ptr, " ")))
		return -EINVAL;
	if (!check_type(str, &type, &size))
		return -EINVAL;

	strscpy(syms[pos].name, name, sizeof(name));
	syms[pos].addr = (void *)((uintptr_t)addr);
	syms[pos].type = type;
	syms[pos].size = size;
	atomic_inc(&symlen);

	return len;
}

static const struct file_operations rk_fops = {
    .owner = THIS_MODULE,
    .open = rk_open,
    .write = rk_write,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = seq_release_private,
};

static int __init
rk_init(void)
{
	int err;

	pr_info("Starting module\n");

	if (maxsyms <= 0)
		return -EINVAL;

	syms = kcalloc(maxsyms, sizeof(*syms), GFP_KERNEL);
	if (!syms)
		return -ENOMEM;

	atomic_set(&symlen, 0);

	err = register_chrdev_region(MKDEV(RK_MAJOR, 0), 1, "readsym");
	if (err)
		return err;
	cdev_init(&cdev, &rk_fops);
	cdev_add(&cdev, MKDEV(RK_MAJOR, 0), 1);

	return 0;
}

static void
rk_exit(void)
{
	pr_info("Stopping module\n");

	kfree(syms);
	cdev_del(&cdev);
	unregister_chrdev_region(MKDEV(RK_MAJOR, 0), 1);
}

module_init(rk_init);
module_exit(rk_exit);

MODULE_AUTHOR("Quan Tran");
MODULE_DESCRIPTION("Read symbols in a horribly unsafe manner for debugging");
MODULE_LICENSE("GPL");

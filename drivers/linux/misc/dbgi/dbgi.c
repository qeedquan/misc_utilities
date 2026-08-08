#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>

static const char *ftab[] = {
    "cat",
    "dog",
    "chicken",
    "sheep",
    "wolf",
    "montaru",
    "watarou",
    "kookoo",
    "penguin",
    "kangaroo",
    "fish",
    "shark",
    "whale",
    "galactic octopus",
    "moby deacon",
};

static struct dentry *file[ARRAY_SIZE(ftab)];
static struct dentry *directory;

static ssize_t
dbgi_read(struct file *filp, char __user *buf, size_t len, loff_t *offp)
{
	struct qstr *qp;
	ssize_t n;

	qp = &filp->f_path.dentry->d_name;
	n = simple_read_from_buffer(buf, len, offp, qp->name, qp->len);
	if (n > 0)
		return n;

	if (*offp == qp->len && len > 0) {
		n = copy_to_user(buf, "\n", 1);
		if (n != 0)
			n = 0;
		else
			n = 1;
		*offp += n;
	}

	return n;
}

static const struct file_operations dbgi_fops = {
    .owner = THIS_MODULE,
    .read = dbgi_read,
};

static int __init
dbgi_init(void)
{
	size_t i;

	pr_info("dbgi: starting module\n");

	directory = debugfs_create_dir("dbgi", NULL);
	if (directory == NULL)
		return -ENOENT;

	for (i = 0; i < ARRAY_SIZE(ftab); i++) {
		file[i] = debugfs_create_file(ftab[i], 0644, directory, NULL, &dbgi_fops);
		if (file[i] == NULL) {
			debugfs_remove_recursive(directory);
			return -ENOENT;
		}
	}

	return 0;
}

static void
dbgi_exit(void)
{
	pr_info("dbgi: stopping module\n");
	debugfs_remove_recursive(directory);
}

module_init(dbgi_init);
module_exit(dbgi_exit);

MODULE_AUTHOR("Quan Tran");
MODULE_DESCRIPTION("debugfs test");
MODULE_LICENSE("GPL");

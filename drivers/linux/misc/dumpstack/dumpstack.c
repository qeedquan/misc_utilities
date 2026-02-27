#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#ifdef pr_fmt
#undef pr_fmt
#endif
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#define DK_MAJOR 100

static struct cdev cdev;

#define DUMP_TRUCK()                 \
	pr_info("%s:%d\n", __func__, __LINE__); \
	dump_stack()

static int
dk_open(struct inode *inode, struct file *file)
{
	DUMP_TRUCK();
	return 0;
}

static ssize_t
dk_read(struct file *file, char __user *ubuf, size_t len, loff_t *offp)
{
	DUMP_TRUCK();
	return 0;
}

static ssize_t
dk_write(struct file *file, const char __user *ubuf, size_t len, loff_t *offp)
{
	DUMP_TRUCK();
	return len;
}

static loff_t
dk_lseek(struct file *file, loff_t pos, int whence)
{
	DUMP_TRUCK();
	return 0;
}

static int
dk_release(struct inode *inode, struct file *file)
{
	DUMP_TRUCK();
	return 0;
}

static long
dk_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	DUMP_TRUCK();
	return 0;
}

static int
dk_mmap(struct file *file, struct vm_area_struct *vma)
{
	DUMP_TRUCK();
	return 0;
}

static int
dk_fsync(struct file *file, loff_t start, loff_t end, int datasync)
{
	DUMP_TRUCK();
	return 0;
}

static const struct file_operations dk_fops = {
    .owner = THIS_MODULE,
    .open = dk_open,
    .write = dk_write,
    .read = dk_read,
    .llseek = dk_lseek,
    .release = dk_release,
    .unlocked_ioctl = dk_ioctl,
    .mmap = dk_mmap,
    .fsync = dk_fsync,
};

static int __init
dk_init(void)
{
	int err;

	DUMP_TRUCK();

	err = register_chrdev_region(MKDEV(DK_MAJOR, 0), 1, "dumpstack");
	if (err)
		return err;
	cdev_init(&cdev, &dk_fops);
	cdev_add(&cdev, MKDEV(DK_MAJOR, 0), 1);

	return 0;
}

static void
dk_exit(void)
{
	DUMP_TRUCK();

	cdev_del(&cdev);
	unregister_chrdev_region(MKDEV(DK_MAJOR, 0), 1);
}

module_init(dk_init);
module_exit(dk_exit);

MODULE_AUTHOR("Quan Tran");
MODULE_DESCRIPTION("Dump stack information on various calls");
MODULE_LICENSE("GPL");

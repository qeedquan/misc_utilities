#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/slab.h>
#include <linux/ioport.h>

#ifdef pr_fmt
#undef pr_fmt
#endif
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

static struct dentry *file;
static struct dentry *directory;

// newer raspberry pi uses device tree to specify everything
// we traverse the tree to find register locations and print them out (unsafely)
// because other device drivers are using these registers

// https://www.raspberrypi.org/documentation/configuration/config-txt/overclocking.md
// gives all the clock frequencies for various models

// /sys/kernel/debug/clk/clk_summary contains all the register clocks for the pi
// each clock can have a parent relationship in which case the child clock will usually
// be a divider of the parent clock

// i2c derives its clock from the parent pll with a divider
// the device tree has a field clock_frequency to specify the frequency the i2c
// device and who its parent clock is. 
// It should run at and then use the parent clock to figure out the dividing factor
// parent_clock_frequency / i2c_clock_frequency = divider which is set in the DIV register
// ex: by default i2c wants to run at 100 khz and parent clock is the vpu (video core clock)
// and that runs at 400 mhz (rpi3)
// so have 400 mhz / 100 khz = 4000 (0xfa0) for divisor
static int
dr_dump_i2c(const char *name, char *buf, size_t len)
{
	static const char *reg_names[] = {
		"C", "S", "DLEN", "A", "FIFO", "DIV", "DEL", "CLKT",
	};
	
	struct device_node *np;
	struct resource res;
	size_t i, nr;
	int rc;
	void __iomem *regs;

	np = of_find_node_by_path(name);
	nr = 0;
	nr += scnprintf(buf + nr, len - nr, "%s %p {\n", name, np);
	if (np) {
		rc = of_address_to_resource(np, 0, &res);
		if (rc)
			goto out;
		
		nr += scnprintf(buf + nr, len - nr, "\treg: %llx-%llx %llx {\n", res.start, res.end, resource_size(&res));

		regs = of_iomap(np, 0);
		if (!regs)
			goto out;

		for (i = 0; i < ARRAY_SIZE(reg_names); i++)
			nr += scnprintf(buf + nr, len - nr, "\t\t%s: %x\n", reg_names[i], readl(regs+4*i));
		nr += scnprintf(buf + nr, len - nr, "\t}\n");
		
		iounmap(regs);
	}

out:
	nr += scnprintf(buf + nr, len + nr, "}\n");

	return nr;
}

static size_t
dr_dump(char *buf, size_t len)
{
	size_t nr;

	nr = 0;
	nr += dr_dump_i2c("i2c0", buf + nr, len - nr);
	nr += dr_dump_i2c("i2c1", buf + nr, len - nr);
	nr += dr_dump_i2c("i2c2", buf + nr, len - nr);
	return nr;
}

static ssize_t
dr_read(struct file *filp, char __user *ubuf, size_t ulen, loff_t *offp)
{
	char *buf;
	size_t len;
	ssize_t nr;

	len = 4096;
	buf = kmalloc(len, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	len = dr_dump(buf, len);
	nr = simple_read_from_buffer(ubuf, ulen, offp, buf, len);
	kfree(buf);

	return nr;
}

static const struct file_operations dr_fops = {
    .owner = THIS_MODULE,
    .read = dr_read,
};

static int __init
dr_init(void)
{
	pr_info("Starting module\n");

	directory = debugfs_create_dir("rpi", NULL);
	if (directory == NULL)
		return -ENOENT;

	file = debugfs_create_file("regs", 0644, directory, NULL, &dr_fops);
	if (file == NULL) {
		debugfs_remove_recursive(directory);
		return -ENOENT;
	}

	return 0;
}

static void
dr_exit(void)
{
	pr_info("Exiting module\n");
	debugfs_remove_recursive(directory);
}

module_init(dr_init);
module_exit(dr_exit);

MODULE_AUTHOR("Quan Tran");
MODULE_DESCRIPTION("Dump Raspberry Pi registers in a horribly unsafe manner for debugging");
MODULE_LICENSE("GPL");

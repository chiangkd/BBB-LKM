#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/version.h>

#define MEM_SIZE 512
#define PSEUDO_DEIVCE_NAME "pseudo"

char pseudo_device_buf[MEM_SIZE];   // pseudo device
dev_t device_number;
struct device *pseudo_device;
struct cdev pseudo_cdev;


/* This is descriptive information about the module */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("aaron");
MODULE_DESCRIPTION("Hello Kernel");
MODULE_INFO(name, "string_value");

static loff_t pseudo_lseek(struct file *file, loff_t offset, int orig)
{
    return 0;
}
static ssize_t pseudo_read(struct file *file, char __user *buf, size_t size, loff_t *offset)
{
    return 0;
}
static ssize_t pseudo_write(struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
    return 0;
}
static int pseudo_release(struct inode *inode, struct file *file)
{
    return 0;
}
static int pseudo_open(struct inode *inode, struct file *file)
{
    return 0;
}

// File operations of the driver
struct file_operations pseudo_fops = {
    .owner = THIS_MODULE,
    .read = pseudo_read,
    .write = pseudo_write,
    .open = pseudo_open,
    .release = pseudo_release,
    .llseek = pseudo_lseek,
};

struct class *pseudo_class;

static int __init pseudo_init(void)
{
    // Dynamically allocate a device number
    alloc_chrdev_region(&device_number, 0, 1, PSEUDO_DEIVCE_NAME);

    // Initialize the cdev structure with fops
    cdev_init(&pseudo_cdev, &pseudo_fops);

    // Register a device (cdev structure) with VFS
    pseudo_cdev.owner = THIS_MODULE;
    cdev_add(&pseudo_cdev, device_number, 1);

    // Create device class under /sys/class
// #if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    pseudo_class = class_create(THIS_MODULE, PSEUDO_DEIVCE_NAME);
// #else
//     pseudo_class = class_create(PSEUDO_DEIVCE_NAME);
// #endif

    // Populate the sysfs with device information
    pseudo_device = device_create(pseudo_class, NULL, device_number, NULL, PSEUDO_DEIVCE_NAME);

    return 0;
}

static void __exit pseudo_exit(void)
{

}

module_init(pseudo_init);
module_exit(pseudo_exit);


/*

struct file_operations {
	struct module *owner;
	fop_flags_t fop_flags;
	loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	ssize_t (*read_iter) (struct kiocb *, struct iov_iter *);
	ssize_t (*write_iter) (struct kiocb *, struct iov_iter *);
	int (*iopoll)(struct kiocb *kiocb, struct io_comp_batch *,
			unsigned int flags);
	int (*iterate_shared) (struct file *, struct dir_context *);
	__poll_t (*poll) (struct file *, struct poll_table_struct *);
	long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
	long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
	int (*mmap) (struct file *, struct vm_area_struct *);
	int (*open) (struct inode *, struct file *);
	int (*flush) (struct file *, fl_owner_t id);
	int (*release) (struct inode *, struct file *);
	int (*fsync) (struct file *, loff_t, loff_t, int datasync);
	int (*fasync) (int, struct file *, int);
	int (*lock) (struct file *, int, struct file_lock *);
	unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
	int (*check_flags)(int);
	int (*flock) (struct file *, int, struct file_lock *);
	ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
	ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
	void (*splice_eof)(struct file *file);
	int (*setlease)(struct file *, int, struct file_lease **, void **);
	long (*fallocate)(struct file *file, int mode, loff_t offset,
			  loff_t len);
	void (*show_fdinfo)(struct seq_file *m, struct file *f);
#ifndef CONFIG_MMU
	unsigned (*mmap_capabilities)(struct file *);
#endif
	ssize_t (*copy_file_range)(struct file *, loff_t, struct file *,
			loff_t, size_t, unsigned int);
	loff_t (*remap_file_range)(struct file *file_in, loff_t pos_in,
				   struct file *file_out, loff_t pos_out,
				   loff_t len, unsigned int remap_flags);
	int (*fadvise)(struct file *, loff_t, loff_t, int);
	int (*uring_cmd)(struct io_uring_cmd *ioucmd, unsigned int issue_flags);
	int (*uring_cmd_iopoll)(struct io_uring_cmd *, struct io_comp_batch *,
				unsigned int poll_flags);
} __randomize_layout;

*/
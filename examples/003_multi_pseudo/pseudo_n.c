#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>

#define NO_OF_DEVICES 4

#define MEM_SIZE_DEV1 1024
#define MEM_SIZE_DEV2 512
#define MEM_SIZE_DEV3 1024
#define MEM_SIZE_DEV4 512

#define RDONLY 	0x01
#define WRONLY 	0x10
#define RDWR	0x11

char pseudo_device_buf1[MEM_SIZE_DEV1];   // pseudo device
char pseudo_device_buf2[MEM_SIZE_DEV2];   // pseudo device
char pseudo_device_buf3[MEM_SIZE_DEV3];   // pseudo device
char pseudo_device_buf4[MEM_SIZE_DEV4];   // pseudo device

// Device private data structure
struct pseudo_dev_private_data {
	char *buffer;
	unsigned size;
	const char *serial_number;
	int perm;
	struct cdev pseudo_cdev;
};

// Driver private data structure
struct pseudo_drv_private_data {
	int total_devices;
	dev_t device_number;		// This holds the device number
	struct device *pseudo_device;
	struct class *pseudo_class;
	struct pseudo_dev_private_data pseudo_dev_data[NO_OF_DEVICES];
};

struct pseudo_drv_private_data pseudo_drv_data = {
	.total_devices = NO_OF_DEVICES,
	.pseudo_dev_data = {
		[0] = {
			.buffer = pseudo_device_buf1,
			.size = MEM_SIZE_DEV1,
			.serial_number = "PSEUDODEV1",
			.perm = RDONLY	/* RDONLY */
		},
		[1] = {
			.buffer = pseudo_device_buf2,
			.size = MEM_SIZE_DEV2,
			.serial_number = "PSEUDODEV2",
			.perm = WRONLY	/* WRONLY */
		},
		[2] = {
			.buffer = pseudo_device_buf3,
			.size = MEM_SIZE_DEV3,
			.serial_number = "PSEUDODEV3",
			.perm = RDWR	/* RDWR */
		},
		[3] = {
			.buffer = pseudo_device_buf4,
			.size = MEM_SIZE_DEV4,
			.serial_number = "PSEUDODEV4",
			.perm = RDWR	/* RDWR */
		},
	}
};

#define PSEUDO_DEIVCE_NAME "pseudo"


// Custumize print format
#ifdef pr_fmt
#undef pr_fmt
#define pr_fmt(fmt) "%s: " fmt, __func__
#endif



/* This is descriptive information about the module */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("aaron");
MODULE_DESCRIPTION("Hello Kernel");
MODULE_INFO(name, "string_value");

static loff_t pseudo_lseek(struct file *file, loff_t offset, int orig)
{
	struct pseudo_dev_private_data *pseudo_dev_data = (struct pseudo_dev_private_data *)file->private_data;
	int max_size = pseudo_dev_data->size;

	loff_t new_pos = 0;
	pr_info("lseek requested\n");
	pr_info("current file position = %lld\n", file->f_pos);

	switch (orig)
	{
		case SEEK_SET:
			new_pos = offset;
			break;
		case SEEK_CUR:
			new_pos = file->f_pos + offset;
			break;
		case SEEK_END:
			new_pos = max_size + offset;
			break;
		default:
			return -EINVAL;
	}
	if (new_pos > max_size) {
		new_pos = max_size;
	}
	if (new_pos < 0) {
		new_pos = 0;
	}
	file->f_pos = new_pos;
	
	pr_info("new file position = %lld\n", file->f_pos);

	return new_pos;
}

static ssize_t pseudo_read(struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	struct pseudo_dev_private_data *pseudo_dev_data = (struct pseudo_dev_private_data *)file->private_data;
	int max_size = pseudo_dev_data->size;

	pr_info("read requested for %zu bytes\n", size);
	pr_info("current file position = %lld\n", *offset);

	// Adjust the 'size'
	if ((*offset + size) > max_size) {
		size = max_size - *offset;
	}

	// Copy to user

	/**
	 Global data access should be serialized using mutual exclusion locks
	 to avoid race condition.
	*/	
	if (copy_to_user(buf, pseudo_dev_data->buffer + (*offset), size)) {
		return -EFAULT;
	};

	// Update the current file position
	*offset += size;

	pr_info("Number of bytes successfully read = %zu\n", size);
	pr_info("Updated file position = %lld\n", *offset);

	// Return number of bytes which have been successfully read
    return size;
}

static ssize_t pseudo_write(struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
	struct pseudo_dev_private_data *pseudo_dev_data = (struct pseudo_dev_private_data *)file->private_data;
	int max_size = pseudo_dev_data->size;

	pr_info("write requested for %zu bytes\n", size);
	pr_info("current file position = %lld\n", *offset);

	// Adjust the 'size'
	if ((*offset + size) > max_size) {
		size = max_size - *offset;
	}

	if (size == 0) {
		pr_err("No space left on the device\n");
		return -ENOMEM;
	}

	// Copy from user
	if (copy_from_user(pseudo_dev_data->buffer + (*offset), buf, size)) {
		return -EFAULT;
	};

	// Update the current file position
	*offset += size;

	pr_info("Number of bytes successfully writen = %zu\n", size);
	pr_info("Updated file position = %lld\n", *offset);

	return size;
}

static int pseudo_release(struct inode *inode, struct file *file)
{
	pr_info("release was successful\n");
    return 0;
}


int check_permission(int perm, int acc_mode)
{
	if (perm == RDWR)
		return 0;

	if ((perm == RDONLY) && ((acc_mode & FMODE_READ) && !(acc_mode & FMODE_WRITE)))
		return 0;

	if ((perm == WRONLY) && (!(acc_mode & FMODE_READ) && (acc_mode & FMODE_WRITE)))
		return 0;
	
	return -EPERM;
}

static int pseudo_open(struct inode *inode, struct file *file)
{
	int rc;
	int minor_n;
	struct pseudo_dev_private_data *pseudo_dev_data;
	/** Find out on which device file open was attempted by the user space */
	minor_n = MINOR(inode->i_rdev);
	pr_info("minor access = %d\n", minor_n);
	/** Get device's private data structure  */
	pseudo_dev_data = container_of(inode->i_cdev, struct pseudo_dev_private_data, pseudo_cdev);
	
	/** To supply device private data to other methods of the driver */
	file->private_data = pseudo_dev_data;

	/** Check permission */
	rc = check_permission(pseudo_dev_data->perm, file->f_mode);

	if (!rc)
		pr_info("open was successful\n");
    else
		pr_info("open was fail\n");

	return rc;
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


static int __init pseudo_init(void)
{
	int rc = 0;
	int i = 0;

    // Dynamically allocate a device number
    rc = alloc_chrdev_region(&pseudo_drv_data.device_number, 0, NO_OF_DEVICES, PSEUDO_DEIVCE_NAME);

	if (rc < 0) {
		pr_err("alloc_chrdev_region fail\n");
		goto alloc_cdev_fail;
	}

	// Create device class under /sys/class
	pseudo_drv_data.pseudo_class = class_create(THIS_MODULE, PSEUDO_DEIVCE_NAME);
	if (IS_ERR(pseudo_drv_data.pseudo_class)) {
		pr_err("class_create fail\n");
		rc = PTR_ERR(pseudo_drv_data.pseudo_class);
		goto unreg_cdev;
	}

	for (i = 0; i < NO_OF_DEVICES; i++) {
		pr_info("Device number <major>:<minor> = %d:%d\n", MAJOR(pseudo_drv_data.device_number + i), MINOR(pseudo_drv_data.device_number + i));

		// Initialize the cdev structure with fops
		cdev_init(&pseudo_drv_data.pseudo_dev_data[i].pseudo_cdev, &pseudo_fops);

		// Register a device (cdev structure) with VFS
		pseudo_drv_data.pseudo_dev_data[i].pseudo_cdev.owner = THIS_MODULE;
		rc = cdev_add(&pseudo_drv_data.pseudo_dev_data[i].pseudo_cdev, pseudo_drv_data.device_number + i, 1);
		if (rc < 0) {
			pr_err("cdev_add fail\n");
			goto cleanup_cdev;
		}


		// Populate the sysfs with device information
		pseudo_drv_data.pseudo_device = device_create(pseudo_drv_data.pseudo_class, NULL, pseudo_drv_data.device_number + i, NULL, "%s%d", PSEUDO_DEIVCE_NAME, i);
		if (IS_ERR(pseudo_drv_data.pseudo_device)) {
			pr_err("device_create fail\n");
			rc = PTR_ERR(pseudo_drv_data.pseudo_device);
			goto cleanup_cdev;
		}

	}

	pr_info("Module init was successful\n");

    return rc;

cleanup_cdev:
	/** Clean the devices/cdev which has already create */
	while (--i >= 0) {
		device_destroy(pseudo_drv_data.pseudo_class, pseudo_drv_data.device_number + 1);
		cdev_del(&pseudo_drv_data.pseudo_dev_data[i].pseudo_cdev);
	}
	class_destroy(pseudo_drv_data.pseudo_class);
unreg_cdev:
	unregister_chrdev_region(pseudo_drv_data.device_number, NO_OF_DEVICES);
alloc_cdev_fail:
	return rc;
}

static void __exit pseudo_exit(void)
{
	int i = 0;
	for (i = 0; i < NO_OF_DEVICES; i++) {
		device_destroy(pseudo_drv_data.pseudo_class, pseudo_drv_data.device_number + i);
		cdev_del(&pseudo_drv_data.pseudo_dev_data[i].pseudo_cdev);
	}
	class_destroy(pseudo_drv_data.pseudo_class);
	unregister_chrdev_region(pseudo_drv_data.device_number, NO_OF_DEVICES);
	pr_info("module unloaded\n");
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
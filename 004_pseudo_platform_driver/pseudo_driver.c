#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/platform_device.h>

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
    return -ENOMEM;
}

static int pseudo_release(struct inode *inode, struct file *file)
{
	pr_info("release was successful\n");
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


// Gets called when the device is removed from the system
int pseudo_platform_driver_remove(struct platform_device *pdef)
{
    pr_info("A device is removed\n");
    return 0;
}


// Gets called when matched platform device is found
int pseudo_platform_driver_probe(struct platform_device *pdev)
{
    pr_info("A device is detected\n");
    return 0;
}



struct platform_driver pseudo_platform_driver = {
    .probe = pseudo_platform_driver_probe,
    .remove = pseudo_platform_driver_remove,
    .driver = {
        .name = "pseudo-char-device"
    }
};

static int __init pseudo_platform_driver_init(void)
{
    // Register the driver
    platform_driver_register(&pseudo_platform_driver);
    pr_info("pseudo platform driver loaded\n");

    return 0;
}

static void __exit pseudo_platform_driver_exit(void)
{
    platform_driver_unregister(&pseudo_platform_driver);
    pr_info("pseudo platform driver unloaded\n");
}

module_init(pseudo_platform_driver_init);
module_exit(pseudo_platform_driver_exit);

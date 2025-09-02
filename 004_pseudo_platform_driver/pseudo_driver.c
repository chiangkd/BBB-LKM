#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#include "platform.h"

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

// Driver private data structure
struct pseudo_drv_private_data {
	int total_devices;
	dev_t device_number_base;
	struct class *pseudo_class;
	struct device *pseudo_device;
};

// Device private data structure
struct pseudo_dev_private_data {
	struct pseudo_platform_data pdata;
	char *buffer;
	dev_t dev_num;
	struct cdev pseudo_cdev;
};

struct pseudo_drv_private_data pseudo_drv_data;


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
	int rc = 0;

	struct pseudo_dev_private_data *device_data;
	struct pseudo_platform_data *pdata;
	/** 1. Get the platform data */
	pdata = (struct pseudo_platform_data *) dev_get_platdata(&pdev->dev);
	if (!pdata) {
		pr_info("No platform data available\n");
		rc = -EINVAL;
		goto out;
	}
	/** 2. Dynamically allocate memory for the device private data */
	device_data = (struct pseudo_dev_private_data *) kzalloc(sizeof(struct pseudo_dev_private_data), GFP_KERNEL);
	if (!device_data) {
		pr_info("Cannot allocate memory\n");
		rc = -ENOMEM;
		goto out;
	}

	device_data->pdata.size = pdata->size;
	device_data->pdata.perm = pdata->perm;
	device_data->pdata.serial_number = pdata->serial_number;
	
	pr_info("Device serial number = %s\n", pdata->serial_number);
	pr_info("Device size = %d\n", pdata->size);
	pr_info("Device permission = %d\n", pdata->perm);

	/** 3. Dynamically allocate memory for the device buffer using suze
	 * information from the platform data */

	/** 4. Get the devuce number */

	/** 5. Do cdev init and cdev add */

	/** 6. Create device file for the detected platform device */

	/** 7. Error handling */

	
    pr_info("A device is detected\n");
    return 0;

out:
	pr_info("DEvice probe failed\n");
	return rc;
}


struct platform_driver pseudo_platform_driver = {
    .probe = pseudo_platform_driver_probe,
    .remove = pseudo_platform_driver_remove,
    .driver = {
        .name = "pseudo-char-device"
    }
};

#define MAX_DEVICES 10
static int __init pseudo_platform_driver_init(void)
{
	int rc = 0;
    /** 1. Dynamically allocate a device number for MAX_DEVICES */
	rc = alloc_chrdev_region(&pseudo_drv_data.device_number_base, 0, MAX_DEVICES, PSEUDO_DEIVCE_NAME);
	if (rc < 0) {
		pr_err ("Alloc chrdev failed\n");
		return rc;
	}
	
	/** 2. Create device class under /sys/class */
	pseudo_drv_data.pseudo_class = class_create(THIS_MODULE, PSEUDO_DEIVCE_NAME);
	if (IS_ERR(pseudo_drv_data.pseudo_class)) {
		pr_err("class_create fail\n");
		rc = PTR_ERR(pseudo_drv_data.pseudo_class);
		unregister_chrdev_region(pseudo_drv_data.device_number_base, MAX_DEVICES);
		return rc;
	}

	/** 3. Register a platform driver */
	platform_driver_register(&pseudo_platform_driver);
	pr_info("pseudo platform driver loaded\n");
    return 0;
}

static void __exit pseudo_platform_driver_cleanup(void)
{
	/** 1. Unregister the platform driver */
    platform_driver_unregister(&pseudo_platform_driver);

	/** 2. Class destroy */
	class_destroy(pseudo_drv_data.pseudo_class);
	/** 3. Unregister device number for MAX_DEVICES */
	unregister_chrdev_region(pseudo_drv_data.device_number_base, MAX_DEVICES);


    pr_info("pseudo platform driver unloaded\n");
}

module_init(pseudo_platform_driver_init);
module_exit(pseudo_platform_driver_cleanup);

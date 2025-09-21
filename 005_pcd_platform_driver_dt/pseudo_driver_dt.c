#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/mod_devicetable.h>
#include <linux/of.h>
#include <linux/of_device.h>
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

struct device_config {
	int config_item1;
	int config_item2;
};

enum pesudo_dev_names {
	PSEUDO_DEV_A1X,
	PSEUDO_DEV_B1X,
	PSEUDO_DEV_C1X,
	PSEUDO_DEV_D1X,
};

struct device_config pseudo_dev_config[] = {
	[PSEUDO_DEV_A1X] = {.config_item1 = 60, .config_item2 = 10},
	[PSEUDO_DEV_B1X] = {.config_item1 = 50, .config_item2 = 20},
	[PSEUDO_DEV_C1X] = {.config_item1 = 40, .config_item2 = 30},
	[PSEUDO_DEV_D1X] = {.config_item1 = 30, .config_item2 = 40},
};

struct platform_device_id pseudo_device_ids[] = {
	[0] = {.name = "pseudo-dev-A1x", .driver_data = PSEUDO_DEV_A1X},
	[1] = {.name = "pseudo-dev-B1x", .driver_data = PSEUDO_DEV_B1X},
	[2] = {.name = "pseudo-dev-C1x", .driver_data = PSEUDO_DEV_C1X},
	[2] = {.name = "pseudo-dev-D1x", .driver_data = PSEUDO_DEV_D1X},
	{}	// Null terminated entry
};

struct of_device_id pseudo_device_dt_match[] = {
	{.compatible = "pseudo-dev-A1x", .data = (void *)PSEUDO_DEV_A1X},
	{.compatible = "pseudo-dev-B1x", .data = (void *)PSEUDO_DEV_B1X},
	{.compatible = "pseudo-dev-C1x", .data = (void *)PSEUDO_DEV_C1X},
	{.compatible = "pseudo-dev-D1x", .data = (void *)PSEUDO_DEV_D1X},
	{},
};

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
int pseudo_platform_driver_remove(struct platform_device *pdev)
{
	struct pseudo_dev_private_data *device_data = dev_get_drvdata(&pdev->dev);
	/** 1. Remove a device that was created with device_create() */
	device_destroy(pseudo_drv_data.pseudo_class, device_data->dev_num);
	/** 2. Remove a cdev entyry from the system */
	cdev_del(&device_data->pseudo_cdev);

	pseudo_drv_data.total_devices--;

    dev_info(&pdev->dev, "A device is removed\n");
    return 0;
}

struct pseudo_platform_data *pseudo_device_get_platdata_from_dt(struct device *dev)
{
	struct device_node *dev_node = dev->of_node;
	struct pseudo_platform_data *pdata;
	if (!dev_node) {
		/* This probe did not happen because of device tree node */
		return NULL;
	}

	pdata = devm_kzalloc(dev, sizeof(struct pseudo_platform_data), GFP_KERNEL);
	if (!pdata) {
		dev_info(dev, "Cannot allocate memory\n");
		return ERR_PTR(-ENOMEM);
	}
	
	if (of_property_read_string(dev_node, "org,device-serial-number", &pdata->serial_number)) {
		dev_info(dev, "Missing serial number property\n");
		return ERR_PTR(-EINVAL);
	}

	if (of_property_read_u32(dev_node, "org,size", &pdata->size)) {
		dev_info(dev, "Missing size property\n");
		return ERR_PTR(-EINVAL);
	}

	if (of_property_read_u32(dev_node, "org,perm", &pdata->perm)) {
		dev_info(dev, "Missing perm property\n");
		return ERR_PTR(-EINVAL);
	}

	return pdata;
}

// Gets called when matched platform device is found
int pseudo_platform_driver_probe(struct platform_device *pdev)
{
	int rc = 0;

	struct pseudo_dev_private_data *device_data;
	struct pseudo_platform_data *pdata;
	struct device *dev = &pdev->dev;
	struct of_device_id *match;
	int driver_data;
	dev_info(dev, "A device is detected\n");

	/** 1. Get the platform data */


	pdata = pseudo_device_get_platdata_from_dt(dev);
	if (IS_ERR(pdata)) {
		return PTR_ERR(pdata);
	}

	if (!pdata) {
		// If pdata is NULL, means device instantiation did not happen.
		// Then, check device setup
		pdata = (struct pseudo_platform_data *) dev_get_platdata(dev);
		if (!pdata) {
			pr_info("No platform data available from device setup\n");
			return -EINVAL;
		}

		driver_data = pdev->id_entry->driver_data;
	} else {
		// Extract driver data from device tree node
		driver_data = (int) of_device_get_match_data(dev);
		// match = of_match_device(pdev->dev.driver->of_match_table, &pdev->dev);
		// driver_data = (int) match->data;
	}

	/** 2. Dynamically allocate memory for the device private data */
	device_data = devm_kzalloc(&pdev->dev, sizeof(struct pseudo_dev_private_data), GFP_KERNEL);
	if (!device_data) {
		pr_info("Cannot allocate memory\n");
		return -ENOMEM;
	}

	/** Save the device private data pointer in platform device structure */
	dev_set_drvdata(&pdev->dev, device_data);

	device_data->pdata.size = pdata->size;
	device_data->pdata.perm = pdata->perm;
	device_data->pdata.serial_number = pdata->serial_number;
	
	pr_info("Device serial number = %s\n", pdata->serial_number);
	pr_info("Device size = %d\n", pdata->size);
	pr_info("Device permission = %d\n", pdata->perm);

	// print driver data
	pr_info("Config item1 = %d\n", pseudo_dev_config[driver_data].config_item1);
	pr_info("Config item1 = %d\n", pseudo_dev_config[driver_data].config_item2);

	/** 3. Dynamically allocate memory for the device buffer using suze
	 * information from the platform data */
	device_data->buffer = devm_kzalloc(&pdev->dev, device_data->pdata.size, GFP_KERNEL);
	if (!device_data->buffer) {
		pr_info("Cannot allocate memory\n");
		return -ENOMEM;
	}
	/** 4. Get the device number */
	device_data->dev_num = pseudo_drv_data.device_number_base + pseudo_drv_data.total_devices;

	/** 5. Do cdev init and cdev add */
	cdev_init(&device_data->pseudo_cdev, &pseudo_fops);

	device_data->pseudo_cdev.owner = THIS_MODULE;
	rc = cdev_add(&device_data->pseudo_cdev, device_data->dev_num, 1);
	if (rc < 0) {
		pr_err("cdev_add fail\n");
		return rc;
	}

	/** 6. Create device file for the detected platform device */
	pseudo_drv_data.pseudo_device = device_create(pseudo_drv_data.pseudo_class, dev, device_data->dev_num, NULL, "%s%d", PSEUDO_DEIVCE_NAME, pseudo_drv_data.total_devices);
	if (IS_ERR(pseudo_drv_data.pseudo_device)) {
		pr_err("device_create fail\n");
		rc = PTR_ERR(pseudo_drv_data.pseudo_device);
		cdev_del(&device_data->pseudo_cdev);
		return rc;
	}

	pseudo_drv_data.total_devices++;

	pr_info("The probe was successful\n");

    return 0;
}


struct platform_driver pseudo_platform_driver = {
    .probe = pseudo_platform_driver_probe,
    .remove = pseudo_platform_driver_remove,
	// .id_table = pseudo_device_ids,
    .driver = {
        .name = "pseudo-char-device",
		.of_match_table = pseudo_device_dt_match
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

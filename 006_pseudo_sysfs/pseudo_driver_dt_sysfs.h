#ifndef _PSEUDO_DRIVER_DT_SYSFS_H_
#define _PSEUDO_DRIVER_DT_SYSFS_H_

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


int check_permission(int perm, int acc_mode);
loff_t pseudo_lseek(struct file *file, loff_t offset, int orig);
ssize_t pseudo_read(struct file *file, char __user *buf, size_t size, loff_t *offset);
ssize_t pseudo_write(struct file *file, const char __user *buf, size_t size, loff_t *offset);
int pseudo_release(struct inode *inode, struct file *file);
int pseudo_open(struct inode *inode, struct file *file);

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

#endif

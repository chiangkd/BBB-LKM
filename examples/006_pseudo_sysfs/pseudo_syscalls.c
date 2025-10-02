#include "pseudo_driver_dt_sysfs.h"

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


loff_t pseudo_lseek(struct file *file, loff_t offset, int orig)
{
    return 0;
}

ssize_t pseudo_read(struct file *file, char __user *buf, size_t size, loff_t *offset)
{
    return 0;
}

ssize_t pseudo_write(struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
    return -ENOMEM;
}

int pseudo_release(struct inode *inode, struct file *file)
{
	pr_info("release was successful\n");
    return 0;
}

int pseudo_open(struct inode *inode, struct file *file)
{
    return 0;
}
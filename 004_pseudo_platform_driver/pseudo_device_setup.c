#include <linux/module.h>
#include <linux/platform_device.h>

#include "platform.h"

// Custumize print format
#ifdef pr_fmt
#undef pr_fmt
#define pr_fmt(fmt) "%s: " fmt, __func__
#endif

void pseudo_release(struct device *dev)
{
    pr_info("Device released\n");    
}

// Create 2 platform data
struct pseudo_platform_data pseudo_pdata[2] = {
    [0] = {.size = 512, .perm = RDWR, .serial_number = "PSEUDO_1"},
    [1] = {.size = 512, .perm = RDWR, .serial_number = "PSEUDO_2"},
};

// Create 2 platform devices
struct platform_device platform_pseudo_dev_1 = {
    .name = "pseudo-char-device",
    .id = 0,
    .dev = {
        .platform_data = &pseudo_pdata[0],
        .release = pseudo_release,
    }
};

struct platform_device platform_pseudo_dev_2 = {
    .name = "pseudo-char-device",
    .id = 1,
    .dev = {
        .platform_data = &pseudo_pdata[1],
        .release = pseudo_release,
    }
};

static int __init pseudo_platform_init(void)
{
    // register paltform device
    platform_device_register(&platform_pseudo_dev_1);
    platform_device_register(&platform_pseudo_dev_2);

    pr_info("Device setup module inserted\n");
    return 0;
}

static void __exit pseudo_platform_exit(void)
{
    platform_device_unregister(&platform_pseudo_dev_1);
    platform_device_unregister(&platform_pseudo_dev_2);

    pr_info("Device set module removed\n");
}

module_init(pseudo_platform_init);
module_exit(pseudo_platform_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module which registers platform devices");


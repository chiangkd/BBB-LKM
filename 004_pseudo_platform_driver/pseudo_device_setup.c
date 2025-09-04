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
struct pseudo_platform_data pseudo_pdata[] = {
    [0] = {.size = 512, .perm = RDWR, .serial_number = "PSEUDO_1"},
    [1] = {.size = 1024,.perm = RDWR, .serial_number = "PSEUDO_2"},
    [2] = {.size = 128, .perm = RDONLY, .serial_number = "PSEUDO_3"},
    [3] = {.size = 32,  .perm = WRONLY, .serial_number = "PSEUDO_4"},
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

struct platform_device platform_pseudo_dev_3 = {
    .name = "pseudo-char-device",
    .id = 2,
    .dev = {
        .platform_data = &pseudo_pdata[2],
        .release = pseudo_release,
    }
};

struct platform_device platform_pseudo_dev_4 = {
    .name = "pseudo-char-device",
    .id = 3,
    .dev = {
        .platform_data = &pseudo_pdata[3],
        .release = pseudo_release,
    }
};

struct platform_device *platform_pseudo_devs[] = {
    &platform_pseudo_dev_1,
    &platform_pseudo_dev_2,
    &platform_pseudo_dev_3,
    &platform_pseudo_dev_4,
};

static int __init pseudo_platform_init(void)
{
    // register paltform device
    // platform_device_register(&platform_pseudo_dev_1);
    // platform_device_register(&platform_pseudo_dev_2);

    platform_add_devices(platform_pseudo_devs, ARRAY_SIZE(platform_pseudo_devs));

    pr_info("Device setup module loaded\n");
    return 0;
}

static void __exit pseudo_platform_exit(void)
{
    platform_device_unregister(&platform_pseudo_dev_1);
    platform_device_unregister(&platform_pseudo_dev_2);
    platform_device_unregister(&platform_pseudo_dev_3);
    platform_device_unregister(&platform_pseudo_dev_4);

    pr_info("Device set module unloaded\n");
}

module_init(pseudo_platform_init);
module_exit(pseudo_platform_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module which registers platform devices");


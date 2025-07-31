#include <linux/module.h>

/* This is descriptive information about the module */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("aaron");
MODULE_DESCRIPTION("Hello Kernel");
MODULE_INFO(name, "string_value");

/* This is module initialization entry point */
static int __init my_kernel_module_init(void)   // module initialization function
{
    /* kernel's printf */
    pr_info("Hello kernel! \n");
    return 0;
}

/* This is module clean-up entry point */
static void __exit my_kernel_module_exit(void)  // module clean-up function
{
    pr_info("Good by kernel!\n");
}


/* This is registration of above entry points with kernel */
module_init(my_kernel_module_init);
module_exit(my_kernel_module_exit);
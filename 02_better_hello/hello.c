// Linux kernel headers are in the "linux" subfolder.
// Linux kernel headers contain all the header and make files that are needed to build a Linux kernel module.
#include <linux/module.h>
#include <linux/init.h>

/**
 * @brief Callback function for when the module is loaded into the kernel.
 * 
 * @return Zero if the loading of the module was successful.
 */
static int __init my_init(void) {
    // Can't use stdout because there is no stdout for the Linux kernel. 
    // We will instead write to the kernel's log.
    printk(KERN_INFO "hello - Hello, Kernel!\n");
    return 0;
}

/**
 * @brief Callback function for when the module is removed from the kernel.
 * @details
 * Declaring this function as static:
 *   • Limits their visibility and linkage.
 *   • Can't call this function from outside this source file.
 *   • Makes this function only available within this kernel module.
 */
static void __exit my_exit(void) {
    printk(KERN_INFO "hello - Goodbye, Kernel!\n");
}

// Specify the function to use when the module is loaded into the kernel.
module_init(my_init);

// Specify the function to use when the module is removed from the kernel.
module_exit(my_exit);

// Specify the license of this kernel module.
// Some Linux distributions only allow you to use free/open source kernel modules.
MODULE_LICENSE("GPL");

// Specify the metadata of this kernel module.
MODULE_AUTHOR("Preston");
MODULE_DESCRIPTION("A simple Hello World Linux kernel module");

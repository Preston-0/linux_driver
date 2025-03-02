// Linux kernel headers are in the "linux" subfolder.
// Linux kernel headers contain all the header and make files that are needed to build a Linux kernel module.
#include <linux/module.h>
#include <linux/init.h>

// Callback function for when the module is loaded into the kernel.
// The return value will determine if the loading of the module was successful.
int my_init(void) {
    // Can't use stdout because there is no stdout for the Linux kernel. 
    // We will instead write to the kernel's log.
    printk("Hello, Kernel!\n");
    return 0;
}

// Callback function for when the module is removed from the kernel.
void my_exit(void) {
    printk("Goodbye, Kernel!\n");
}

// Specify the function to use when the module is loaded into the kernel.
module_init(my_init);

// Specify the function to use when the module is removed from the kernel.
module_exit(my_exit);

// Specify the license of this kernel module.
MODULE_LICENSE("GPL");

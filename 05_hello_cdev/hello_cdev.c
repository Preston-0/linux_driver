// Linux kernel headers are in the "linux" subfolder.
// Linux kernel headers contain all the header and make files that are needed to build a Linux kernel module.
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

static int major_dev_num;  // Major device number that will be allocated by our kernel module.

static ssize_t my_read(struct file *f, char __user *u, size_t l, loff_t *o) {
    printk(KERN_INFO "hello_cdev - Read is called\n");
    return 0;
}

static struct file_operations fops = {
    // Overload the read function pointer.
    .read = my_read
};

// Callback function for when the module is loaded into the kernel.
// The return value will determine if the loading of the module was successful.
static int __init my_init(void) {
    // register_chrdev(): 
    //   will allocate device numbers, create a character device, and link the device numbers to the character device.
    //   1st arg is the major device number that it should allocate for the device number. If zero, it will search for and use a free device number.
    //   2nd arg is a label, which will appear in proc/devices.
    //   3rd arg is a pointer to the file operations, which should be supported by our character device.
    major_dev_num = register_chrdev(0, "hello_cdev", &fops);

    // Check for error while registering the character device.
    if (major_dev_num < 0) {
        printk(KERN_ERR "hello_cdev - Error registering chrdev\n");
        return major_dev_num;
    }

    // The registration of the character device worked.
    printk(KERN_INFO "hello_cdev - Major device number: %d\n", major_dev_num);
    return 0;
}

// Callback function for when the module is removed from the kernel.
// Declaring this as static makes this function only available within this kernel module.
static void __exit my_exit(void) {
    // Delete the character device and free the device numbers via unregister_chrdev().
    // unregister_chrdev()'s 2nd arg is the label that appears in proc/devices.
    unregister_chrdev(major_dev_num, "hello_cdev");
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
MODULE_DESCRIPTION("A sample driver for registering a character device");

// Linux kernel headers are in the "linux" subfolder.
// Linux kernel headers contain all the header and make files that are needed to build a Linux kernel module.
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>  // "fs" stands for "file system."
#include <linux/ioctl.h>

#include "ioctl_test.h"

/* Global variables */
static int major_dev_num;  // Major device number that will be allocated by our kernel module.
static int32_t answer = 42;  // Global variable for reading and writing.


static int my_open(struct inode *inode, struct file *filep) {
// • `struct inode` represents a file. We can get the major and minor device number of the
//    opened device file, among other things.
// • `struct file` represents an open file in the Linux kernel. This struct is created when
//    we're opening a file (before calling "open" callback function), and destroyed after
//    calling the release function. This only lives as long as the file is opened.
// • `file.f_mode` allows us to read back the permissions given to this file. Before a read or
//    write function is called, it checks if the permissions are there to read or write.
//    If not, the callback function specified in our driver is never called.

    // Print out the major and minor device numbers of the currently opened file.
    pr_info("ioctl_example - Major: %d, Minor %d\n", imajor(inode), iminor(inode));

    return 0;  // Indicate that opening the file was successful.
}

/**
 * @brief Callback function for when the module is removed from the kernel.
 * 
 * @return Return code.
 */
static int my_release(struct inode *inode, struct file *filep) {
    pr_info("ioctl_example - File is closed.\n");
    return 0;
}

/**
 * @brief 
 * 
 * @param[in] file: Our device file.
 * @param[in] cmd: The command.
 * @param[in] arg: Potential argument(s).
 * @return Return code.
 */
static long int my_ioctl(struct file *file, unsigned cmd, unsigned long arg) {
    struct mystruct test;

    // The function that will be executed depends on the command.
    // IOCtl is a very driver-specific function because on every device there are different
    // commands that are available.
    switch(cmd) {
        case WRITE_FROM_USER_TO_KERNEL:
            if (copy_from_user(&answer, (int32_t *) arg, sizeof(answer)))
                pr_err("ioctl_example - Error copying data from user!.\n");
            else
                pr_info("ioctl_example - Updated the answer to %d\n", answer);
            break;
        case WRITE_FROM_KERNEL_TO_USER:
            if (copy_to_user((int32_t *) arg, &answer, sizeof(answer)))
                pr_err("ioctl_example - Error copying data to user!.\n");
            else
                pr_info("ioctl_example - The answer was copied!\n");
            break;
        case GREETER:
            if (copy_from_user(&test, (struct mystruct *) arg, sizeof(test)))
                pr_err("ioctl_example - Error copying data from user!.\n");
            else
                pr_info("ioctl_example - %d greetings to %s\n", test.repeat, test.name);
            break;
    }

    return 0;
}

static struct file_operations fops = {
    // Set file operations function pointers to our own functions.
    .open = my_open,
    .release = my_release,
    .unlocked_ioctl = my_ioctl,
};

/**
 * @brief Callback function for when the module is loaded into the kernel.
 * 
 * @return Zero if the loading of the module was successful.
 */
static int __init my_init(void) {
    // register_chrdev(): 
    //   Will allocate device numbers, create a character device, and link the device numbers to the character device.
    //   • 1st arg is the major device number that it should allocate for the device number.
    //       • If zero, it will search for and use a free device number.
    //       • If non-zero, it will allocate all 255 minor device numbers for that major device number.
    //   • 2nd arg is a label, which will appear in `/proc/devices`.
    //   • 3rd arg is a pointer to the file operations, which should be supported by our character device.
    major_dev_num = register_chrdev(0, "ioctl_example", &fops);

    // Check for error while registering the character device.
    if (major_dev_num < 0) {
        pr_err("ioctl_example - Error registering chrdev\n");
        return major_dev_num;
    }

    // The registration of the character device worked.
    pr_info("ioctl_example - Major device number: %d\n", major_dev_num);
    return 0;
}

/**
 * @brief Callback function for when the module is removed from the kernel.
 * Declaring this function as static:
 *   • Limits their visibility and linkage.
 *   • Can't call this function from outside this source file.
 *   • Makes this function only available within this kernel module.
 * 
 * @return void
 */
static void __exit my_exit(void) {
    // Delete the character device and free the allocated device numbers via `unregister_chrdev()`.
    // `unregister_chrdev()`'s 2nd arg is the label that appears in `/proc/devices`.
    unregister_chrdev(major_dev_num, "ioctl_example");
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
MODULE_DESCRIPTION("A simple example for ioctl in a LKM.");

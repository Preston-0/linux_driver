// Linux kernel headers are in the "linux" subfolder.
// Linux kernel headers contain all the header and make files that are needed to build a Linux kernel module.
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>  // "fs" stands for "file system."

static int major_dev_num;  // Major device number that will be allocated by our kernel module.
static char text[64];


/**
 * @brief The `read()` callback function.
 * 
 * @param[in] filp: An opened file in the Linux kernel.
 * @param[in] user_buf: A user space buffer.
 * @param[in] len: Length of the buffer.
 * @param[inout] off: Our current file offset.
 * @return The number of bytes that were read successfully.
 */
static ssize_t my_read(struct file *filp, char __user *user_buf, size_t len, loff_t *off) {
    // With `struct file`, we can:
    //   • Access the permissions of this file.
    //   • Access the file operations which were supported by this file.
    //   • Use the private data.
    // We want the `user_buf` to have the data after we are done with the `read` function.
    int num_bytes_not_copied;
    int bytes_copied;

    // The maximum bytes we can copy is what is remaining in the `text` buffer.
    int num_bytes_to_copy = (len + *off) < sizeof(text) ? len : (sizeof(text) - *off);

    pr_info("hello_cdev - Read is called, we want to read %ld bytes, but actually read %d bytes. The offset is %lld.\n", len, num_bytes_to_copy, *off);

    // We can't read any bytes if the offset is at or beyond the end of the `text` buffer.
    if (*off >= sizeof(text))
        return 0;

    // Copy from our device driver `text` buffer into our user buffer (`user_buf`).
    num_bytes_not_copied = copy_to_user(user_buf, &text[*off], num_bytes_to_copy);

    bytes_copied = num_bytes_to_copy - num_bytes_not_copied;

    // Need to update our file offset based on the amount of bytes that were copied.
    *off += bytes_copied;

    // Print a warning if any bytes were not copied.
    if (num_bytes_not_copied)
        pr_warn("hello_cdev - Could only copy %d bytes\n.", bytes_copied);

    return bytes_copied;
}

/**
 * @brief The `write()` callback function.
 * 
 * @param[in] filp: An opened file in the Linux kernel.
 * @param[in] user_buf: A user space buffer.
 * @param[in] len: Length of the buffer.
 * @param[inout] off: Our current file offset.
 * @return The number of bytes that were written successfully.
 */
static ssize_t my_write(struct file *filp, const char __user *user_buf, size_t len, loff_t *off) {
    int num_bytes_not_copied;
    int bytes_copied;

    // The maximum bytes we can copy is what is remaining in the `text` buffer.
    int num_bytes_to_copy = (len + *off) < sizeof(text) ? len : (sizeof(text) - *off);

    pr_info("hello_cdev - Write is called, we want to write %ld bytes, but actually wrote %d bytes. The offset is %lld.\n", len, num_bytes_to_copy, *off);

    // We can't read any bytes if the offset is at or beyond the end of the `text` buffer.
    if (*off >= sizeof(text))
        return 0;

    // Copy from user buffer (`user_buf`) to our device driver `text` buffer.
    num_bytes_not_copied = copy_from_user(&text[*off], user_buf, num_bytes_to_copy);

    bytes_copied = num_bytes_to_copy - num_bytes_not_copied;

    // Print a warning if any bytes were not copied.
    if (num_bytes_not_copied)
        pr_warn("hello_cdev - Could only copy %d bytes\n.", bytes_copied);

    // Need to update our file offset based on the amount of bytes that were copied.
    *off += bytes_copied;

    return bytes_copied;
}

static struct file_operations fops = {
    // Set file operations function pointers to our own functions.
    .read = my_read,  // The `read` callback function.
    .write = my_write,  // The `write` callback function.
};

// Callback function for when the module is loaded into the kernel.
// The return value will determine if the loading of the module was successful.
static int __init my_init(void) {
    // register_chrdev(): 
    //   Will allocate device numbers, create a character device, and link the device numbers to the character device.
    //   • 1st arg is the major device number that it should allocate for the device number.
    //       • If zero, it will search for and use a free device number.
    //       • If non-zero, it will allocate all 255 minor device numbers for that major device number.
    //   • 2nd arg is a label, which will appear in `/proc/devices`.
    //   • 3rd arg is a pointer to the file operations, which should be supported by our character device.
    major_dev_num = register_chrdev(0, "hello_cdev", &fops);

    // Check for error while registering the character device.
    if (major_dev_num < 0) {
        pr_err("hello_cdev - Error registering chrdev\n");
        return major_dev_num;
    }

    // The registration of the character device worked.
    pr_info("hello_cdev - Major device number: %d\n", major_dev_num);
    return 0;
}

// Callback function for when the module is removed from the kernel.
// Declaring this function as static:
//   • Limits their visibility and linkage.
//   • Can't call this function from outside this source file.
//   • Makes this function only available within this kernel module.
static void __exit my_exit(void) {
    // Delete the character device and free the allocated device numbers via `unregister_chrdev()`.
    // `unregister_chrdev()`'s 2nd arg is the label that appears in `/proc/devices`.
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

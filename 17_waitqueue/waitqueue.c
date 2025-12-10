// Linux kernel headers are in the "linux" subfolder.
// Linux kernel headers contain all the header and make files that are needed to build a Linux kernel module.
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>  // Provides all the functions needed for thread handling.
#include <linux/sched.h>  // Scheduler.
#include <linux/delay.h>  // We'll use some delay functions in our threads.
#include <linux/wait.h>
#include <linux/jiffies.h>  // Allows us to do a wait with a timeout.

/* Global variables */
#define MAJOR_DEV_NUM 64  // Major device number that will be allocated by our kernel module.
static struct task_struct *kthread_1;
static struct task_struct *kthread_2;
static int t1_data = 1, t2_data = 2;  // Data to be passed to the threads' functions.
static long int watch_var = 0;  // Used to monitor with the waitqueues.
DECLARE_WAIT_QUEUE_HEAD(wq1);  // Static declaration of a waitqueue (will already be initialized).
static wait_queue_head_t wq2;  // Dynamic declaration of a waitqueue.

/* Function prototypes */
int thread_function(void * thread_num);


/**
 * @brief This function will be executed by the threads.
 *
 * @param[in] thread_num: Thread number. Each thread has a different wait function.
 *
 * @return Return code.
 */
int thread_function(void * thread_num) {
    int thread_selection = *(int *) thread_num;
    pr_info("waitqueue - `thread_selection` == %d\n", thread_selection);

    switch(thread_selection) {
        case 1:
            // If the condition (`watch_var == 11`) is false then it will go to sleep. It will
            // sleep forever as long as the condition is false. The condition is checked each time
            // the waitqueue is woken up (via the `wake_up()` function).
            wait_event(wq1, watch_var == 11);

            // We will get here once the condition (`watch_var == 11`) is true.
            pr_info("waitqueue - `watch_var` is now 11!\n");
            break;

        case 2:
            // Use `wait_event_timeout()` if we want to wait a maximum amount of time.
            // If the condition is still not met after the maxiumum specified amount of time,
            // then it will return zero. If the timeout elapsed and the condition is met, then it
            // will return a 1. If the function is woken up with the `wake_up()` function and the
            // condition is met, it will return the remaining time (jiffies) from the timeout.
            // In this case, let's wait a maxiumum of 5 seconds.
            while (wait_event_timeout(wq2, watch_var == 22, msecs_to_jiffies(5000)) == 0)
                pr_info("waitqueue - `watch_var` is still not 22, but timeout elapsed!\n");

            // We will get here once the condition (`watch_var == 22`) is true.
            pr_info("waitqueue - `watch_var` is now 22!\n");
            break;

        default:
            pr_info("waitqueue - Default thread_selection.\n");
            break;
    }

    pr_info("waitqueue - Thread monitoring waitqueue #%d finished execution.\n", thread_selection);

    return 0;  // Indicate the function has executed correctly.
}

/**
 * @brief The `write()` callback function. Writes from user space to kernel space.
 *
 * In this kernel module, the value written to our device is what we will store in `watch_var`.
 *
 * @param[in] filp: An opened file in the Linux kernel.
 * @param[in] user_buf: A user space buffer.
 * @param[in] len: Length of the buffer.
 * @param[inout] off: Our current file offset.
 *
 * @return The number of bytes that were written successfully.
 *
 * @note We will ignnore the `loff_t *off` parameter.
 */
static ssize_t my_write(struct file *filp, const char __user *user_buf, size_t len, loff_t *off) {
    char buffer[16];

    pr_info("waitqueue - Write callback function called.\n");

    // Clean out `buffer` in case it's dirty.
    memset(buffer, 0, sizeof(buffer));

    // Get the amount of data to copy. We can't copy more then the size of `buffer`.
    int num_bytes_to_copy = min(len, sizeof(buffer));

    // Copy from user buffer (`user_buf`) to our kernel space `buffer`.
    int num_bytes_not_copied = copy_from_user(buffer, user_buf, num_bytes_to_copy);

    // Calculate how many bytes were copied.
    int bytes_copied = num_bytes_to_copy - num_bytes_not_copied;

    // Convert the string in `buffer` to a long integer and store it into `watch_var`.
    if (kstrtol(buffer, 10, &watch_var) == -EINVAL)
        // Print an error if the string conversion failed.
        pr_err("waitqueue - Error converting input!\n");
    else
        // The string conversion succeeded.
        pr_info("waitqueue - `watch_var` is now %ld.\n", watch_var);

    // Have the waitqueues check if their respective conditions are now met.
    wake_up(&wq1);
    wake_up(&wq2);

    return bytes_copied;
}

static struct file_operations fops = {
    // Set file operations function pointers to our own functions.
    .owner = THIS_MODULE,
    .write = my_write,  // The `write()` callback function.
};

/**
 * @brief Callback function for when the module is loaded into the kernel.
 *
 * @return Zero if the loading of the module was successful.
 */
static int __init my_init(void) {
    // Initialize our dynamically-created waitqueue.
    init_waitqueue_head(&wq2);

    // We will start to create and initialize the threads now.
    pr_info("waitqueue - Init threads.\n");

    // Register the device number.
    if (register_chrdev(MAJOR_DEV_NUM, "waitqueue", &fops)) {
        pr_err("waitqueue - Could not register the device number (%d)!\n", MAJOR_DEV_NUM);
        return -1;
    }

    pr_info("waitqueue - Device number %d successfully registered!\n", MAJOR_DEV_NUM);

    // Create and run `kthread_1`. `kthread_run()` will create a thread and run it.
    kthread_1 = kthread_run(thread_function, &t1_data, "kthread_1");

    // Check if `kthread_1` failed to be created.
    if (kthread_1 == NULL) {
        pr_err("waitqueue - Thread 1 could not be created!\n");
        unregister_chrdev(MAJOR_DEV_NUM, "waitqueue");  // Unregister our character device.
        return -1;  // We can't continue. Indicate that we failed.
    }
    else  // `kthread_1` has successfully been created and started.
        pr_info("waitqueue - Thread 1 was created and is now running.\n");

    // Create and run `kthread_2`. `kthread_run()` will create a thread and run it.
    kthread_2 = kthread_run(thread_function, &t2_data, "kthread_2");

    // Check if `kthread_2` failed to be created.
    if (kthread_2 == NULL) {
        pr_err("waitqueue - Thread 2 could not be created!\n");
        unregister_chrdev(MAJOR_DEV_NUM, "waitqueue");  // Unregister our character device.
        return -1;  // We can't continue. Indicate that we failed.
    }
    else  // `kthread_2` has successfully been created and started.
        pr_info("waitqueue - Thread 2 was created and is now running.\n");

    pr_info("waitqueue - Both threads are now running!\n");

    return 0;  // Indicate the function has executed correctly.
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
    // Make the wait function return for `wq1`.
    watch_var = 11;
    wake_up(&wq1);
    mdelay(10);

    // Make the wait function return for `wq2`.
    watch_var = 22;
    wake_up(&wq2);
    mdelay(10);

    // Stop both threads.
    pr_info("waitqueue - Stopping both threads...\n");
    kthread_stop(kthread_1);
    kthread_stop(kthread_2);

    // Unregister our character device.
    pr_info("waitqueue - Unregistering character device %d.\n", MAJOR_DEV_NUM);
    unregister_chrdev(MAJOR_DEV_NUM, "waitqueue");
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
MODULE_DESCRIPTION("A simple example for threads in a LKM.");

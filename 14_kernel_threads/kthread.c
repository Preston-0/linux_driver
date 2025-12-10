// Linux kernel headers are in the "linux" subfolder.
// Linux kernel headers contain all the header and make files that are needed to build a Linux kernel module.
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>  // Provides all the functions needed for thread handling.
#include <linux/sched.h>  // Scheduler.
#include <linux/delay.h>  // We'll use some delay functions in our threads.

/* Global variables */
static struct task_struct *kthread_1;
static struct task_struct *kthread_2;
static int t1_data = 1, t2_data = 2;  // Data to be passed to the threads' functions.


/**
 * @brief This function will be executed by the threads.
 * 
 * @param[in] thread_number: Number/identifier of the thread.
 * @return Return code.
 */
int thread_function(void * thread_number) {
    unsigned int i = 0;
    int thread_num = *(int *)thread_number;

    /* Working loop */
    while (!kthread_should_stop()) {
        pr_info("kthread - Thread %d has executed. Iteration #%u\n", thread_num, i++);
        msleep(thread_num * 1000);  // Millisecond sleep.
    }

    pr_info("kthread - Thread %d finished execution!\n", thread_num);

    return 0;  // Indicate the function has executed correctly.
}

/**
 * @brief Callback function for when the module is loaded into the kernel.
 * 
 * @return Zero if the loading of the module was successful.
 */
static int __init my_init(void) {
    // We will start to create and initialize the threads now.
    pr_info("kthread - Init threads\n");

    // Create "kthread_1" via `kthread_create()`:
    //   • 1st arg: the function that the thread should execute.
    //   • 2nd arg: pointer to the data that we will pass into the function that will be executed.
    //   • 3rd arg: string that will identify this thread.
    kthread_1 = kthread_create(thread_function, &t1_data, "kthread_1");

    // Check if "kthread_1" failed to be created.
    if (kthread_1 == NULL) {
        pr_err("kthread - Thread 1 could not be created!\n");
        return -1;
    }

    // Start "kthread_1."
    wake_up_process(kthread_1);
    pr_info("kthread - Thread 1 was created and is now running.\n");

    // Create and run "kthread_2." `kthread_run()` will create a thread and run it.
    kthread_2 = kthread_run(thread_function, &t2_data, "kthread_2");

    // Check if "kthread_2" failed to be created.
    if (kthread_2 == NULL) {
        pr_err("kthread - Thread 2 could not be created!\n");
        kthread_stop(kthread_1);  // Let's stop "kthread_1" if "kthread_2" failed.
        return -1;  // We can't continue. Indicate that we failed.
    }
    else  // Thread 2 has successfully been created and started.
        pr_info("kthread - Thread 2 was created and is now running.\n");

    pr_info("kthread - Both threads are now running!\n");

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
    // Stop both threads.
    pr_info("kthread - Stopping both threads...\n");
    kthread_stop(kthread_1);
    kthread_stop(kthread_2);
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

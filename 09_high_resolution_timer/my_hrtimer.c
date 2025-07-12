// Linux kernel headers are in the "linux" subfolder.
// Linux kernel headers contain all the header and make files that are needed to build a Linux kernel module.
#include <linux/module.h>
#include <linux/init.h>
#include <linux/hrtimer.h>
#include <linux/jiffies.h>

// Global variables.
static struct hrtimer my_hrtimer;
u64 start_time;

/**
 * @brief Timer expiry callback function.
 * 
 * @return If the timer should be restarted or not.
 */
static enum hrtimer_restart test_hrtimer_handler(struct hrtimer *timer) {
    // Get the current time. `jiffies` is a global variable which will count the processor ticks.
    u64 now_time = jiffies;

    // Calculate our delta time in milliseconds.
    u64 delta_time_in_msecs = jiffies_to_msecs(now_time - start_time);

    pr_info("my_hrtimer - delta_time_in_msecs = %llu.\n", delta_time_in_msecs);

    return HRTIMER_NORESTART;
}

/**
 * @brief Callback function for when the module is loaded into the kernel.
 * 
 * @return Zero if the loading of the module was successful.
 */
static int __init my_init(void) {
    // Can't use stdout because there is no stdout for the Linux kernel. 
    // We will instead write to the kernel's log.
    pr_info("my_hrtimer - Hello, Kernel!\n");

    // Initialize our high resolution timer.
    hrtimer_init(&my_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);

    // Set the timer expiry callback function. When our timer's time
    // has been reached, then our `test_hrtimer_handler()` function will be called.
    my_hrtimer.function = &test_hrtimer_handler;

    // Set the starting time to the current time.
    start_time = jiffies;

    // Start the timer with `hrtimer_start()`:
    // • 1st arg is the timer that will be started.
    // • 2nd arg is the amount of time to wait. Value passed to `ms_to_ktime()` is in milliseconds.
    // • 3rd arg is the timer mode. `HRTIMER_MODE_REL` is "relative" timer mode.
    hrtimer_start(&my_hrtimer, ms_to_ktime(100), HRTIMER_MODE_REL);

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
    // We don't want to remove the kernel module while the timer is still running.
    // No harm is done if the timer has already expired by the time we call `hrtimer_cancel()`.
    hrtimer_cancel(&my_hrtimer);

    pr_info("my_hrtimer - Goodbye, Kernel!\n");
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
MODULE_DESCRIPTION("A simple LKM using high resolution timers");

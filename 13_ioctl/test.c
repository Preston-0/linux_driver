#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // For open and close.
#include <fcntl.h>  // For the flags being associated with our character device.
#include <sys/ioctl.h>

#include "ioctl_test.h"

// This is a user space program.
int main(int argc, char **argv) {
    int fd;  // File descriptor.
    int answer;
    struct mystruct test = {4, "Preston"};

    {  /* Test #1 */
        // Open our device [driver] with read-only permissions.
        fd = open("/dev/mydevice", O_WRONLY);

        // Check if we couldn't open the file.
        if (fd < 0) {
            perror("Error opening file.");
            return fd;
        }

        printf("WRITE_FROM_USER_TO_KERNEL = %ld\n", WRITE_FROM_USER_TO_KERNEL);
        printf("WRITE_FROM_KERNEL_TO_USER = %ld\n", WRITE_FROM_KERNEL_TO_USER);
        printf("GREETER = %ld\n\n", GREETER);

        // Write default `answer` from kernel to user.
        ioctl(fd, WRITE_FROM_KERNEL_TO_USER, &answer);
        printf("Wrote `answer` from kernel to user.\n`answer` is: %d\n\n", answer);

        answer = 123;

        // Write new `answer` from user to kernel.
        ioctl(fd, WRITE_FROM_USER_TO_KERNEL, &answer);
        printf("Wrote `answer` from user to kernel.\n`answer` is: %d\n\n", answer);

        // Write new `answer` from kernel to user.
        ioctl(fd, WRITE_FROM_KERNEL_TO_USER, &answer);
        printf("Wrote `answer` from kernel to user.\n`answer` is: %d\n\n", answer);

        // Perform the greeting as specifed by `struct mystruct test`.
        ioctl(fd, GREETER, &test);
        printf("Performed greeting in the kernel log: {%d, %s}\n", test.repeat, test.name);

        close(fd);  // Close the file.
    }

    return 0;
}

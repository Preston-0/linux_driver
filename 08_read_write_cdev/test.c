#include <stdio.h>
#include <unistd.h>  // For open and close.
#include <fcntl.h>  // For the flags being associated with our character device.

// This is a user space program.
int main(int argc, char **argv) {
    int fd;  // File descriptor.
    char c;

    {  /* Test #1 */
        // Open our character device [driver] with read/write permissions.
        fd = open("/dev/hello0", O_RDWR);

        // Check if we couldn't open the file.
        if (fd < 0) {
            perror("Error opening file.");
            return fd;
        }

        // Keep reading 1 byte at a time until we reach the end.
        while(read(fd, &c, 1))
            putchar(c);  // Print the character that was read.

        close(fd);  // Close the file.
    }

    return 0;
}

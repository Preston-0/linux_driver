#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // For open and close.
#include <fcntl.h>  // For the flags being associated with our character device.

int main(int argc, char **argv) {
    int fd;  // File descriptor.

    // The first argument to this program is the file that should be opened and closed.
    if (argc < 2) {
        // We can't do anything if no file was passed as an agrument.
        printf("I need the file to open as an argument!\n");
        return 0;
    }

    {  // Test #1: Open the file (read-only).
        fd = open(argv[1], O_RDONLY);

        // Check if we couldn't open the file.
        if (fd < 0) {
            perror("Error opening file.");
            return fd;
        }

        close(fd);  // Close the file.
    }

    {  // Test #2: Open the file (read-write).
        fd = open(argv[1], O_RDWR | O_SYNC);

        // Check if we couldn't open the file.
        if (fd < 0) {
            perror("Error opening file.");
            return fd;
        }

        close(fd);  // Close the file.
    }

    {  // Test #3: Open the file (write-only).
        fd = open(argv[1], O_WRONLY | O_NONBLOCK);

        // Check if we couldn't open the file.
        if (fd < 0) {
            perror("Error opening file.");
            return fd;
        }

        close(fd);  // Close the file.
    }

    return 0;
}

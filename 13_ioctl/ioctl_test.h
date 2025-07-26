#ifndef IOCTL_TEST_H
#define IOCTL_TEST_H

struct mystruct {
    int repeat;
    char name[64];
};

// Write from the user space to the kernel space.
// First 2 args will be combined to a magic number, which will be our command's number.
// 3rd arg will be the type of argument we are passing.
#define WRITE_FROM_USER_TO_KERNEL _IOW('a', 'b', int32_t *)

// Write from the kernel space to the user space.
#define WRITE_FROM_KERNEL_TO_USER _IOR('a', 'b', int32_t *)

#define GREETER _IOR('a', 'c', struct mystruct *)

#endif  // #ifndef IOCTL_TEST_H
